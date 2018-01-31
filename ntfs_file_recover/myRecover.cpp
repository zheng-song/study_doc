#include "stdafx.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "myNtfs.h"

template <class T1, class T2> inline T1* Padd(T1* p, T2 n)
{
	return (T1*)((char *)p + n);
}

void LoadMFT(const &bootP);

int wmain(int argc, WCHAR **argv)
{
	WCHAR drive[] = L"\\\\.\\C:";
	ULONG n;

	ULONG BytesPerFileRecord; // 每个MFT entry的大小，通常为1K
	HANDLE hVolume;           // 文件句柄
	BOOT_BLOCK bootb;        
	PFILE_RECORD_HEADER MFT ; // 存储MFT的内容
	
	// No argument supplied
	if (argc < 2)
	{
		wprintf(L"Usage:\n");
		wprintf(L"Find deleted files: %s <primary_partition>\n", argv[0]);
		wprintf(L"Read the file records: %s <primary_partition> <index> <file_name>\n", argv[0]);
		// Just exit
		exit(1);
	}

	drive[4] = *argv[1];

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

	LoadMFT(bootb);
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
}
