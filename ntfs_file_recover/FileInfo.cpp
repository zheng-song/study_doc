
#include "stdafx.h"
#include "ntfs.h"
#include <windows.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>
using namespace std;

// ȫ�ֱ���
ULONG BytesPerFileRecord; // LoadMFT()
HANDLE hVolume;           // main()
BOOT_BLOCK bootb;         // main().ReadFile()
PFILE_RECORD_HEADER MFT; // LoadMFT()

// Template for padding
template <class T1, class T2> inline T1* Padd(T1* p, T2 n)
{
	return (T1*)((char *)p + n);
}

ULONG RunLength(PUCHAR run)
{
	return (*run & 0xf) + ((*run >> 4) & 0xf) + 1;
}

// ����ʼ�غ�
LONGLONG RunLCN(PUCHAR run)
{
	LONG i = 0;
	UCHAR n1 = 0, n2 = 0;
	LONGLONG lcn = 0;

	// run�ֽڵĵ�4λ��ʾ�������ٴ�
	n1 = *run & 0xf;
	// run�ֽڵĸ�4λ��ʾ��ʼ�߼���LCN
	n2 = (*run >> 4) & 0xf;

	lcn = n2 == 0 ? 0 : CHAR(run[n1 + n2]);
	// �ֽڰ� little endian ��ţ����ֽڴ���ڵ�λ������ʼ�غ�
	for (i = n1 + n2 - 1; i > n1; i--)
		lcn = (lcn << 8) + run[i];
	// ������ʼ��
	return lcn;
}

// ������
ULONGLONG RunCount(PUCHAR run)
{
	UCHAR n = *run & 0xf;
	ULONGLONG count = 0;
	ULONG i;

	for (i = n; i > 0; i--)
		count = (count << 8) + run[i];

	return count;
}

