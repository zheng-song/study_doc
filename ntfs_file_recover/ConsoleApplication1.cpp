// ConsoleApplication1.cpp: 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <windows.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include "ntfs.h"

ULONG BytesPerFileRecord; 
HANDLE hVolume; 
BOOT_BLOCK bootb; 
PFILE_RECORD_HEADER MFT;

template <class T1, class T2> inline T1 * Padd(T1 *p, T2 n) { return (T1 *)((char *)p + n); }

ULONG RunLength(PUCHAR run) 
{ 
	return (*run & 0xf) + ((*run >> 4) & 0xf) + 1; 
}

LONGLONG RunLCN(PUCHAR run) 
{
	UCHAR n1 = *run & 0xf; 
	UCHAR n2 = (*run >> 4) & 0xf; 
	LONGLONG lcn = (n2 == 0 ? 0 : CHAR(run[n1 + n2]));

	for (LONG i = n1 + n2 - 1; i > n1; i--)
		lcn = (lcn << 8) + run[i];

	return lcn;
}

ULONGLONG RunCount(PUCHAR run) 
{
	UCHAR n = *run & 0xf; 
	ULONGLONG count = 0;

	for (ULONG i = n; i > 0; i--)
		count = (count << 8) + run[i];

	return count;

}

BOOL FindRun(PNONRESIDENT_ATTRIBUTE attr, ULONGLONG vcn, PULONGLONG lcn, PULONGLONG count) 
{
	if (vcn < attr->LowVcn || vcn > attr->HighVcn) 
		return FALSE;

	*lcn = 0;
	ULONGLONG base = attr->LowVcn;
	PUCHAR run = PUCHAR(Padd(attr, attr->RunArrayOffset));

	for (; *run != 0; run += RunLength(run));
	{
		*lcn += RunLCN(run);
		*count = RunCount(run);

		if (base <= vcn && vcn < base + *count)
		{
			*lcn = RunLCN(run) == 0 ? 0 : *lcn + vcn - base;
			*count -= ULONG(vcn - base);
			return TRUE;
		}
		else
		{
			base += *count;
		}
	}
	return FALSE;
}

PATTRIBUTE FindAttribute(PFILE_RECORD_HEADER file, ATTRIBUTE_TYPE type, PWSTR name) 
{
	PATTRIBUTE attr = PATTRIBUTE(Padd(file, file->AttributeOffset));
	for (; attr->AttributeType != -1; attr = Padd(attr, attr->Length)) {
		if (attr->AttributeType == type) 
		{
			if (name == 0 && attr->NameLength == 0) 
				return attr;

			if (name != 0 && wcslen(name) == attr->NameLength
				&& _wcsicmp(name, PWSTR(Padd(attr, attr->NameOffset))) == 0)
				return attr;
		}
	}
	return 0;
}

VOID FixUpdateSequenceArray(PFILE_RECORD_HEADER file)
{
	PUSHORT usa = PUSHORT(Padd(file, file->Ntfs.UsaOffset));
	PUSHORT sector = PUSHORT(file);

	for (ULONG i = 1; i < file->Ntfs.UsaCount; i++)
	{
		sector[255] = usa[i];
		sector += 256;
	}
}

VOID ReadSector(ULONGLONG sector, ULONG count ,PVOID buffer)
{
	ULARGE_INTEGER offset;
	OVERLAPPED overlap = { 0 };
	ULONG n;

	offset.QuadPart = sector * bootb.BytesPerSector;
	overlap.Offset = offset.LowPart;
	overlap.OffsetHigh = offset.HighPart;

	ReadFile(hVolume, buffer, count * bootb.BytesPerSector, &n, &overlap);
}

VOID ReadLCN(ULONGLONG lcn, ULONG count, PVOID buffer)
{
	ReadSector(lcn * bootb.SectorsPerCluster,
		count * bootb.SectorsPerCluster, buffer);
}

