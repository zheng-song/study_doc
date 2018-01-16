[TOC]

在winioctl.h当中定义的一个有趣的文件系统控制操作参数是`FSCTL_GET_NTFS_RECORD`，这个参数用来从一个NTFS卷的MFT(Master File Table)中获取一个文件记录。当用这个控制代码调用`ZwFsControlFile(或者是Win32的函数DeviceIoControl)` 时,InputBuffer参数指向一个NTFS_FILE_RECORD_INOUT_BUFFER的结构体，OutputBuffer参数指向一个足够大的能够容纳一个NTFS_FILE_RECORD_OUTPUT_BUFFRE结构的和file record的缓存区。

```c++
typedef struct{
  ULONGLONG FileReferenceNumber;
}NTFS_FILE_RECORD_INPUT_BUFFER, *PNTFS_FILE_RECORD_INPUT_BUFFER;

typedef struct{
  ULONGLONG FileReferenceNumber;
  ULONG FileRecordLength;
  UCHAR FileRecordBuffer[1];
}NTFS_FILE_RECORD_OUTPUT_BUFFER, *PNTFS_FILE_RECORD_OUTPUT_BUFFER;
```

严格的讲，一个FileReferenceNumber包含一个48bit的指向MFT的index，和一个用来记录这个表的入口(entry)被复用(reused)了多少次的16bits的序列号。但是当使用`FSCTL_GET_NTFS_FILE_RECORD` 参数时,这个16bit的序列号会被忽略。因此,为了获得在30号index的文件记录, 30就应该被赋值给FileReferenceNumber. 若30号index 处的表入口(Table entry)是空的, 那么`FSCTL_GET_NTFS_FILE_RECORD` 就会获取一个附近的非空入口(nearby entry that is not empty)。为了验证想要的table entry被取得，有必要将output buffer 和input buffer中的FileReferenceNumber的低48位进行比较。



本节的剩余部分记录NTFS在磁盘上表现的数据结构，包括一个简单的实例，该实例用来说明恢复数据和被删除的文件的数据结构。磁盘数据结构(on-disk data structures)的描述被用来解释`FSCTL_GET_NTFS_FILE_RECORD` 获取的FileRecordBuffer中的返回值的内容。

```c++
// **NTFS RECORD HEADER**
typedef struct{
  ULONG Type;
  USHORT UsaOffset;
  USHORT UsaCount;
  USN Usn;
}NTFS_RECORD_HEADER, *PNTFS_RECORD_HEADER;
```

数据成员：

- Type： NTFS记录的类型，定义的值包括：FILE 、INDX、BAAD、HOLE、CHKD。
- JsaOffset： 字节(Byte)描述的偏移量,从结构的开始位置到Update Sequence Array。
- JsaCount：The number of values in the Update Sequence Array。
- Jsn： The Update Sequence Number of the NTFS record。

```c++
//****FILE RECORD_HEADER****
typedef struct{
  NTFS_RECORD_HEADER Ntfs;
  USHORT SequenceNumber;
  USHORT LinkCount;
  USHORT AttributeOffset;
  USHORT Flags;
  ULONG BytesInUse;
  ULONG BytesAllocated;
  ULONGLONG BaseFileRecord;
  USHORT NextAttributeNumber;
}FILE_RECORD_HEADER, *PFILE_RECORD_HEADER;
```

数据成员：

- Ntfs :  一个FILE类型的NTFS_RECORD_HEADER结构。

- SequenceNumber: The number of times that the MFT entry has been reused.

- LinkCount: The number of directory links to the MFT entry.

- AttributeOffset: The offset, in bytes, from the start  of the structure to the first attribute of the MFT entry.

- Flags:  A bit array of flags specifying properties of the MFT entry. The values defined include: 

  InUse 0x0001  //The MFT entry is in use

  Directoty  0x0002   // The MFT entry represents a directory.

- BytesInUse : The number of bytes used by the MFT entry.

- BytesAllocated: The number of bytes allocated for MFT entry.

- BaseFileRecord: If the MFT entry contains attributes that overflowed a base MFT entry, this member contains  the file reference number of the base entry ;otherwise, it contains zero;