BOOL FindRun(PNONRESIDENT_ATTRIBUTE attr, ULONGLONG vcn, PULONGLONG lcn, PULONGLONG count)
{
	PUCHAR run = NULL;
	*lcn = 0;
	ULONGLONG base = attr->LowVcn;

	if (vcn < attr->LowVcn || vcn > attr->HighVcn)
		return FALSE;

	for (run = PUCHAR(Padd(attr, attr->RunArrayOffset)); *run != 0; run += RunLength(run))
	{
		// ����ʼ�غ�***************lcn��Ҫ***************
		*lcn += RunLCN(run);
		// ������***************count��Ҫ***************
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

	for (attr = PATTRIBUTE(Padd(file, file->AttributesOffset));
		attr->AttributeType != -1; attr = Padd(attr, attr->Length))
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

	for (i = 1; i < file->Ntfs.UsaCount; i++)
	{
		sector[255] = usa[i];
		sector += 256;
	}
}

// ���������ݵ�buffer
VOID ReadSector(ULONGLONG sector, ULONG count, PVOID buffer)
{
	ULARGE_INTEGER offset;
	OVERLAPPED overlap = { 0 };
	ULONG n;

	offset.QuadPart = sector * bootb.BytesPerSector;
	overlap.Offset = offset.LowPart;
	overlap.OffsetHigh = offset.HighPart;
	ReadFile(hVolume, buffer, count * bootb.BytesPerSector, &n, &overlap);
}

// ���߼���
VOID ReadLCN(ULONGLONG lcn, ULONG count, PVOID buffer)
{
	ReadSector(lcn * bootb.SectorsPerCluster, count * bootb.SectorsPerCluster, buffer);
}

// ���ǳ�פ����
VOID ReadExternalAttribute(PNONRESIDENT_ATTRIBUTE attr, ULONGLONG vcn, ULONG count, string clus, PVOID buffer)
{
	ULONGLONG lcn, runcount;
	ULONG readcount, left;
	PUCHAR bytes = PUCHAR(buffer);

	for (left = count; left > 0; left -= readcount)
	{
		FindRun(attr, vcn, &lcn, &runcount);
		readcount = ULONG(min(runcount, left));
		// readcount * ���ֽ�
		ULONG n = readcount * bootb.BytesPerSector * bootb.SectorsPerCluster;

		if (lcn == 0)
			memset(bytes, 0, n);
		else
		{
			printf("��ʼ��:%lld    ����:%ld\n", lcn, readcount);
			clus.append(to_string(lcn));  // ULONGLONG ת��Ϊ string
			clus.append(to_string(readcount));
			// ������readcount���ص����ݵ��ڴ�ռ�bytes
			ReadLCN(lcn, readcount, bytes);
		}
		vcn += readcount;
		bytes += n;
	}
}

ULONG AttributeLength(PATTRIBUTE attr)
{
	return attr->Nonresident == FALSE ? PRESIDENT_ATTRIBUTE(attr)->ValueLength : ULONG(PNONRESIDENT_ATTRIBUTE(attr)->DataSize);
}

ULONG AttributeLengthAllocated(PATTRIBUTE attr)
{
	return attr->Nonresident == FALSE ? PRESIDENT_ATTRIBUTE(attr)->ValueLength : ULONG(PNONRESIDENT_ATTRIBUTE(attr)->AllocatedSize);
}

VOID ReadAttribute(PATTRIBUTE attr, string clus, PVOID buffer)
{
	PRESIDENT_ATTRIBUTE rattr = NULL;
	PNONRESIDENT_ATTRIBUTE nattr = NULL;

	if (attr->Nonresident == FALSE) // ��פ����
	{
		rattr = PRESIDENT_ATTRIBUTE(attr);
		memcpy(buffer, Padd(rattr, rattr->ValueOffset), rattr->ValueLength); // ��������ͷ��ֻ��������
	}
	else // �ǳ�פ����
	{
		nattr = PNONRESIDENT_ATTRIBUTE(attr);
		// ��ʼvcn = 0, count = ULONG(nattr->HighVcn) + 1���ܵ����������
		ReadExternalAttribute(nattr, 0, ULONG(nattr->HighVcn) + 1, clus, buffer);
	}
}

// �������
VOID ReadVCN(PFILE_RECORD_HEADER file, ATTRIBUTE_TYPE type, ULONGLONG vcn, ULONG count, string clus, PVOID buffer)
{
	PATTRIBUTE attrlist = NULL;
	PNONRESIDENT_ATTRIBUTE attr = PNONRESIDENT_ATTRIBUTE(FindAttribute(file, type, 0));

	if (attr == 0 || (vcn < attr->LowVcn || vcn > attr->HighVcn))
	{
		// Support for huge files
		attrlist = FindAttribute(file, AttributeAttributeList, 0);
		DebugBreak();
	}
	ReadExternalAttribute(attr, vcn, count, clus, buffer);
}

// ��MFT��¼��Ϊindex���ļ���¼�����ڴ�ռ�file��
VOID ReadFileRecord(ULONG index, PFILE_RECORD_HEADER file)
{
	// ÿ���ļ���¼�ж��ٸ���
	ULONG clusters = bootb.ClustersPerFileRecord;

	if (clusters > 0x80)
		clusters = 1;
	// �����ڴ�ռ�Ϊclusters���أ�һ���ļ���¼�Ĵػ�1���أ���С���ֽ���
	PUCHAR p = new UCHAR[bootb.BytesPerSector* bootb.SectorsPerCluster * clusters];
	// MFT ��¼��Ϊ index �ļ�¼����MFT�е���ʼVCN
	ULONGLONG vcn = ULONGLONG(index) * BytesPerFileRecord / bootb.BytesPerSector / bootb.SectorsPerCluster;
	// ����ʼVCNΪvcn������clusters���ض����ڴ�ռ�p�У���index�ļ���¼����p�У�
	ReadVCN(MFT, AttributeData, vcn, clusters,0,p);
	// m = ÿ���ؿ��԰������ļ���¼�� - 1
	LONG m = (bootb.SectorsPerCluster * bootb.BytesPerSector / BytesPerFileRecord) - 1;
	// ��MFT�У���¼��Ϊindex���ļ���¼�ڸô���ƫ�Ƶ��ļ���¼��
	ULONG n = m > 0 ? (index & m) : 0;
	// ��p��һ���أ��е�ƫ��n���ļ���¼�������ļ���¼���ݸ��Ƶ�file�У���СΪһ���ļ���¼�Ĵ�С
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

	// �����ڴ�ռ䣬�����ռ����Ϊ PFILE_RECORD_HEADER ����
	MFT = PFILE_RECORD_HEADER(new UCHAR[BytesPerFileRecord]);
	// �� $MFT 
	ReadSector((bootb.MftStartLcn)*(bootb.SectorsPerCluster), (BytesPerFileRecord) / (bootb.BytesPerSector), MFT);
	FixupUpdateSequenceArray(MFT);
}

BOOL bitset(PUCHAR bitmap, ULONG i)
{
	return (bitmap[i >> 3] & (1 << (i & 7))) != 0;
}

VOID Initiate(WCHAR *disk)
{
	// Default primary partition
	WCHAR drive[] = L"\\\\.\\C:";
	ULONG n;
	// Read the user input
	drive[4] = *disk;
	// Get the handle to the primary partition/volume/physical disk
	hVolume = CreateFile(
		drive,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		0,
		OPEN_EXISTING,
		0,
		0);

	if (hVolume == INVALID_HANDLE_VALUE)
	{
		wprintf(L"CreateFile() failed, error %u\n", GetLastError());
		exit(1);
	}

	// Reads data from the specified input/output (I/O) device - volume/physical disk
	// ��boot sector�����ݣ�������ݽṹBOOT_BLOCK��ʵ��bootb��
	if (ReadFile(hVolume, &bootb, sizeof bootb, &n, 0) == 0)
	{
		wprintf(L"ReadFile() failed, error %u\n", GetLastError());
		exit(1);
	}

	LoadMFT();
}

VOID DumpClusNum(ULONG index, string clus, PVOID buf) 
{
	PATTRIBUTE attr = NULL;
	PFILE_RECORD_HEADER file = PFILE_RECORD_HEADER(new UCHAR[BytesPerFileRecord]);
	ReadFileRecord(index, file);// ����¼��Ϊindex���ļ���¼�����ڴ�ռ�file��
	if (file->Ntfs.Type != 'ELIF')
		return;
	attr = FindAttribute(file, AttributeData, 0); // ���ڴ�ռ�file����80����
	if (attr == 0)
		return;
	ReadAttribute(attr, clus, buf); // ���ڴ�ռ�file�е�80����д��buf��,ͬʱ���غ�д��clus
}

VOID DumpData(ULONGLONG lcn, ULONG readcount, PVOID buffer)
{
	ReadLCN(lcn, readcount, buffer);  // ���أ�����buffer
}

WCHAR* CharToWchar(WCHAR* m_wchar, LPCSTR c)
{
	int len = MultiByteToWideChar(CP_ACP, 0, c, strlen(c), NULL, 0);
	m_wchar = new WCHAR[len + 1];
	MultiByteToWideChar(CP_ACP, 0, c, strlen(c), m_wchar, len);
	m_wchar[len] = '\0';
	return m_wchar;
}

WCHAR* StringToWchar(const string& str)
{
	WCHAR* m_wchar = NULL;
	LPCSTR p = str.c_str();
	return CharToWchar(m_wchar, p);
}

// ����Ŀ¼�����ļ������������������Ŀ¼���ļ���MFT��¼��
ULONG TraverseIndexEntry(ATTRIBUTE_TYPE type, PUCHAR buf, WCHAR *dir)
{
	if (type == AttributeIndexRoot) // ����90�����ڵ������buf����90���Դӵ�һ�������ʼ�������壬ֱ�ӱ���������
	{

	}
	else if (type == AttributeIndexAllocation) { 
	/* ����A0����ָ��������ڵ��е������buf����һ������INDX��ʼ�������ڵ㣨�أ� ���Ȱ������ڵ������
	���ڽڵ��ڲ�������������������������СΪ0x10��0x18ʱ��ʾ�ýڵ���������ѱ����꣬���ű�����һ���ڵ㣨�أ�*/

	}
}

VOID FindMFTRecNum(string filepath)
{
	// string -> const char* -> char*
	LPSTR cpath = new CHAR[filepath.length() + 1];
	strcpy_s(cpath, strlen(cpath), filepath.c_str());

	// �ֽ�·����Ŀ¼���ļ���������WCHAR **dir��
	vector<string> str_vec;
	CHAR seps[] = "\\";
	PCHAR next_token = NULL;
	PCHAR token = NULL;
	token = strtok_s(cpath, seps, &next_token);
	while ((token != NULL))
	{
		str_vec.push_back(token);
		token = strtok_s(NULL, seps, &next_token);
	}
	WCHAR **dir = new WCHAR *[str_vec.size()];
	for (int i = 0; i < str_vec.size(); i++)
	{
		dir[i] = StringToWchar(str_vec[i]);
		wprintf(L"%x\n", dir[i]);
	}

	// LoadMFT
	Initiate(dir[0]);

	// ����Ŀ¼�����ļ���
	ULONG index = 5;
	PFILE_RECORD_HEADER file = NULL;
	PATTRIBUTE attr90 = NULL;
	for (int i = 1; i < str_vec.size(); i++)
	{
		file = PFILE_RECORD_HEADER(new UCHAR[BytesPerFileRecord]);
		ReadFileRecord(index, file); // ��MFT��¼��Ϊindex���ļ���¼�����ڴ�ռ�file��
		if (file->Ntfs.Type != 'ELIF')
			return;
		attr90 = FindAttribute(file, AttributeIndexRoot, 0); // ���ڴ�ռ�file����90����
		if (attr90 == 0)
			return;
		PUCHAR buffer1 = new UCHAR[AttributeLengthAllocated(attr90)]; // ����90���Դ�С���ڴ�ռ�
		ReadAttribute(attr90, 0, buffer1); // ���ڴ�ռ�file�е�90����д��buf1��
		if (FindAttribute(file, AttributeIndexAllocation, 0) == 0) // 90���Ե�һ���������A0��B0����
		{
			index = TraverseIndexEntry(AttributeIndexRoot, buffer1, dir[i]);
			continue;
		}
		else if (��һ��������ƫ��4@0x30 == 0 && 2@3C == 1){ // 90���Եڶ������������Ϊ����������A0��B0����
			PATTRIBUTE attrA0 = FindAttribute(file, AttributeIndexAllocation, 0); //���ڴ�file����A0���� 
			PUCHAR buffer2 = new UCHAR[AttributeLengthAllocated(attrA0)]; // ����A0���Դ�С���ڴ�ռ�
			/***** �����䣺��ReadAttributeʱ����Ҫ��B0���ԣ����˵���Ч�������ڵ� */
			ReadAttribute(attrA0, 0, buffer2); // ���ڴ�ռ�file�е�A0����д��buf2�У�A0�Ƿǳ�פ���ԣ����������ڵ������ȫ��д��buf2
			index = TraverseIndexEntry(AttributeIndexAllocation, buffer2, dir[i]); // ����buf2�������ڵ��������ҵ���һ��Ŀ¼��MFT��¼��
			continue;
		}
		else if (ƫ��4@0x30 == 0x10 && 2@3C == 1){ // 90���Ե��������
			ULONG indexTemp = TraverseIndexEntry(AttributeIndexRoot, buffer1, dir[i]); // �ֱ����90�ڵ��������A0ָ��������ڵ㣬��Ŀ���ļ���
			if (indexTemp == 0) 
			{
				PATTRIBUTE attrA0 = FindAttribute(file, AttributeIndexAllocation, 0);//���ڴ�file����A0����
				PUCHAR buffer2 = new UCHAR[AttributeLengthAllocated(attrA0)]; // ����A0���Դ�С���ڴ�ռ�
				ReadAttribute(attrA0, 0, buffer2);
				index = TraverseIndexEntry(AttributeIndexAllocation, buffer2, dir[i]);
				continue;
			}
			else {
				index = indexTemp;
				continue;
			}
		}
	}

	// delete **dir
	for (int i = 0; i<str_vec.size(); i++)
	{
		delete[] dir[i];
	}
	delete dir;
	delete[] cpath;
}