VOID ReadExtrenalAttribute(PNONRESIDENT_ATTRIBUTE attr, ULONGLONG vcn, ULONG count, PVOID buffer)
{
	ULONGLONG lcn, runcount;
	ULONG readcount, left;
	PUCHAR bytes = PUCHAR(buffer);

	for (left = count; left > 0; left -= readcount)
	{
		FindRun(attr, vcn, &lcn, &runcount);
		readcount = ULONG(min(runcount, left));
		ULONG n = readcount * bootb.BytesPerSector * bootb.SectorsPerCluster;

		if (lcn == 0)
			memset(bytes, 0, n);
		else
			ReadLCN(lcn, readcount, bytes);

		vcn += readcount;
		bytes += n;
	}
}

ULONG AttributeLength(PATTRIBUTE attr)
{
	return attr->Nonresident == FALSE
		? PRESIDENT_ATTRIBUTE(attr)->ValueLength
		: ULONG(PNONRESIDENT_ATTRIBUTE(attr)->DataSize);
}

ULONG AttributeLengthAllocated(PATTRIBUTE attr)
{
	return attr->Nonresident == FALSE
		? PRESIDENT_ATTRIBUTE(attr)->ValueLength
		: ULONG(PNONRESIDENT_ATTRIBUTE(attr)->AllocatedSize);
}


VOID ReadAttribute(PATTRIBUTE attr, PVOID buffer)
{
	if (attr->Nonresident == FALSE) {
		PRESIDENT_ATTRIBUTE rattr = PRESIDENT_ATTRIBUTE(attr);
		memcpy(buffer, Padd(rattr, rattr->ValueOffset), rattr->ValueLength);
	}
	else {
		PNONRESIDENT_ATTRIBUTE nattr = PNONRESIDENT_ATTRIBUTE(attr);
		ReadExtrenalAttribute(nattr, 0, ULONG(nattr->HighVcn) + 1, buffer);
	}
}

VOID ReadVCN(PFILE_RECORD_HEADER file, ATTRIBUTE_TYPE type,
	ULONGLONG vcn, ULONG count, PVOID buffer)
{
	PNONRESIDENT_ATTRIBUTE attr = PNONRESIDENT_ATTRIBUTE(FindAttribute(file, type, 0));

	if (attr == 0 || (vcn < attr->LowVcn || vcn > attr->HighVcn)) 
	{
		PATTRIBUTE attrlist = FindAttribute(file, AttributeAttributeList, 0);
		DebugBreak();
	}

	ReadExtrenalAttribute(attr, vcn, count, buffer);
}

VOID ReadFileRecord(ULONG index, PFILE_RECORD_HEADER file)
{
	ULONG clusters = bootb.ClustersPerFileRecord;
	if (clusters > 0x80) clusters = 1;

	PUCHAR p = new UCHAR[bootb.BytesPerSector * bootb.SectorsPerCluster * clusters];
	ULONGLONG vcn = ULONGLONG(index) * BytesPerFileRecord / bootb.BytesPerSector / bootb.SectorsPerCluster;
	ReadVCN(MFT, AttributeData, vcn, clusters, p);
	LONG m = (bootb.SectorsPerCluster * bootb.BytesPerSector / BytesPerFileRecord) - 1;
	ULONG n = m > 0 ? (index & m) : 0;
	memcpy(file, p + n * BytesPerFileRecord, BytesPerFileRecord);
	delete[]p;
	FixUpdateSequenceArray(file);
}


