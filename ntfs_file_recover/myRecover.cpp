#include "stdafx.h"
#include <windows.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>

using namespace std;
#include "myNtfs.h"


ULONG BytesPerFileRecord; // 每个MFT entry的大小，通常为1K
HANDLE hVolume;           // 文件句柄
BOOT_BLOCK bootb;        
PFILE_RECORD_HEADER MFT ; // 存储MFT的内容

template <class T1, class T2> inline T1* Padd(T1* p, T2 n)
{
	return (T1*)((char *)p + n);
}


void LoadMFT(const &bootP);
void HandleString(string &str);



int wmain(int argc, WCHAR **argv)
{
	WCHAR drive[] = L"\\\\.\\C:";
	ULONG n;
	
	// No argument supplied
	if (argc < 2)
	{
		wprintf(L"Usage:\n");
		wprintf(L"Find deleted files: %s <primary_partition>\n", argv[0]);
		wprintf(L"Read the file records: %s <primary_partition> <index> <file_name>\n", argv[0]);
		// Just exit
		exit(1);
	}
	string s = argv[1];

	drive[4] = *argv[1];

	hVolume = CreateFile(
		drive,
		GENERIC_READ,//中文
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

	LoadMFT(bootb);
}

/* 
 * 功能: 将扇区位置为sector,长度为count个扇区的内容读到buffer中
 */
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

// ????这段代码的功能是完成更新序列号的调整,但是没看出来这段代码到底完成了什么,有什么用
VOID FixupUpdateSequenceArray(PFILE_RECORD_HEADER file)
{
	ULONG i = 0;
	//定位到usa的偏移位置.
	PUSHORT usa = PUSHORT(Padd(file, file->Ntfs.UsaOffset));
	PUSHORT sector = PUSHORT(file);
	wprintf(L"In FixupUpdateSequenceArray()...\n");
	
	for (i = 1; i < file->Ntfs.UsaCount; i++)
	{
		sector[255] = usa[i];
		sector += 256;
	}
}


void LoadMFT(const BOOT_BLOCK &bootb)
{
	wprintf(L"In LoadMFT() - Loading MFT...\n");

	BytesPerFileRecord =  bootb.ClustersPerFileRecord < 0x80
		? bootb.ClustersPerFileRecord* bootb.SectorsPerCluster
		* bootb.BytesPerSector : 1 << (0x100 - bootb.ClustersPerFileRecord);

	wprintf(L"\nBytes Per MFT Record = %u\n\n", BytesPerFileRecord);
	wprintf(L"======THESE INFO ARE NOT ACCURATE FOR DISPLAY LOL!=====\n");
	wprintf(L"bootb.BytesPerSector = %u\n", bootb.BytesPerSector);
	wprintf(L"bootb.ClustersPerFileRecord = %u\n", bootb.ClustersPerFileRecord);
	wprintf(L"bootb.ClustersPerIndexBlock = %u\n", bootb.ClustersPerIndexBlock);
	wprintf(L"bootb.Format = %u\n", bootb.Format);
	wprintf(L"bootb.MediaType = 0X%X\n", bootb.MediaType);
	wprintf(L"bootb.Mft2StartLcn = 0X%.8X\n", bootb.Mft2StartLcn);
	wprintf(L"bootb.MftStartLcn = 0X%.8X\n", bootb.MftStartLcn);
	wprintf(L"bootb.NumberOfHeads = %u\n", bootb.NumberOfHeads);
	wprintf(L"bootb.PartitionOffset = %lu\n", bootb.PartitionOffset);
	wprintf(L"bootb.SectorsPerCluster = %u\n", bootb.SectorsPerCluster);
	wprintf(L"bootb.SectorsPerTrack = %u\n", bootb.SectorsPerTrack);
	wprintf(L"bootb.TotalSectors = %lu\n", bootb.TotalSectors);
	wprintf(L"bootb.VolumeSerialNumber = 0X%.8X%.8X\n\n", bootb.VolumeSerialNumber.HighPart, bootb.VolumeSerialNumber.HighPart);

	// 为MFT分配内存空间
	MFT = PFILE_RECORD_HEADER(new UCHAR[BytesPerFileRecord]);

	//将MFT的起始簇所在的扇区的内容读到MFT中,即MFT中存储的内容为$MFT entry的内容
	ReadSector((bootb.MftStartLcn)*(bootb.SectorsPerCluster), (BytesPerFileRecord) / (bootb.BytesPerSector), MFT);
	
	FixupUpdateSequenceArray(MFT);

	if (argc == 4)
		DumpData(wcstoul(argv[2], 0, 0), argv[3]);
}


vector<string> split(const string &s, const string &seperator){
    vector<string> result;
    typedef string::size_type string_size;
    string_size i = 0;

    while(i != s.size()){
        //找到字符串中首个不等于分隔符的字母；
        int flag = 0;
        while(i != s.size() && flag == 0){
            flag = 1;
            for(string_size x = 0; x < seperator.size(); ++x){\
                if(s[i] == seperator[x]){
                    ++i;
                    flag = 0;
                    break;
                }
            }
        }

        //找到又一个分隔符，将两个分隔符之间的字符串取出；
        flag = 0;
        string_size j = i;
        while(j != s.size() && flag == 0){
            for(string_size x = 0; x < seperator.size(); ++x)
                if(s[j] == seperator[x]){
                    flag = 1;
                    break;
                }
            if(flag == 0) ++j;
        }
        if(i != j){
            result.push_back(s.substr(i, j-i));
            i = j;
        }
    }
    return result;
}

void HandleString(string &s)
{
    vector<string> v = split(s, ":/"); //可按多个字符来分隔;
    for(vector<string>::size_type i = 0; i != v.size(); ++i)
        cout << v[i] << "\t";
    cout << endl;
}