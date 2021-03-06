#include "stdafx.h"
#include <windows.h>
#include <winnls.h>
#include <stdlib.h>
#include "ntfs.h"
#include <iostream>
#include <string>
using namespace std;

#if 0

ULONG BytesPerFileRecord; // 每个MFT entry的大小，通常为1K
HANDLE hVolume;           // 文件句柄
BOOT_BLOCK bootb;
PFILE_RECORD_HEADER MFT; // 存储MFT的内容
						 // Template for padding
template <class T1, class T2> inline T1* Padd(T1* p, T2 n)
{
	return (T1*)((char *)p + n);
}

ULONG RunLength(PUCHAR run)
{
	
	wprintf(L"In RunLength()...\n");
	return (*run & 0xf) + ((*run >> 4) & 0xf) + 1;
}

// 读起始簇号
LONGLONG RunLCN(PUCHAR run)
{
	LONG i = 0;
	UCHAR n1 = 0, n2 = 0;
	LONGLONG lcn = 0;

	wprintf(L"In RunLCN()...\n");
	// run字节的低4位表示连续多少簇
	n1 = *run & 0xf;
	// run字节的高4位表示起始逻辑簇LCN
	n2 = (*run >> 4) & 0xf;

	lcn = n2 == 0 ? 0 : CHAR(run[n1 + n2]);
	// 字节按 little endian 存放，低字节存放在低位，读起始簇号
	for (i = n1 + n2 - 1; i > n1; i--)
		lcn = (lcn << 8) + run[i];
	// 返回起始簇
	return lcn;
}

// 读簇数
ULONGLONG RunCount(PUCHAR run)
{
	UCHAR n = *run & 0xf;
	ULONGLONG count = 0;
	ULONG i;

	wprintf(L"In RunCount()...\n");

	for (i = n; i > 0; i--)
		count = (count << 8) + run[i];

	return count;
}

BOOL FindRun(PNONRESIDENT_ATTRIBUTE attr, ULONGLONG vcn, PULONGLONG lcn, PULONGLONG count)
{
	PUCHAR run = NULL;
	*lcn = 0;
	ULONGLONG base = attr->LowVcn;

	wprintf(L"In FindRun()...\n");

	if (vcn < attr->LowVcn || vcn > attr->HighVcn)
		return FALSE;

	for (run = PUCHAR(Padd(attr, attr->RunArrayOffset)); *run != 0; run += RunLength(run))
	{
		// 读起始簇号***************lcn重要***************
		*lcn += RunLCN(run);
		// 读簇数***************count重要***************
		*count = RunCount(run);

		if (base <= vcn && vcn < base + *count)
		{
			*lcn = RunLCN(run) == 0 ? 0 : *lcn + vcn - base;
			*count -= ULONG(vcn - base);
			return TRUE;
		}
		else
			base += *count;
	}
	return FALSE;
}

PATTRIBUTE FindAttribute(PFILE_RECORD_HEADER file, ATTRIBUTE_TYPE type, PWSTR name)
{
	PATTRIBUTE attr = NULL;

	wprintf(L"FindAttribute() - Finding attributes...\n");

	for (attr = PATTRIBUTE(Padd(file,file->AttributeOffset));
		attr->AttributeType != -1; attr = Padd(attr,attr->AttributeLength attr->Length))
	{
		if (attr->AttributeType == type)
		{
			if (name == 0 && attr->NameLength == 0)
				return attr;
			if (name != 0 && wcslen(name) == attr->NameLength && _wcsicmp(name,
				PWSTR(Padd(attr, attr->NameOffset))) == 0)
				return attr;
		}
	}
	return 0;
}

VOID FixupUpdateSequenceArray(PFILE_RECORD_HEADER file)
{
	ULONG i = 0;
	PUSHORT usa = PUSHORT(Padd(file, file->Ntfs.UsaOffset));
	PUSHORT sector = PUSHORT(file);

	wprintf(L"In FixupUpdateSequenceArray()...\n");
	for (i = 1; i < file->Ntfs.UsaCount; i++)
	{
		sector[255] = usa[i];
		sector += 256;
	}
}

