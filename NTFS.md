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




### MFT Entry Attribute

​	在NTFS中，所有与数据相关的信息都称为“属性”，NTFS与其他文件系统最大的不同之处在于，大多数文件系统是对文件的内容进行读写，而NTFS则是对包含文件内容的属性进行读写。

在数据结构中，属性又可以分为长驻属性和非常驻属性：

1. **常驻属性：** 有的属性其属性内容很小，它的MFT项可以容纳下它的全部内容，为了节约空间，系统会直接将其存放在MFT项中，而不再为其另外分配簇空间，这样的属性称为常驻属性
2. **非常驻属性：** 非常驻属性是指那些内容较大，无法完全存放在其MFT项中的属性。如文件的数据属性，通常内容很大，需要在MFT之外另为其分配足够的簇空间进行存储，这样的属性就是非长驻属性。

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





## 对于NTFS 文件系统的研究

​	NTFS分区的最开始的16个扇区是分区引导扇区，用于保存分区引导代码，接下来是主文件表(MFT),如果MFT所在的磁盘扇区出现损坏，NTFS文件系统会将MFT转移到到硬盘的其他扇区，这样就保证了NTFS文件系统和Windows操作系统的正常运行。比之先前的FAT16和FAT32文件系统的FAT(文件分配表)，FAT只能固定在分区引导扇区的后面，一旦该扇区，整个文件系统就会瘫痪，NTFS文件系统显然要先进的多了。不过这种移动MFT的做法却也并非十全十美，如果分区引导代码中指向MFT的部分出现错误，那么NTFS文件系统便会不知道到哪里寻找MFT ，从而会报告“磁盘没有格式化”这样的错误信息。为了避免这样的问题发生，分区引导代码中会包含一段校验程序，专门负责侦错。

​	一个NTFS文件系统大致上可以分为

- 引导区: 引导区部分包括DBR(DOS BOOT RECORD)和引导代码，一般系统为其分配16个扇区，未完全使用。
- MFT区:   文件系统中出现一个“MFT”区，这个“MFT区”是一个连续的簇空间，除非其他空间已全部被分配使用，否则不会在此空间中存储用户文件或目录。在WINXP下创建的NTFS，其MFT通常距离引导扇区较远，但在WIN2000下创建的NTFS，其MFT通常起始于4号簇位置。
- MFT备份区：由于MFT备份的重要性，在文件系统的中部为其保存了一个备份，不过这个备份很小，只是MFT前几个项的备份。
- 数据区：用户数据
- DBR备份扇区： 在卷的最后一个扇区，保存了一份DBR扇区的备份。这个扇区包含在分区表描述的该分区大小中，但却不在DBR描述的文件系统大小范围之内。DBR描述文件系统大小时，总是比分区表描述的扇区数小1个扇区。

因为NTFS将所有的数据都视为文件，理论上除了引导扇区必须位于第一个扇区以外，NTFS卷可以在任意的位置存放任意的文件，但是通常都会遵循一定的布局。

## NTFS的特点

- NTFS与FAT文件系统一样，也是用**簇**作为数据的存取的最小单位。但是因为他将所有的数据，包括文件系统管理数据也做为文件进行管理，所以NTFS文件系统中的所有扇区都被分配以簇号，并以0开始对所有的簇进行编号，文件系统的0号扇区为0号簇的起始位置。
- 它的可升级性基于使用常规结构对特殊数据结构进行管理。在NTFS文件系统将所有的数据都视为文件，通常在其他文件系统中被隐藏的管理数据在NTFS中也被存储在文件中，文件系统管理数据可以像普通文件一样被存放在文件系统内任何位置。

## NTFS 元文件

​	NTFS文件系统被创建时，会同时建立一些重要的系统信息。这些系统信息也全是以文件的形式存在，被称为元文件。元文件的文件名都以“ $”符号开头，表示其为隐藏的系统文件，用户不可直接访问。

NTFS的元文件总共有17个，其具体的含义如下：