VOID LoadMFT()
{
	BytesPerFileRecord = bootb.ClustersPerFileRecord < 0x80
		? bootb.ClustersPerFileRecord*bootb.SectorsPerCluster*bootb.BytesPerSector
		: 1 << (0x100 - bootb.ClustersPerFileRecord);

	wprintf(L"\nBytes Per File Record = %u\n\n", BytesPerFileRecord);
	wprintf(L"======THESE INFO ARE NOT ACCURATE FOR DISPLAY LOL!=====\n");
	wprintf(L"bootb.BootSectors = %u\n", bootb.BootSectors);
	wprintf(L"bootb.BootSignature = %u\n", bootb.BootSignature);
	wprintf(L"bootb.BytesPerSector = %u\n", bootb.BytesPerSector);
	wprintf(L"bootb.ClustersPerFileRecord = %u\n", bootb.ClustersPerFileRecord);
	wprintf(L"bootb.ClustersPerIndexBlock = %u\n", bootb.ClustersPerIndexBlock);
	wprintf(L"bootb.Code = %u\n", bootb.Code);
	wprintf(L"bootb.Format = %u\n", bootb.Format);
	wprintf(L"bootb.Jump = %u\n", bootb.Jump);
	wprintf(L"bootb.Mbz1 = %u\n", bootb.Mbz1);
	wprintf(L"bootb.Mbz2 = %u\n", bootb.Mbz2);
	wprintf(L"bootb.Mbz3 = %u\n", bootb.Mbz3);
	wprintf(L"bootb.MediaType = 0X%X\n", bootb.MediaType);
	wprintf(L"bootb.Mft2StartLcn = 0X%.8X\n", bootb.Mft2StartLcn);
	wprintf(L"bootb.MftStartLcn = 0X%.8X\n", bootb.MftStartLcn);
	wprintf(L"bootb.NumberOfHeads = %u\n", bootb.NumberOfHeads);
	wprintf(L"bootb.PartitionOffset = %lu\n", bootb.PartitionOffset);
	wprintf(L"bootb.SectorsPerCluster = %u\n", bootb.SectorsPerCluster);
	wprintf(L"bootb.SectorsPerTrack = %u\n", bootb.SectorsPerTrack);
	wprintf(L"bootb.TotalSectors = %lu\n", bootb.TotalSectors);
	wprintf(L"bootb.VolumeSerialNumber = 0X%.8X%.8X\n\n",
		bootb.VolumeSerialNumber.HighPart, bootb.VolumeSerialNumber.HighPart);


	MFT = PFILE_RECORD_HEADER(new UCHAR[BytesPerFileRecord]);
	ReadSector(bootb.MftStartLcn*bootb.SectorsPerCluster,
		BytesPerFileRecord / bootb.BytesPerSector, MFT);
	FixUpdateSequenceArray(MFT);
}

BOOL bitset(PUCHAR bitmap, ULONG i)
{
	return (bitmap[i >> 3] & (i << (i & 7))) != 0;
}

VOID FindDeleted()
{
	PATTRIBUTE attr = FindAttribute(MFT, AttributeBitmap, 0);
	PUCHAR bitmap = new UCHAR[AttributeLengthAllocated(attr)];

	ReadAttribute(attr, bitmap);
	ULONG n = AttributeLength(FindAttribute(MFT, AttributeData, 0)) / BytesPerFileRecord;

	wprintf(L"FindDeleted() - Finding the deleted files...\n");

	PFILE_RECORD_HEADER file = PFILE_RECORD_HEADER(new UCHAR[BytesPerFileRecord]);

	for (ULONG i = 0; i < n; ++i)
	{
		if (bitset(bitmap, i))continue;
		
		ReadFileRecord(i, file);

		if (file->Ntfs.Type == 'ELIF' && (file->Flags & 1) == 0)
		{
			attr = FindAttribute(file, AttributeFileName, 0);
			if (attr == 0)continue;

			PFILENAME_ATTRIBUTE name = 
				PFILENAME_ATTRIBUTE(Padd(attr, PRESIDENT_ATTRIBUTE(attr)->ValueOffset));

			// * means the width/precision was supplied in the argument list
			// ws ~ wide character string
			wprintf(L"\n%10u %u %.*s\n\n", i, int(name->NameLength), int(name->NameLength), name->Name);
			// To see the very long output short, uncomment the following line
			// _getwch();
		}
	}
}