VOID ReadSector(ULONGLONG sector, ULONG count, PVOID buffer)
{
	ULARGE_INTEGER offset;
	OVERLAPPED overlap = { 0 };
	ULONG n;

	wprintf(L"ReadSector() - Reading the sector...\n");
	wprintf(L"Sector: %lu\n", sector);

	offset.QuadPart = sector * bootb.BytesPerSector;
	overlap.Offset = offset.LowPart;
	overlap.OffsetHigh = offset.HighPart;
	ReadFile(hVolume, buffer, count * bootb.BytesPerSector, &n, &overlap);
}

// 读逻辑簇
VOID ReadLCN(ULONGLONG lcn, ULONG count, PVOID buffer)
{
	wprintf(L"\nReadLCN() - Reading the LCN, LCN: 0X%.8X\n", lcn);
	ReadSector(lcn * bootb.SectorsPerCluster, count * bootb.SectorsPerCluster, buffer);
}

// Non resident attributes
VOID ReadExternalAttribute(PNONRESIDENT_ATTRIBUTE attr, ULONGLONG vcn, ULONG count, PVOID buffer)
{
	ULONGLONG lcn, runcount;
	ULONG readcount, left;
	PUCHAR bytes = PUCHAR(buffer);

	wprintf(L"ReadExternalAttribute() - Reading the Non resident attributes...\n");

	for (left = count; left > 0; left -= readcount)
	{
		FindRun(attr, vcn, &lcn, &runcount);
		readcount = ULONG(min(runcount, left));
		// readcount * 簇字节
		ULONG n = readcount * bootb.BytesPerSector * bootb.SectorsPerCluster;

		if (lcn == 0)
			memset(bytes, 0, n);
		else
		{
			// 读连续readcount个簇的数据到内存空间bytes
			ReadLCN(lcn, readcount, bytes);
			wprintf(L"LCN: 0X%.8X\n", lcn);
		}
		vcn += readcount;
		bytes += n;
	}
}

ULONG AttributeLength(PATTRIBUTE attr)
{
	wprintf(L"In AttributeLength()...\n");
	return attr->Nonresident == FALSE ? PRESIDENT_ATTRIBUTE(attr)->ValueLength : ULONG(PNONRESIDENT_ATTRIBUTE(attr)->DataSize);
}

ULONG AttributeLengthAllocated(PATTRIBUTE attr)
{
	wprintf(L"\nIn AttributeLengthAllocated()...\n");
	return attr->Nonresident == FALSE ? PRESIDENT_ATTRIBUTE(attr)->ValueLength : ULONG(PNONRESIDENT_ATTRIBUTE(attr)->AllocatedSize);
}

VOID ReadAttribute(PATTRIBUTE attr, PVOID buffer)
{
	PRESIDENT_ATTRIBUTE rattr = NULL;
	PNONRESIDENT_ATTRIBUTE nattr = NULL;

	wprintf(L"ReadAttribute() - Reading the attributes...\n");

	if (attr->Nonresident == FALSE) // 常驻属性
	{
		wprintf(L"Resident attribute...\n");
		rattr = PRESIDENT_ATTRIBUTE(attr);
		memcpy(buffer, Padd(rattr, rattr->ValueOffset), rattr->ValueLength);
	}
	else // 非常驻属性
	{
		wprintf(L"Non-resident attribute...\n");
		nattr = PNONRESIDENT_ATTRIBUTE(attr);
		// 起始vcn = 0, count = ULONG(nattr->HighVcn) + 1, ULONG(nattr->HighVcn) + 1 表示总的虚拟簇数
		ReadExternalAttribute(nattr, 0, ULONG(nattr->HighVcn) + 1, buffer);
	}
}

// 读虚拟簇
VOID ReadVCN(PFILE_RECORD_HEADER file, ATTRIBUTE_TYPE type, ULONGLONG vcn, ULONG count, PVOID buffer)
{
	PATTRIBUTE attrlist = NULL;
	PNONRESIDENT_ATTRIBUTE attr = PNONRESIDENT_ATTRIBUTE(FindAttribute(file, type, 0));

	wprintf(L"In ReadVCN()...\n");
	if (attr == 0 || (vcn < attr->LowVcn || vcn > attr->HighVcn))
	{
		// Support for huge files
		attrlist = FindAttribute(file, AttributeAttributeList, 0);
		DebugBreak();
	}
	ReadExternalAttribute(attr, vcn, count, buffer);
}