- NextAttributeNumber: The number that will assigned to the next attribute added to the MFT entry.

备注： MFT中的一个条目(entry)包含一个FILE_RECORD_HEADER，跟随在一系列的attributes之后。



```c++
typedef struct{
  ATTRIBUTE_TYPE AttributeType;
  ULONG Length;
  BOOLEAN Nonresident;
  UCHAR NameLength;
  USHORT NameOffset;
  USHORT Flags;	 //0x0001 = Compressed
  USHORT AttributeNumber;
}ATTRIBUTE, *PATTRIBUTE;
```





#### RESIDENT_ATTRIBUTE

```c++
typedef struct{
  ATTRIBUTE Attribute;
  ULONG ValueLength;
  USHORT ValueOffset;
  USHORT Flags;
}RESIDENT, *PRESIDENT_ATTRIBUTE;
```



#### NONRESIDENT_ATTRIBUTE

```c++
typedef struct{
  ATTRIBUTE Attribiute;
  ULONGLONG LowVcn;
  ULONGLONG HighVcn;
  USHORT RunArrayOffset;
  UCHAR CompressionUint;
  UCHAR AlignmentOrReserved[5];
  ULONGLONG AllocatedSize;
  ULONGLONG DataSize;
  ULONGLONG InitializedSize;
  ULONGLONG CompressedSize;	// Only when compressed
}NONRESIDENT_ATTRIBUTE, *PNONRESIDENT_ATTRIBUTE;
```



#### AttributeStandardInformation

```c++
typedef struct{
  ULONGLONG CreateTime;
  ULONGLONG ChangeTime;
  ULONGLONG LastWriteTime;
  ULONGLONG LastAccessTime;
  ULONG FileAttributes;
  ULONG AlignmentOrReservedOrUnkonwn[3];
  ULONG QuotaId; //NTFS 3.0 only
  ULONG SecurityId; //NTFS 3.0 only
  ULONGLONG QuotaCharge; //NTFS 3.0 only
  USN Usn;//NTFS 3.0 only
}STANDARD_INFORMATION, *PSTANDARD_INFORMATION;
```



#### AttributeAttributeList

```c++
typedef struct{
  ATTRIBUTE_TYPE AttributeType;
  USHORT Length;
  UCHAR NameLength;
  UCHAR NameOffset;
  ULONGLONG LowVcn;
  ULONGLONG FileReferenceNumber;
  USHORT AttributeNumber;
  USHORT AlignmentOrReserved[3];
}ATTRIBUTE_LIST, *PATTRIBUTE_LIST;
```

- 备注：

  这个属性列表属性必须是nonresident并且包含了一个ATTRIBUTE_LIST结构列表。

  attribute_list属性只有在一个文件的属性在一个单MFT记录（a single MFT record）中不匹配的时候才需要. 一个单MFT条目溢出（overflowing）的原因可能包括：

  - 这个文件有很多的可变名字（即硬链接很多）
  - The attribute value is large, and the volume is badly fragmented.
  - The file has a complex security descriptor（does not affect NTFS 3.0）
  - The file has many streams

#### AttributeFileName

```c++
typedef struct{
  ULONGLONG DirectoryFileReferenceNumber;
  ULONGLONG CreationTime; // Saved when filename last changed
  ULONGLONG ChangeTime; // ditto
  ULONGLONG LastWriteTime; // ditto
  ULONGLONG LastAccessTime; // ditto
  ULONGLONG AllocatedSize; // ditto
  ULONGLONG DataSize; // ditto
  ULONG FileAttributes; // ditto
  ULONG AlignmentOrReserved;
  UCHAR NameLength;
  UCHAR NameType; // 0x01 = Long, 0x02 = Short
  WCHAR Name[1];
}FILENAME_ATTRIBUTE, *PFILENAME_ATTRIBUTE;
```

- 备注：

  文件名属性必须是resident

#### AttributeObjectId

```c++
typedef struct{
  GUID ObjectId;
  union{
    struct{
      GUID BirthVolumeId;
      GUID BirthObjectId;
      GUID DomainId;
    };
    UCHAR ExtendedInfo[48];
  };
}OBJECTID_ATTRIBUTE, *POBJECTID_ATTRIBUTE;
```