- **$MFT:** 它其实是整个主文件表，也就是将整个MFT看做一个文件。
- **$MFTMirr:** MFT前几个MFT项的备份，NTFS也将其作为一个文件看待。
- **$LogFile:** 日志文件。
- **$Volume:** 卷文件，包含有卷标和其他的版本信息。
- **$AttDef:** 属性定义列表，定义每种属性的名字和类型。
- **$Root:** 根目录文件
- **$Bitmap:** 位图文件，它的数据属性的每一个bit对应文件系统中的一个簇，用以描述簇的分配情况。
- **$Boot：** 引导文件，DBR扇区就是引导文件的第一个扇区。
- **$BadClus：** 坏簇记录文件。
- **$Quota：** 早期的NT系统中记录磁盘配额信息。
- **$Secure：** 安全文件。
- **$UpCase:** 大小写字符转换表文件。
- **$Extend metadata directory:** 扩展元数据目录。
- **$Extend\$Reparse：** 重解析点文件.
- **$Extend\$UsnJrnl：** 变更日志文件。
- **$Extend\$Quota:** 配额管理文件。
- **$Extend\$ObjId：**对象ID文件。



## DBR (DOS BOOT RECORD)的作用

​	NTFS的引导扇区也位于文件系统的0号扇区，这是它与FAT文件系统在布局上的唯一相同之处。

​	DBR（DOS BOOT RECORD，DOS引导记录），位于柱面0，磁头1，扇区1，即逻辑扇区0。DBR分为两部分：DOS引导程序和BPB（BIOS参数块）。其中DOS引导程序完成DOS系统文件（IO.SYS，MSDOS.SYS）的定位与装载，而BPB用来描述本DOS分区的磁盘信息，**PB位于DBR偏移0BH处，共13字节。** 它包含逻辑格式化时使用的参数，可供DOS计算磁盘上的文件分配表，目录区和数据区的起始地址，BPB之后三个字提供物理格式化（低格）时采用的一些参数。引导程序或设备驱动程序根据这些信息将磁盘逻辑地址（DOS扇区号）转换成物理地址（绝对扇区号）。

### [Boot Sector](https://technet.microsoft.com/en-us/library/cc976796.aspx)

​	The boot sector, located at sector 1 of each volume, is a critical disk structure for starting your computer. It contains executable code and data required by the code, including information that the file system uses to access the volume. The boot sector is created when you format a volume. At the end of the boot sector is a two-byte structure called a signature word or end of sector marker, which is always set to 0x55AA. On computers running Windows 2000, the boot sector on the active partition loads into memory and starts Ntldr, which loads the operating system.