// 将MFT记录号为index的文件记录读到内存空间file中
VOID ReadFileRecord(ULONG index, PFILE_RECORD_HEADER file)
{
	// 每条文件记录有多少个簇
	ULONG clusters = bootb.ClustersPerFileRecord;

	wprintf(L"ReadFileRecord() - Reading the file records..\n");
	// ？？？
	if (clusters > 0x80)
		clusters = 1;
	// 申请内存空间为clusters个簇（一条文件记录的簇或1个簇）大小的字节数
	PUCHAR p = new UCHAR[bootb.BytesPerSector* bootb.SectorsPerCluster * clusters];
	// MFT 记录号为 index 的记录，在MFT中的起始VCN
	ULONGLONG vcn = ULONGLONG(index) * BytesPerFileRecord / bootb.BytesPerSector / bootb.SectorsPerCluster;
	// 将起始VCN为vcn，连续clusters个簇读到内存空间p中（将index文件记录读到p中）？？？
	ReadVCN(MFT, AttributeData, vcn, clusters, p);
	// m = 每个簇可以包含的文件记录数 - 1
	LONG m = (bootb.SectorsPerCluster * bootb.BytesPerSector / BytesPerFileRecord) - 1;
	// 在MFT中记录号为index的文件记录在该簇中偏移的文件记录数
	ULONG n = m > 0 ? (index & m) : 0;
	// 将p（一个簇）中的偏移n个文件记录的那条文件记录数据复制到file中，大小为一个文件记录的大小
	memcpy(file, p + n * BytesPerFileRecord, BytesPerFileRecord);
	delete[] p;
	FixupUpdateSequenceArray(file);
}

VOID LoadMFT()
{
	wprintf(L"In LoadMFT() - Loading MFT...\n");

	BytesPerFileRecord = bootb.ClustersPerFileRecord < 0x80
		? bootb.ClustersPerFileRecord* bootb.SectorsPerCluster
		* bootb.BytesPerSector : 1 << (0x100 - bootb.ClustersPerFileRecord);

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
	wprintf(L"bootb.VolumeSerialNumber = 0X%.8X%.8X\n\n", bootb.VolumeSerialNumber.HighPart, bootb.VolumeSerialNumber.HighPart);

	// 分配内存空间，并将空间解释为 PFILE_RECORD_HEADER 类型
	MFT = PFILE_RECORD_HEADER(new UCHAR[BytesPerFileRecord]);
	// 读 $MFT 
	ReadSector((bootb.MftStartLcn)*(bootb.SectorsPerCluster), (BytesPerFileRecord) / (bootb.BytesPerSector), MFT);
	FixupUpdateSequenceArray(MFT);
}

BOOL bitset(PUCHAR bitmap, ULONG i)
{
	return (bitmap[i >> 3] & (1 << (i & 7))) != 0;
}

VOID FindDeleted()
{
	// 找MFT中第一个bitmap属性，即文件$MFT的$bitmap属性，显示哪个文件记录在用
	PATTRIBUTE attr = FindAttribute(MFT, AttributeBitmap, 0);
	PUCHAR bitmap = new UCHAR[AttributeLengthAllocated(attr)];
	// 将bitmap属性读到内存空间bitmap中
	ReadAttribute(attr, bitmap);
	// $MFT文件的$data属性就是MFT，n表示MFT有多少条文件记录
	ULONG n = AttributeLength(FindAttribute(MFT, AttributeData, 0)) / BytesPerFileRecord;

	wprintf(L"FindDeleted() - Finding the deleted files...\n");

	PFILE_RECORD_HEADER file = PFILE_RECORD_HEADER(new UCHAR[BytesPerFileRecord]);

	// 遍历MFT的文件记录，找到没在使用的文件记录
	for (ULONG i = 0; i < n; i++)
	{
		// 如果第i条文件记录正在使用，则继续查找下一条
		if (bitset(bitmap, i))
			continue;

		ReadFileRecord(i, file);

		// 如果文件记录以“FILE”开头，并且文件记录的Flags = 0，表示文件记录空闲，从未被使用？？？
		if (file->Ntfs.Type == 'ELIF' && (file->Flags & 1) == 0)
		{
			attr = FindAttribute(file, AttributeFileName, 0);
			if (attr == 0)
				continue;

			PFILENAME_ATTRIBUTE name = PFILENAME_ATTRIBUTE(Padd(attr, PRESIDENT_ATTRIBUTE(attr)->ValueOffset));

			// * means the width/precision was supplied in the argument list
			// ws ~ wide character string
			wprintf(L"\n%10u %u %.*s\n\n", i, int(name->NameLength), int(name->NameLength), name->Name);
			// To see the very long output short, uncomment the following line
			// _getwch();
		}
	}
}