- 备注：

  The object identifier attribute is always resident.

#### AttributeSecurityDescriptor

安全描述属性作为一个标准自持的(self-relative)安全描述存储在磁盘上。在NTFS3.0 格式的卷上，这个属性通常不存在于MFT条目当中。



#### AttributeVolumeName

卷名(volume name)属性仅仅包含一个Unicode编码的字符串volume label。



#### AttributeVolumentInformation

```c++
typedef struct {
  ULONG Unknown[2];
  UCHAR MajorVersion;
  UCHAR MinorVersion;
  USHORT Flags;
} VOLUME_INFORMATION, *PVOLUME_INFORMATION; 
```



#### AttributeData

Data 属性包含所有的创建者选择的数据



#### AttributeIndexRoot

```c++
typedef struct {
  ATTRIBUTE_TYPE Type;
  ULONG CollationRule;
  ULONG BytesPerIndexBlock;
  ULONG ClustersPerIndexBlock;
  DIRECTORY_INDEX DirectoryIndex;
} INDEX_ROOT, *PINDEX_ROOT;
```

- 备注

  一个INDEX_ROOT结构后面跟随者一个DIRECTORY_ENTRY结构序列。

#### AttributeIndexAllocation

```c++
typedef struct{
  NTFS_RECORD_HEADER Ntfs;
  ULONGLONG IndexBlockVcn;
  DIRECTORY_INDEX DirectoryIndex;
} INDEX_BLOCK_HEADER, *PINDEX_BLOCK_HEADER;
```

- 备注: 

  index allocation属性是一个块索引数组(an array of index blocks).每一个块索引以一个INDEX_BLOCK_HEADER结构开始，之后是一个DIRCETORY_ENTRY结构序列。

#### DIRECROTY_INDEX

```c++
typedef struct{
  ULONG EntriesOffset;
  ULONG IndexBlockLength;
  ULONG AllocatedSize;
  ULONG Flags; //0x00 = Small directory, 0x01 = Large directoty
}DIRECTOTY_INDEX, *PDIRECTOTY_INDEX;
```





#### DIRECTOTY_ENTRY

```c++
typedef struct{
  ULONGLONG FileReferenceNumber;
  USHORT Length;
  USHORT AttributeLength;
  ULONG Flags; // 0x01 = Has trailing VCN, 0x02 = Last entry
// FILENAME_ATTRIBUTE Name;
// ULONGLONG Vcn; // VCN in IndexAllocation of earlier entries
} DIRECTORY_ENTRY, *PDIRECTORY_ENTRY;
```

- 备注：

  Until NTFS version 3.0, only filename attributes were indexed。

  如果一个DIRECTOTY_ENTRY结构的HasTrailingVcn标志被设置，那么direcory entry的最后8个字节包含索引块的VCN(the VCN of the index block).

#### AttributeBitmap

位图属性包含一个bits数组。文件"\$Mft"包含有一个bitmap属性，这个属性记录了哪个MFT表条目在使用，并且directories通常包含有一个bitmap属性用以记录哪一个索引块包含有效的条目



#### AttributeReparsePoint

```c++
typedef struct{
  ULONG ReparseTag;
  USHORT ReparseDataLength;
  USHORT Reserved;
  UCHAR ReparseData[1];
}REPAESE_POINT,*PREPARSE_POINT;
```



#### AttributeEAInformation

```c++
typedef struct {
  ULONG EaLength;
  ULONG EaQueryLength;
} EA_INFORMATION, *PEA_INFORMATION; 
```



#### AttributeEA

```c++
typedef struct {
	ULONG NextEntryOffset;
	UCHAR Flags;
	UCHAR EaNameLength;
	USHORT EaValueLength;
	CHAR EaName[1];
// UCHAR EaData[];
} EA_ATTRIBUTE, *PEA_ATTRIBUTE;
```



#### AttributePropertySet

用来支持Native Structured Storage(NSS), 这一特性在NTFS3.0的beta测试版本中被移除。



#### AttributeLoggedUtilityStream

logged utility stream属性包含创建者选择的所有的属性数据，但是对于属性的操作会被记录到NFTS的log file，就像NTFS的元数据被改变一样。 这个属性用在Encrypting File System (EFS)当中。