![NTFSBootSector.png](http://upload-images.jianshu.io/upload_images/6128001-4e9ea324df41ba3c.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

- res means reserved, note that the terms reserved,unused and 0x00 are specified by Microsoft, the difference between reserved and unused is not specified. However it should be noted that the blocks specified as all zeros have defined meaning within FAT boot sectors

```C++
#pragma pack(push,1)   
typedef struct { //512B   
	UCHAR Jump[3];			//跳过3个字节   
	UCHAR Format[8]; 		//‘N’'T' 'F' 'S' 0x20 0x20 0x20 0x20   
	USHORT BytesPerSector;	//每扇区有多少字节 一般为512B 0x200   
	UCHAR SectorsPerCluster;//每簇有多少个扇区
  	USHORT res;				//保留扇区数   
	UCHAR Mbz1;				//保留0   
	USHORT Mbz2;			//保留0   
	USHORT Reserved1;		//保留0   
	UCHAR MediaType;		//介质描述符，硬盘为0xf8   
	USHORT Mbz3;			//总为0   
	USHORT SectorsPerTrack;	//每磁道的扇区数，一般为0x3f(不检查此项)   
	USHORT NumberOfHeads;	//每柱面磁头数(不检查此项)   
	ULONG PartitionOffset;	//该分区的偏移（即该分区前的隐含扇区数 一般为磁道扇区数0x3f 63，不检查此项）   
	ULONG Reserved2[2];		//总是80008000(不检查此项)
	ULONGLONG TotalSectors;	//该分区总共的扇区数  
	ULONGLONG MftStartLcn;	//MFT表的起始簇号LCN
	ULONGLONG Mft2StartLcn;	//MFT备份表的起始簇号LCN
	ULONG ClustersPerFileRecord;//每个MFT记录包含几个簇  
	//记录的字节不一定为：ClustersPerFileRecord*SectorsPerCluster*BytesPerSector  
	ULONG ClustersPerIndexBlock;//每个索引块的簇数
	LARGE_INTEGER VolumeSerialNumber;//卷序列号   
	UCHAR Code[0x1AE]; 		//包含校验码和引导代码。
	USHORT BootSignature;	//签名55AA标记
} BOOT_BLOCK, *PBOOT_BLOCK;
#pragma pack(pop) 
```

​	以上引导扇区最为关键的字节数是0B-0C（每扇区字节数） 0B-0C（每扇区字节数） 0D（每簇扇区数）28-2F（文件系统扇区总和） 30-37（MFT起始簇号）38-3F（MFT备份的起始簇号） 40（每MFT项大小）44（每个索引的簇数），但数据发生不可预料的损坏时，可以根据以上信息重建分区表，定位数据区，恢复MFT，重建DBR，这些关键字节码的用处不言而喻。

```C++
//计算每个MFT表项所占用的Byte数	
BytesPerFileRecord = bootb.ClustersPerFileRecord < 0x80
		? bootb.ClustersPerFileRecord*bootb.SectorsPerCluster*bootb.BytesPerSector
		: 1 << (0x100 - bootb.ClustersPerFileRecord);
```



## MFT(Master File Record)

​	格式化成NTFS文件系统时，就是在其中建立了一个主文件表MFT，其中包含16个元文件的文件记录。为了尽可能减少$MFT文件产生碎片的可能性，系统预先为其预先为其预留整个文件系统大约12.5%的空间。只有在用户数据区的空间用尽时，才会临时让出MFT区的部分空间存储数据，但一旦数据区有了足够的空间，就会立即收回原来让出的MFT空间。MFT是NTFS文件系统的核心，MFT由一个个的MFT项(也称为文件记录)组成，其中用各种属性记录着该文件或目录的各种信息。每个MFT项实际的大小在引导扇区中进行说明，Microsoft的所有版本都使用1024字节的大小。前部为一个包含几十个字节的具有固定的大小和结构的MFT头，剩余的字节为属性列表，用于存放各种属性。**每一个文件和目录的信息都包含在MFT当中**，每一个目录和文件在表中至少有一个MFT项，除了引导扇区以外，访问其他的任何一个前都需要先访问MFT，在MFT中找到该文件的MFT项，根据MFT项中记录的信息找到文件内容并对其进行访问。

​	文件记录是$MFT文件的基本组成部分，卷中的所有文件都由至少一个文件记录来描述，文件通过主文件表MFT来确定其在磁盘上的存储位置、大小、属性等信息, 对于使用多个文件记录的文件，其第一个文件记录叫基本文件记录，其余的叫做扩展文件记录。MFT的每条记录都包含一个头部和一个或多个属性（按属性ID升序），并以四个字节的0xFFFFFFFF结束。

### MFT的基本特点

- MFT的第一个区域是签名，所有的MFT项都有相同的签名（FILE）。如果在项中发现错误，可能将其改写成“BAAD”的字样。
- MFT项还有一个标志域用以说明该项是一个文件项还是目录项，以及它的分配状态。MFT的分配状态也在一个$BITMAP属性文件中进行描述。
- 每个MFT项占用两个扇区，每个扇区的结束两个字节都有一个修正值，这个修正值与MFT项的更新序列号相同，如果发现不同，会认为该MFT项存在错误。
- 如果一个文件的属性较多，使用一个MFT项无法容纳下全部的属性，可以使用多个MFT项，第一个项被称为基本文件记录或基本MFT项。

### MFT头部

![MFT](http://img.blog.csdn.net/20180117194726094?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

```
/* NTFS RECORD HEADER */
typedef struct {
	ULONG Type;              // NTFS记录的类型，FILE(46494C45)/BAAD/INDX/CHKD/HOLE
	USHORT UsaOffset;        // Update Sequence Array：更新序列号偏移(相对文件头部)
	USHORT UsaCount;         // 更新序列号的数组个数+1(N)，通常为3
	USN Usn;       // 日志序列号(LSN)该数值在每一次被修改的时候都会被改动。
} NTFS_RECORD_HEADER, *PNTFS_RECORD_HEADER;  // 16 bytes

/* FILE RECORD_HEADER */
typedef struct {
	NTFS_RECORD_HEADER Ntfs;       // when Type = FILE
	USHORT SequenceNumber;         // 重复使用 更新序列号(删除一次加1) 
	USHORT LinkCount;              // 硬链接数
	USHORT AttributesOffset;       // 第一个属性数据在此 MFT entry 中的偏移
	USHORT Flags;                  // 0x0000=deleted file; 0x0001=file, 0x0002=deleted dir; 0x0003= dir;
	ULONG BytesInUse;              // MFT项的逻辑长度(已被该 MFT entry 使用的字节数,即已使用的空间)
	ULONG BytesAllocated;          // MFT的物理长度(分配给该 MFT entry 的字节数，即总共的空间)
	
 /*当前文件记录的基本文件记录的索引，如果当前文件记录是基本文件记录则该值为0，否则指向基本文件记录的记录索引。注意该值的低6字节是MFT记录号，高2字节是该MFT记录的序列号。*/	
	ULONGLONG BaseFileRecord;

/* 下一个属性的ID，下一次将会被添加到文件记录的属性的ID，每次往文件记录中添加属性时该值都会增加，每次文件记录被重新使用时该值都会被清零，第一个值肯定是0*/
	USHORT NextAttributeNumber;    
	UCHAR border[2];			   // 边界
	ULONG MFTRecordNo;			   // 该MFT的记录编号(起始编号0)
/*更新序列号数组,该值记录的是文件记录被修改的次数，每次修改时该值为+1，(包括文件被删除操作)该值不能够为0。*/	
	ULONGLONG updateSeqArr;
	ULONGLONG Reserved;			   // 属性和修正值
} FILE_RECORD_HEADER, *PFILE_RECORD_HEADER;  // 42 bytes
```

- 更新序列号: 更新序列号是Microsoft公司为了确保记录数据的可靠性而在NTFS卷中提出的一项技术，在NTFS卷中，所有的记录类型数据(FR、IR)占用的空间都是按扇区尺寸(512字节)对齐。保护记录数据时，在每512字节的最末2个字节都会写入一个校验值以确保记录中的所有数据都被正确的写入磁盘中，而校验值所在位置的数据被拷贝到记录头之后被称作USA(Update Sequence Array)的数据块中，系统将记录数据从磁盘读入内存时将检查每个校验值是否与记录头的中序列号是否相同，如果相同则用USA中相应位置的数据恢复校验值位置的数据，反之则表明该记录被没有正确地修改。

在每次写记录数据时序列号都会加1，当序列号为0时则再加1。

|      |               |               |      |                |
| ---- | ------------- | ------------- | ---- | -------------- |
| 校验码  | 第1个512字节扇区末原值 | 第2个512字节扇区末原值 | 。。。。 | 最后一个512字节扇区末原值 |

- 虚拟簇号: 非驻留数据中的每个簇都有一个特定的序号，这个序号就叫做虚拟簇号，虚拟簇号0指向数据流的第一个簇。


- 逻辑簇号：卷中的每一个簇都有一个特定的序号，这个序号就叫做逻辑簇号，逻辑簇号0指向卷中的第一个簇(引导扇区)。
- 数据流描述: 存放在间隔的簇中的属性数据称为**流**。每一个流都由起始簇号和尺寸来描述。流的起始簇号是相对于前一个流的偏移，该值是一个有符号数。流描述的格式如下：

| 名字      | 偏移   | 尺寸   | 说明                                       |
| ------- | ---- | ---- | ---------------------------------------- |
| SD_Desc | 0    | 1    | 流描述说明，高4位(M)描述流描述偏移的字节数，低4位(N)描述流描述尺寸的字节数 |
| SD_Size | 1    | N    | 当前流的尺寸                                   |
| SD_Off  | N+1  | M    | 当前流相对于上一个流的偏移，如果该值的最高位为1则表示该值是一个负数       |

​	一个流描述之后紧随着下一个流描述，如果下一个描述的SD_Desc为0则表示当前描述是最后一个。一般情况下压缩文件和稀疏文件数据都以流的形式描述。



## 文件属性记录

​	NTFS文件系统当中的文件属性可以分成两种：常驻属性和非常主属性，常驻属性直接保存在MFT当中，像文件名和相关时间信息(例如文件的创建时间、修改时间)永远属于常驻属性，非常驻属性则保存在MFT之外，但是会使用一种复杂的索引方式来进行指示。如果文件或者是文件夹小于1500字节(其实我们的电脑中有相当多的这样的大小的文件或者是文件夹)，那么它们的所有的属性，包括内容都会常驻在MFT中，而MFT是windows一启动就会载入到内存当中的，这样当你查看这些文件或者是文件夹的时候，其实他们的内容早已在缓存当中，这样就大大提高到了文件和文件夹的访问速度。

> **为什么FAT的效率不如NTFS高**、
>
> ​	FAT的文件分配表只列出了每一个文件的名称以及起始簇，并没有说明这个文件是否存在，而需要通过其所在的文件夹的记录来判断，而文件夹入口又包含在文件分配表的索引当中。因此访问文件的时候，首先要读取文件分配表来确定文件是否存在，然后再次读取文件分配表找到文件的首簇，接着以链式的检索找到文件的所有存放簇，最终确定后才可以访问。

​	文件中的每个记录都是由属性组成。每个属性由相同的格式构成，首先是一个标准属性记录头，然后存放属性的专用数据。下面列出$AttrDef中定义的可用到的属性。

| 类型    | 操作系统 | 描述                     |
| ----- | ---- | ---------------------- |
| 0x10  |      | STANDARD_INFORMATION   |
| 0x20  |      | ATTRIBUTE_LIST         |
| 0x30  |      | FILE_NAME              |
| 0x40  | NT   | VOLUME_VERSION         |
| 0x40  | 2K   | OBJECT_ID              |
| 0x50  |      | SECURITY_DESCRIPTOR    |
| 0x60  |      | VOLUME_NAME            |
| 0x70  |      | VOLUME_INFORMATION     |
| 0x80  |      | DATA                   |
| 0x90  |      | INDEX_ROOT             |
| 0xA0  |      | INDEX_ALLOCATION       |
| 0xB0  |      | BITMAP                 |
| 0xC0  | NT   | SYMBOL_LINK            |
| 0xC0  | 2K   | REPARSE_POINT          |
| 0xD0  |      | EA_INFORMATION         |
| 0xE0  |      | EA                     |
| 0xF0  | NT   | PROPERTY_SET           |
| 0x100 | 2K   | LOGGED_UNTILITY_STREAM |

##### MFT布局

![MFT布局](http://img.blog.csdn.net/20180117201032032?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)



### NTFS 中建立文件的过程

​	假设我们要加里一个文件“\子目录1\file.txt”,假设子目录1已经存在于根目录下。要建立的文件的大小为7000个字节，每一个簇的大小为4096个字节。

1. 读取文件系统的第一个扇区的引导扇区，获取簇的大小， MFT的起始位置以及每一个MFT项的大小。
2. 读取第一个MFT项，即\$MFT文件的MFT项，由他的\$DATA属性获取其他的MFT的位置。
3. 首先为准备新建的文件建立MFT项----访问\$MFT文件的\$BITMAP属性，找到一个未分配的项，假设一个空闲项为400号，把它分配给新文件，并将$BITMAP中的相应的位置设置为1。
4. 初始化MFT项----跳转到400号MFT项的位置，将其中的内容删除。建立标准的信息属性(\$STANDARD_INFORMATION)和文件名属性(\$FILE_NAME),时间设置为当前时间，在MFT项头当中设置使用中标志
5. 下面需要从使用6号MFT项的\$BITMAP文件中为文件的\$DATA属性寻找并分配两个簇，因为文件需要两个簇的空间。使用最佳分配策略找到两个连续的空闲簇722和723号簇，并将其相应的bit位设置为1，将文件的内容写入到簇中，更新\$DATA属性中的簇地址。 修改了MFT项，所以更新文件的最后修改时间。
6. 下一步为其添加文件名项。访问5号MFT项的根目录，定位`子目录1`， 读取索引根属性(\$INDEX_ROOT)和索引分配属性(\$INDEX_ALLOCATION),在倒置树中分类寻找，找到`子目录1`的索引项，它的MFT项的地址为200,更新目录的最后访问时间。 
7. 跳转到200号的MFT项，访问它的索引根属性(\$INDEX_ROOT)，寻找将为file.txt分配的空间。为其建立一个新的索引。重新对倒置树进行分类，新索引项的文件参考号地址是400号MFT项。设置相应的时间值和标志。更新目录的最后写入、修改、访问时间。
8. 在前面的每一步当中，在文件系统日志当中建立项并将改变记入\\$Extend\\\$UsrJrnl。如果设置了配额管理，新文件的大小将会记录到用户的配额当中(\\\$Extend\\\$Quota)。

FAT文件系统在读文件时，操作系统从目录区中读取文件的相关信息(包括文件名、后缀名、文件大小、修改日期和文件在数据区保存的第一个簇的簇号)，我们这里假设第一个簇号是0023.那么操作系统将会从0023簇读取相应的数据，然后再找到FAT的0023单元。如果内容是文件的结束标志(FF)，那么表示文件结束，否则内容保存下一个簇的簇号，这样重复下去直到遇到文件结束标志。

> **簇:** 由于物理磁盘中扇区是磁盘的最小物理存储单元、在硬盘中存在的量很大并且每一个扇区都应该有编号，所以操作系统无法对数目众多的扇区进行寻址。因此操作系统将相邻的扇区组合在一起，组成簇这一单位用以高效的利用资源。文件系统是操作系统与硬盘驱动器之间的接口，当系统请求从硬盘中读取一个资源时，会请求相应的文件系统打开文件。**簇包含的扇区数是由文件系统格式与分配单元大小而定的，一般每一个簇可以包括2、4、8、16、32或64个扇区。**
>
> **扇区**: 磁盘的每一面被分为很多条磁道，即表面上的一些同心圆，越接近中心，圆就越小。而每一个磁道又按512个字节为单位划分为等分，叫做扇区，在一些硬盘的参数列表上你可以看到描述每个磁道的扇区数的参数，它通常用一个范围标识，例如373～746，这表示，最外圈的磁道有746个扇区，而最里面的磁道有373个扇区。





## NTFS中删除文件的过程

​	下面是删除"\子目录1\file.txt"文件的过程。

1. 读取文件系统的第一个扇区的引导扇区，获取簇的大小，MFT的起始位置、每一个MFT项的大小等等。
2. 读取\$MFT文件的第一项，通过它的\$DATA属性得到其他的MFT位置。
3. 访问5号MFT项，即根目录通过索引根属性(\$INDEX_ROOT)和索引分配属性(\$INDEX_ALLOCATION)找到`子目录1`项，它的MFT项为200，更新目录的最后访问时间。
4. 访问200号MFT项的索引根属性(\$INDEX_ROOT)并寻找file.txt的条目，找到它的MFT项为400号。
5. 从索引中移除文件的项，移动节点中的项覆盖了原来的项，更新目录的最后写入时间、最后修改时间、最后访问时间。
6. 通过取消使用中标志取消400号MFT的分配。访问\$Bitmap文件的\$DATA属性，将该项的相应位置设置为0。
7. 访问400号MFT项的非常驻属性，从\$DATA属性中得到数据内容所在的簇号，并将\$Bitmap中的文件相应簇的bit设置为0,即取消对722、723号簇的分配。
8. 前面的每一步都将在文件系统日志\$Logfile中产生项并将改变记录到\\\$Extend\\\$UsrJrnl。如果设置了磁盘配额管理，将在\\\$Extend\\\$Quota中，把回收的内容从用户已经使用的磁盘空间中减去。




## NTFS中文件恢复

> **原理：** 首先了解一下磁存储技术的原理，这有助于我们了解数据恢复工作。磁存储技术的工作原理是通过改变磁粒子的极性来在磁性介质上记录数据。在读取数据时，磁头将存储介质上的磁粒子极性转换成相应的电脉冲信号，并转换成计算机可以识别的数据形式。进行写操作的原理也是如此。要使用硬盘等介质上的数据文件，通常需要依靠操作系统所提供的文件系统功能，文件系统维护着存储介质上所有文件的索引。因为效率等诸多方面的考虑，在我们利用操作系统提供的指令删除数据文件的时候，磁介质上的磁粒子极性并不会被清除。操作系统只是对文件系统的索引部分进行了修改，将删除文件的相应段落标识进行了删除标记。同样的，目前主流操作系统对存储介质进行格式化操作时，也不会抹除介质上的实际数据信号。正是操作系统在处理存储时的这种设定，为我们进行数据恢复提供了可能。值得注意的是，这种恢复通常只能在数据文件删除之后相应存储位置没有写入新数据的情况下进行。因为一旦新的数据写入，磁粒子极性将无可挽回的被改变从而使得旧有的数据真正意义上被清除。删除文件，其实是修改文件头的前2个代码。这种修改映射在文件分配表中，就为文件作了删除标记，但文件的内容仍保存在原来的簇，如果不被后来保存的数据覆盖，它就不会从磁盘上抹掉。文件被删除后，既然其数据仍在磁盘上，文件分配表中也有它的信息，这个文件就有恢复的机会，只要找出文件头，并恢复前2个代码，在文件分配表中重新映射一下，这个文件就被恢复了。但是，文件被删除后，如果它所占的簇被存入其他数据，文件头也被覆盖，这个文件在文件分配表中的信息就会被新的文件映射所代替，这个文件一般也就无法恢复了。恢复文件，其实就是用恢复软件的查找分析功能找出文件头，重写前2个代码，并修改文件分配表中的映射记录。仅仅是删除的文件，恢复起来比较容易，如果整个磁盘被格式化了，恢复的困难就更大些，但是只要恢复软件能搜寻，并分析到它的残存的文件头，就有可能利用文件头中的信息，连接文件原来占用的簇，以恢复被删除的文件。然而，如果一个文件的某些簇被其他数据覆盖，即使恢复软件强行把原来占用各簇的数据连接起来恢复文件，但是因为其中的某些簇已不是该文件自身的数据，所以这个恢复后的文件往往无法使用。

​	NTFS文件系统中。在文件被彻底删除之后，系统会将此文件记录中的状态字节由文件使用标志“01”变为文件删除标志“00”。而其文件名称、文件的大小以及Run List等重要属性都不会发生变化。而且数据区中的内容也不会发生变化，因此可以将文件数据恢复但是。如果该文件不是使用连续的簇号来存储文件数据，或者文件的数据区已被新的数据所占用，即原数据区被覆盖，那么。文件就很难被恢复出来。如果数据是使用连续的簇号存储。且数据区中的数据没有被新数据覆盖，那么，就可以使用WinHex等常用恢复软件进行恢复只需要选择数据区的全部内容并进行复制，然后将该文件的数据区中所有十六进制值保存为一个新文件，即可实现文件的恢复。