VOID DumpData(ULONG index, WCHAR* filename)
{
	PATTRIBUTE attr = NULL;
	HANDLE hFile = NULL;
	PFILE_RECORD_HEADER file = PFILE_RECORD_HEADER(new UCHAR[BytesPerFileRecord]);
	ULONG n;

	// 将MFT记录号为index的文件记录读到内存空间file中
	ReadFileRecord(index, file);

	wprintf(L"Dumping the data...\n");

	if (file->Ntfs.Type != 'ELIF')
		return;

	// 在内存空间file中找80属性
	attr = FindAttribute(file, AttributeData, 0);
	if (attr == 0)
		return;
	// 申请80属性大小的内存空间
	PUCHAR buf = new UCHAR[AttributeLengthAllocated(attr)];
	// 将内存空间file中的80属性写到buf中
	ReadAttribute(attr, buf);

	// 创建文件filename
	hFile = CreateFile((LPCWSTR)filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		wprintf(L"CreateFile() failed, error %u\n", GetLastError());
		return;
	}
	// 将buf（80属性）写到filename文件中
	if (WriteFile(hFile, buf, AttributeLength(attr), &n, 0) == 0)
	{
		wprintf(L"WriteFile() failed, error %u\n", GetLastError());
		return;
	}

	CloseHandle(hFile);
	delete[] buf;
}
#endif



int main(int argc, char *argv[])
{
	string s = "你好";
	//::MultiByteToWideChar();
	cout << s <<s.size();
	 #if 0
	wchar_t    tc1[16];
	wchar_t    tc2[8] = { 0x540E, 0x6765, 0, 0, 0, 0, 0, 0 };
	//    memset(tc2, 0, 20);
	//  MultiByteToWideChar(CP_ACP, 0, tc1, 4, (LPWSTR)tc2, 4);
	WideCharToMultiByte(CP_ACP, 0, (WCHAR*)tc2, 2, LPSTR(tc1), sizeof(tc1), 0, 0);

	std::cout << "tc1 " << tc1 << sizeof(tc1) << " " << strlen((char*)tc1) << std::endl;
	//PrintHex(tc1);
	std::cout << std::endl;
	std::cout << "tc2 " << sizeof(tc2) << " " << wcslen((LPWSTR)tc2) << std::endl;


	WCHAR driver[] = L"\\\\.\\C:";
	ULONG n;

	if (argc < 2){
		wprintf(L"Usage:\n");
		wprintf(L"Find deleted files: %s <primary_partition>\n", argv[0]);
		wprintf(L"Read the file records: %s <primary_partition> <index> <file_name>\n", argv[0]);
		// Just exit
		exit(1);
	}
	driver[4] = *argv[1];
	hVolume = CreateFile(driver, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);

	if (hVolume == INVALID_HANDLE_VALUE){
		wprintf(L"CreateFile() failed, error %u\n", GetLastError());
		exit(1);
	}

	// Reads data from the specified input/output (I/O) device - volume/physical disk
	if (ReadFile(hVolume, &bootb, sizeof bootb, &n, NULL) == FALSE){
		wprintf(L"ReadFile() failed, error %u\n", GetLastError());
		exit(1);
	}

	LoadMFT();

	// The primary partition supplied else
	// default C:\ will be used
	
	if (argc == 2) FindDeleted();
	
	// Need to convert the recovered filename to long file name
	// Not implemented here. It is 8.3 file name format


	// The primary partition, index and file name to be recovered
	// are supplied
//strtoul(）函数用来将一个空终结符的字符串转化成unsigned long的形式。
	if (argc == 4)
		DumpData(strtoul(argv[2], 0, 0), argv[3]);

	CloseHandle(hVolume);
#endif
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