### Special Files

MFT中最开始的十六个条目被用来保存特殊文件，NTFS3.0只使用最开始的20个条目。

- \\$MFT(entry 0) : MFT。 数据属性包含有MFT条目，bitmap属性记录的是哪个条目正在使用。

- \\$MFTMirr(entry 1): MFT的头四个条目的镜像(backup copy)

- \\$LogFile(entry 2): The volume log file that records changes to the volume structure.

- \\$Volume(entry 3): \$Volume的数据属性代表着整个的volume。打开Win32的路径名“\\\\.\\C:”就打开了C盘（假设C：是一个NTFS格式的卷）。

- \\$AttrDef(enrty 4): \$AttrDef的数据属性包含有一个属性定义数组：

  ```c++
  typedef struct{
    WCHAR AttributeName[64];
    ULONG AttributeNumber;
    ULONG Unkonw[2];
    ULONG Flags;
    ULONGLONG MinimumSize;
    ULONGLONG MaxmumSize;
  }ATTRIBUTE_DEFINITION, *PATTRIBUTE_DEFINITION;
  ```

- \\(entry 5): volumed的根路径。

- \\$Bitmap(entry 6): The data attribute of \$Bitmap is a bitmap of the allocated clusters on the volume.

- \\$Boot(entry 7): \\Boot的第一部分同样也是volume的一部分。






// ConsoleApplication1.cpp: 定义控制台应用程序的入口点。
//

```C++
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
```









## [Master Boot Record](https://technet.microsoft.com/en-us/library/cc976786.aspx)

​	The MBR, the most important data structure on the disk, is created when the disk is partitioned.  The MBR contains a small amount of executable code called the master boot code,the disk signature, and the partition table for the disk.At the end of the MBR is a 2-byte structure called a signature word or end of sector marker, which is always set to 0x55AA. A signature word also marks the end of an extended boot record (EBR) and the boot sector.

The disk signature, a unique number at offset 0x01B8, identifies the disk to the operating system. Windows 2000 uses the disk signature as an index to store and retrieve information about the disk in the registry subkey:

`HKEY_LOCAL_MACHINE\SYSTEM\MountedDevices`

### Master Boot Code

The master boot code performs the following activities:

1. Scans the partition table for the active partition.
2. Finds the starting sector of the active partition.
3. Loads a copy of the boot sector from the active partition into memory.
4. Transfers control to the executable code in the boot sector.

If the master boot code cannot complete these functions, the system displays one of the following error messages:

- Invalid partition table
- Error loading operating system
- Missing operating system