VOID DumpData(ULONG index, PCSTR fileName)
{
	PFILE_RECORD_HEADER file = PFILE_RECORD_HEADER(new UCHAR[BytesPerFileRecord]);
	ULONG n;
	ReadFileRecord(index, file);

	wprintf(L"Dumping the data...\n");

	if (file->Ntfs.Type != 'ELIF')return;

	PATTRIBUTE attr = FindAttribute(file, AttributeData, 0);
	if (attr == 0) return;

	PUCHAR buf = new UCHAR[AttributeLengthAllocated(attr)];
	ReadAttribute(attr, buf);

	HANDLE hFile = CreateFile(LPCWSTR(fileName), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
	if (hFile == INVALID_HANDLE_VALUE){
		wprintf(L"CreateFile() failed, error %u\n", GetLastError());
		return;
	}

	if (WriteFile(hFile, buf, AttributeLength(attr), &n, 0) == 0){
		wprintf(L"WriteFile() failed, error %u\n", GetLastError());
		return;
	}

	CloseHandle(hFile);
	delete []buf;
}

int main(int argc, char *argv[])
{
	CHAR driver[] = "\\\\.\\C:";
	ULONG n;

	if (argc < 2){
		wprintf(L"Usage:\n");
		wprintf(L"Find deleted files: %s <primary_partition>\n", argv[0]);
		wprintf(L"Read the file records: %s <primary_partition> <index> <file_name>\n", argv[0]);
		// Just exit
	//	exit(1);
	}
	driver[4] = argv[1][0];
	hVolume = CreateFile(LPCWSTR(driver), GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);

	if (hVolume == INVALID_HANDLE_VALUE){
		wprintf(L"CreateFile() failed, error %u\n", GetLastError());
		//exit(1);
	}

	// Reads data from the specified input/output (I/O) device - volume/physical disk
	if (ReadFile(hVolume, &bootb, sizeof bootb, &n, 0) == 0){
		wprintf(L"ReadFile() failed, error %u\n", GetLastError());
		//exit(1);
	}

	LoadMFT();

	// The primary partition supplied else
	// default C:\ will be used
	if (argc == 2) FindDeleted();
	// Need to convert the recovered filename to long file name
	// Not implemented here. It is 8.3 file name format


	// The primary partition, index and file name to be recovered
	// are supplied
	if (argc == 4)DumpData(strtoul(argv[2], 0, 0), argv[3]);

	CloseHandle(hVolume);
	return 0;
}


#if 0
#include "stdafx.h"
#include <Windows.h>
#include <winioctl.h>

bool ReadDisk(unsigned char *&out, DWORD start, DWORD size);
int main()
{

	ZwFsControlFile();

	HANDLE handle = CreateFile(L"F:\\work\\cp210x.c",
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		NULL,
		NULL);
	if (handle == INVALID_HANDLE_VALUE)
	{
		printf("INVALID_HANDLE_VALUE\n");
		return 0;
	}
	BY_HANDLE_FILE_INFORMATION FileInformation;

	if (GetFileInformationByHandle(handle, &FileInformation) == 0) 
	{
		printf("get file infomation ERROR!");
		return false;
	}
	printf("file serial number %u!\n", FileInformation.dwVolumeSerialNumber);
	printf("file attributes %u!\n", FileInformation.dwFileAttributes);
	//若文件较小，只需获取nFileSizeLow，若文件很大，还需获取nFileSizeHigh
	printf("filesize %u!\n", FileInformation.nFileSizeLow);

	unsigned char *a;
	bool status = ReadDisk(a, 0, 512);

	if (status) {
		for (int i = 0; i<512; i++)
		{
			printf("%02X", a[i]);
		}
	}
	else {
		printf("status is false\n");
	}

	getchar();
    return 0;
}

bool ReadDisk(unsigned char *&out, DWORD start, DWORD size)
{
	OVERLAPPED over = { 0 };
	over.Offset = start;

//编译之后要用管理员权限运行程序才能够读磁盘
	HANDLE handle = CreateFile(TEXT("\\\\.\\PHYSICALDRIVE0"),
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		NULL,
		NULL);
	if (handle == INVALID_HANDLE_VALUE)
	{
		printf("INVALID_HANDLE_VALUE\n");
		return false;
	}
	BY_HANDLE_FILE_INFORMATION FileInformation;


	

//	GetLogicalDriveStrings();
	
//	SetFilePointer(handle, );

	unsigned char *buffer = new unsigned char[size + 1];
	DWORD readsize;
	if (ReadFile(handle, buffer, size, &readsize, &over) == 0)
	{
		printf("ReadFile\n");
		CloseHandle(handle);
		return false;
	}
	buffer[size] = 0;
	out = buffer;
	return true;
}
#endif