![DiskDataStructure](http://img.blog.csdn.net/20180116194000274?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)



![这里写图片描述](http://img.blog.csdn.net/20180116194158099?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)





### Partition Table

​	The partition table, a 64-byte data structure used to identify the type and location of partitions on a hard disk, conforms to a standard layout independent of the operating system. Each partition table entry is 16 bytes long, with a maximum of four entries. Each entry starts at a 
predetermined offset from the beginning of the sector, as follows:

- Partition 1\ 0x01BE(446)
- Partition 2\ 0x01CE(462)
- Partition 3\ 0x01DE(478)
- Partition 4\ 0x01EE(494)



## [Recovering Data with NTFS](https://technet.microsoft.com/en-us/library/cc976815.aspx)          









## NFTS 系统中查找文件的流程

1. 从BootSector中读出文件所在的磁盘的MFT起始簇

   `hVolume = CreateFile(TEXT("\\\\.\\C:"), GENERIC_READ,FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);`

   `ReadFile(hVolume, &bootb, sizeof bootb, &n, 0)`

   ![NTFSBootSector.png](http://upload-images.jianshu.io/upload_images/6128001-4e9ea324df41ba3c.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

   bootb为BOOT_BLOCK类型的结构体。该结构体表示了NTFS Boot Sector的结构。

   ```C++
   typedef struct { //512B   
   	UCHAR Jump[3];		//Jump Instruction 跳过3个字节   
   	UCHAR Format[8]; 	//OEM ID ‘N’'T' 'F' 'S' 0x20(空格) 0x20 0x20 0x20   
   	USHORT BytesPerSector;//Byte/Sector 每扇区有多少字节 一般为512B 0x200  
   	UCHAR SectorsPerCluster;//Sect/clust 每簇有多少个扇区   
   	USHORT BootSectors;//res   
   	UCHAR Mbz1;//保留0  0x00 
   	USHORT Mbz2;//保留0 0x0000
   	USHORT Reserved1;// unused  保留0   
   	UCHAR MediaType;//  Media descriptor, legacy from DOS, 0xF8 indicates fixed disk, 0xF0 a HD 3.5inch floppy
   	USHORT Mbz3;//0x0000    总是为0   
   	USHORT SectorsPerTrack;// Sect/track 	每道扇区数，一般为0x3f   
   	USHORT NumberOfHeads;// Number heads  	磁头数   
   	ULONG PartitionOffset;// Hidden Sectors	该分区的便宜（即该分区前的隐含扇区数 一般为磁道扇区数0x3f 63）   
   	ULONG Reserved2[2];	// unused
    	ULONGLONG TotalSectors;// Total Sectors	该分区总扇区数   
   	ULONGLONG MftStartLcn;// Logical Cluster of $MFT  MFT表的起始簇号LCN   
   	ULONGLONG Mft2StartLcn;// Logical Cluster of $MFTMirr  MFT备份表的起始簇号LCN   
   	ULONG ClustersPerFileRecord;//Clust/File Record segment 每个MFT记录包含几个簇  记录的字节不一定为：ClustersPerFileRecord*SectorsPerCluster*BytesPerSector  
   	ULONG ClustersPerIndexBlock;// Clusters/index Block  每个索引块的簇数   
   	LARGE_INTEGER VolumeSerialNumber;//Volume Serial Number   卷序列号   
   	UCHAR Code[0x1AE]; // 包含Checksum和BootCode之间的部分
   	USHORT BootSignature; //最后的55和AA部分
   } BOOT_BLOCK, *PBOOT_BLOCK;
   ```

   ![loadMFT](http://img.blog.csdn.net/20180112155603632?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

   ​

   ​

   ​

   ## ntfs.h 头文件

ntfs.h头文件中的内容包含的是文件系统当中的一些结构体的定义。

### [Boot Sector](https://technet.microsoft.com/en-us/library/cc976796.aspx)

​	The boot sector, located at sector 1 of each volume, is a critical disk structure for starting your computer. It contains executable code and data required by the code, including information that the file system uses to access the volume. The boot sector is created when you format a volume. At the end of the boot sector is a two-byte structure called a signature word or end of sector marker, which is always set to 0x55AA. On computers running Windows 2000, the boot sector on the active partition loads into memory and starts Ntldr, which loads the operating system.

![NTFSBootSector.png](http://upload-images.jianshu.io/upload_images/6128001-4e9ea324df41ba3c.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

- res means reserved, note that the terms reserved,unused and 0x00 are specified by Microsoft, the difference between reserved and unused is not specified. However it should be noted that the blocks specified as all zeros have defined meaning within FAT boot sectors

![BootSector](http://img.blog.csdn.net/20180112174614019?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)



### MFT Entry Attribute

![MFTEntryAttribut](http://img.blog.csdn.net/20180112190513952?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

```C++
typedef enum {   
	AttributeStandardInformation = 0x10,  
	AttributeAttributeList = 0x20, 
	AttributeFileName = 0x30, 
  	AttributeObjectId = 0x40,   
	AttributeSecurityDescriptor = 0x50, 
	AttributeVolumeName = 0x60,
  	AttributeVolumeInformation = 0x70,
	AttributeData = 0x80,
	AttributeIndexRoot = 0x90,
	AttributeIndexAllocation = 0xA0,
	AttributeBitmap = 0xB0,
	AttributeReparsePoint = 0xC0,
	AttributeEAInformation = 0xD0,
	AttributeEA = 0xE0,
	AttributePropertySet = 0xF0,
	AttributeLoggedUtilityStream = 0x100,
	AttributeEnd = 0xFFFFFFFF
} ATTRIBUTE_TYPE, *PATTRIBUTE_TYPE;
```

