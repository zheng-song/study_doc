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

```c++
typedef struct{
  ATTRIBUTE_TYPE AttributeType;
  ULONG Length;
  BOOLEAN Nonresident;//true（01） = 非常驻属性
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

#### AttributeStandardInformation（10）

![image](http://www.blogfshare.com/wp-content/uploads/images/NTFS_7E72/image_thumb_9.png)

其中0x20处的文件属性解释如下：

![image](http://www.blogfshare.com/wp-content/uploads/images/NTFS_7E72/image_thumb_10.png)

```c++
typedef struct{
  ULONGLONG CreateTime; //文件创建时间
  ULONGLONG ChangeTime;	//文件修改时间
  ULONGLONG LastWriteTime;//MFT变化时间
  ULONGLONG LastAccessTime;//文件访问时间
  ULONG FileAttributes;	// 文件属性
  ULONG AlignmentOrReservedOrUnkonwn[3]; 
  ULONG QuotaId; //NTFS 3.0 only 类ID，一个双向的
  ULONG SecurityId; //NTFS 3.0 only
  ULONGLONG QuotaCharge; //NTFS 3.0 only
  USN Usn;//NTFS 3.0 only
}STANDARD_INFORMATION, *PSTANDARD_INFORMATION;
```

#### AttributeAttributeList（20）

​	当一个文件需要好几个文件记录的时候，才会需要20H属性。20H属性记录了一个文件的下一个文件记录的位置。如下：

![wKiom1LTRoahX81uAAKQe4fYOWg137](http://www.blogfshare.com/wp-content/uploads/images/NTFS_7E72/wKiom1LTRoahX81uAAKQe4fYOWg137_thumb.jpg)

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

#### AttributeFileName(30)

​	用于存储文件名，是常驻属性，最少68字节，最大578字节，可容纳最大unicode字符的文件名长度。

![image](http://www.blogfshare.com/wp-content/uploads/images/NTFS_7E72/image_thumb_11.png)

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
}FILENAME_ATTRIBUTE, *PFILENAME_ATTRIBUTE;//67B
```

- 备注：

  文件名属性必须是resident

#### AttributeObjectId(40)

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

#### AttributeSecurityDescriptor(50)

安全描述属性作为一个标准自持的(self-relative)安全描述存储在磁盘上。在NTFS3.0 格式的卷上，这个属性通常不存在于MFT条目当中。

#### AttributeVolumeName(60)

卷名(volume name)属性仅仅包含一个Unicode编码的字符串volume label。

#### AttributeVolumentInformation(70)

```c++
typedef struct {
  ULONG Unknown[2];
  UCHAR MajorVersion;
  UCHAR MinorVersion;
  USHORT Flags;
} VOLUME_INFORMATION, *PVOLUME_INFORMATION; 
```

#### AttributeData(80H)

​	Data 属性容纳着文件的内容(未命名数据流)，文件的大小一般指的就是未命名的数据流的大小。该属性没有最大最小限制，最小情况是该属性为常驻属性。最复杂的情况是为非常驻属性。它的结构为属性头加上数据流，如果数据流太大，则标记为非常驻，以运行的方式索引到外部。例如找一个MP3文件，它的MFT项中的80属性中可以看到它一定是非常驻的，它的运行所指向的一系列簇就是音乐文件的数据流。

一个示例如下：

![image](http://www.blogfshare.com/wp-content/uploads/images/NTFS_7E72/image_thumb_12.png)

​	当属性不能存放完所有的数据，系统就会在NTFS数据区域开辟一个空间存放，这个区域是以簇为单位的。Run List就是记录这个数据区域的起始簇号和大小，这个示例中，Run List的值为“12 41 47 03”，因为后面是00H，所以知道已经是结尾。如何解析这个Run List呢？ 
​	第一个字节是压缩字节，高位和低位相加，1+2=3，表示这个Data Run信息占用三个字节，其中高位表示起始簇号占用多少个字节，低位表示大小占用的字节数。在这里，起始簇号占用1个字节，值为03，大小占用2个字节，值为47 41。解析后，得到这个数据流起始簇号为3，大小为18241簇。

#### AttributeIndexRoot(90H)

​	  90H属性是索引根属性，是实现NTFS的B+树索引的根节点，它总是常驻属性。索引根属性由属性头、索引根和索引项组成。属性头是通用属性头的常驻部分。

如下图：

![image](http://www.blogfshare.com/wp-content/uploads/images/NTFS_7E72/image_thumb_13.png)

索引根的结构如表：

[![image](http://www.blogfshare.com/wp-content/uploads/images/NTFS_7E72/image_thumb_14.png)](http://www.blogfshare.com/wp-content/uploads/images/NTFS_7E72/image_14.png)

索引头的结构如表：

[![image](http://www.blogfshare.com/wp-content/uploads/images/NTFS_7E72/image_thumb_15.png)](http://www.blogfshare.com/wp-content/uploads/images/NTFS_7E72/image_15.png)

索引项结构如表：

![image](http://www.blogfshare.com/wp-content/uploads/images/NTFS_7E72/image_thumb_16.png)

```c++
typedef struct {
  ATTRIBUTE_TYPE Type;	// 4B 属性类型
  ULONG CollationRule;	// 4B 排序规则
  ULONG BytesPerIndexBlock;	// 4B 索引项分配大小
  UCHAR ClustersPerIndexBlock; // 每索引记录的簇数
  UCHAR noUse[3];
  DIRECTORY_INDEX DirectoryIndex;
} INDEX_ROOT, *PINDEX_ROOT;

typedef struct{
  ULONG firstIdxOffset; // 第一个索引项的偏移
  ULONG IdxTotalsize; // 索引项的总大小
  ULONG IdxAllocSize;	// 索引项的分配大小
  UCHAR Flags; //0x00 = Small directory, 0x01 = Large directoty
  UCHAR noUse[3];
}DIRECTOTY_INDEX, *PDIRECTOTY_INDEX;

typedef struct {
  ULONGLONG MFTnum;//文件的MFT参考号，前6B是改文件的MFT编号，用于定位此文件
  USHORT ItemLen;//索引项的大小
  UCHAR IdxIdentifier[2];//索引标志
  UCHAR IsNode[2];//1--此索引包含一个子节点，0--此为最后一项
  UCHAR noUse[2];//填充
  ULONGLONG FMFTnum;//父目录的MFT参考号，0x05表示根目录。
  ULONGLONG	CreateTime;
  ULONGLONG FileModifyTime;
  ULONGLONG	MFTModifyTime;
  ULONGLONG LastVisitTime;
  ULONGLONG	FileAllocSize;
  ULONGLONG FileRealSize;
  UCHAR FileIdentifier[8];//文件标识
  UCHAR FileNameLen;//文件名长度F
  UCHAR FileNameSpace;//文件名命名空间
  //---0x52--- 文件名，长度为2F，并在其后填充到8字节
  ULONGLONG NextItemVCN；
}INDEXITEM,*PINDEXITEM;
```

- 备注

  一个INDEX_ROOT结构后面跟随者一个DIRECTORY_ENTRY结构序列。



##### 90H属性详解

1. 第一种情况：

![](http://upload-images.jianshu.io/upload_images/6128001-8fd4c0f8759659cc.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

![](http://upload-images.jianshu.io/upload_images/6128001-7824e3fe9d7dc32b.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)





#### AttributeIndexAllocation(A0H)

​	A0属性是索引分配属性，存储着组成索引的B+树目录索引子节点的定位信息。A0属性由属性头和运行列表组成，一般指向一个或者是多个目录(INDEX文件，即4K的缓存)，它总是常驻属性。A0属性和90属性共同描述了磁盘文件和目录的MFT记录的位置。第五项MFT的A0属性记录根目录的位置。

如下示例：

![image](http://www.blogfshare.com/wp-content/uploads/images/NTFS_7E72/image_thumb_17.png)

​	根据上图A0H属性的“Run List”可以找到索引区域，偏移到索引区域所在的簇，`21 03 59 47`   ==> 起始簇：0x4750==18265 。簇大小：3

起始扇区号 = 该分区的起始扇区+ 簇号*每个簇的扇区数 即：

64 + 18265*8  = 146124

![image](http://www.blogfshare.com/wp-content/uploads/images/NTFS_7E72/image_thumb_18.png)

上图的偏移0x28还要加上0x18 = 0x40。

标准索引头的解释如下：

![image](http://www.blogfshare.com/wp-content/uploads/images/NTFS_7E72/image_thumb_19.png)

索引项的解释如下：

![image](http://www.blogfshare.com/wp-content/uploads/images/NTFS_7E72/image_thumb_20.png)

```c++
typedef struct{
  NTFS_RECORD_HEADER Ntfs;
  ULONGLONG IndexBlockVcn;
  DIRECTORY_INDEX DirectoryIndex;
} INDEX_BLOCK_HEADER, *PINDEX_BLOCK_HEADER;
```

- 备注: 

  index allocation属性是一个块索引数组(an array of index blocks).每一个块索引以一个INDEX_BLOCK_HEADER结构开始，之后是一个DIRCETORY_ENTRY结构序列。

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



####  索引头部

```
typedef struct {
	UCHAR Mark[4];			// 标志为"INDX"
	USHORT UsnOffset;		//更新序列的偏移
	USHORT UsnUpdateArrSize; //更新序列数组的大小
	UCHAR LSN[8];//日志文件序列号
	ULONGLONG VcnIndex;//本索引缓存在分配索引中的VCN
	ULONG ItemOffset;//第一索引项的偏移
	ULONG ItemSize;//索引项的实际大小(B)
	ULONG ItemAllocSize;//索引项的分配大小(B，不包括头部)，略小于4K
	UCHAR IsNode;//是节点则为1，否则表示有子节点。
	UCHAR Nouse[3];//填充
//	UCHAR USN[2 * UsnUpdateArrSize];    //此处有一个使用上面的UsnUpdateArrSize*2大小的字节的USN号
};
```

​	索引头部之后的内容就是索引项，索引项的结构和上面0x90属性中介绍的ItemIndex是一致的，每一项代表的是一个文件或者是目录的MFT项，通过项的MFT记录号皆可以计算出MFT项的磁盘地址，他等于\$MFT的偏移地址+编号*0x400,以此可以找到该索引项对应的文件或者是子目录。

```
typedef struct {
  ULONGLONG MFTnum;//文件的MFT参考号，前6B是改文件的MFT编号，用于定位此文件
  USHORT ItemLen;//索引项的大小
  UCHAR IdxIdentifier[2];//索引标志
  UCHAR IsNode[2];//01--此索引项包含一个子节点，02--此为最后一项
  UCHAR noUse[2];//填充
  ULONGLONG FMFTnum;//父目录的MFT参考号，0x05表示根目录。
  ULONGLONG	CreateTime;
  ULONGLONG FileModifyTime;
  ULONGLONG	MFTModifyTime;
  ULONGLONG LastVisitTime;
  ULONGLONG	FileAllocSize;
  ULONGLONG FileRealSize;
  UCHAR FileIdentifier[8];//文件标识
  UCHAR FileNameLen;//文件名长度F
  UCHAR FileNameSpace;//文件名命名空间
  //---0x52--- 文件名，长度为2F，并在其后填充到8字节
  //ULONGLONG NextItemVCN；
}INDEXITEM,*PINDEXITEM;
```





#### 搜索一个已删除的文件或者是目录的MFT项

​	一个文件的MFT项的地址等于\$MFT的地址+MFT编号*0x400，如果目录中的对应项删除了，那么可以从MFT的首部开始搜索，因为MFT一般是连续的，而一个MFT项的大小又是固定的，一项项读取，找到各自的0x30属性，解析出文件名，进行比较(MFT中有一些空白区域需要跳过)。



#### 备注：

##### 1.关于文件名

一般文件名的前一个字节是文件名的命名空间，不管是在INDEX文件中，还是在0x30属性中。

- 0x00----POSIX，最大命名空间，大小写敏感，支持除了`\0`和`/`的所有Unicode字符，最大255字符。
- 0x01----Win32，是POSIX的子集，不支持字符:`* / < > | \ : ? `,不能用句点或空格结束
- 0x02----DOS，是Win32的子集，字符必须要比空格0x20大，文件名为1~8个字符，然后句点分割接后缀扩展名1~3个字符。
- 0x03----DOS&Win32，必须要兼容Win32和DOS命名方式。

 在INDX文件中，经常可以看到含有0x02和0x03或者0x01的两个不同命名空间、相同MFT编号的项，也就是说这两个目录项指向同一个记录，同样的在这个文件的MFT项中也有两个0X30属性，其中一个是0x01或0x03，表示的是完整的文件名；另一个是0x02，DOS命名方式，它是一个短文件名，它在我们命名的基础上，截断 '.' 之前的超出6个字符的所有字符，只剩前6个，之后接上"~1" ，这样正好8个字符，当然后面的句点和扩展名保留。另外，它必须满足DOS命名规则，必须大写，删除禁止使用的字符等等。如果文件名重复了，在 "~1" 基础上递增，"~2","~3"等等。检索比对时，我们自然要使用前者。

##### 2. 关于字符集

字符集是字符在计算机上的编码方式，可以看成一种协议，一种约定规则，我们处理一串二进制数所代表的字符时，必须清楚它用的是哪一种编码方式；

在windows系统中文件的命名是固定用两个字节表示一个字符，在MFT中可以发现英文文件名字符之间都填充一个 '\0' ，这是宽字符集与变长字符集兼容，

在宽字符集中，小于128的字符数值上是等于ASCII码；我们的文件数据一般用的是变长字符集(GB2312等等)；

为了比较输入的文件名和NTFS中的文件名，我们必须要先转换；

两个WinAPI 函数，用于宽字符和变长字符转换

```c++
// 函数原型
int WideCharToMultiByte(
            UINT     CodePage,            // code page
            DWORD    dwFlags,            // performance and mapping flags
            LPCWSTR  lpWideCharStr,     // address of wide-character string
            int      cchWideChar,        // number of characters in string
            LPSTR    lpMultiByteStr,        // address of buffer for new string
            int      cchMultiByte,        // size of buffer
            LPCSTR   lpDefaultChar,        // address of default for unmappable 
                                       　　 // characters
            LPBOOL   lpUsedDefaultChar    // address of flag set when default 
                                        // char. used
);
int MultiByteToWideChar(
            UINT   CodePage,         // code page
            DWORD  dwFlags,         // character-type options
            LPCSTR lpMultiByteStr, // address of string to map
            int    cchMultiByte,      // number of bytes in string
            LPWSTR lpWideCharStr,  // address of wide-character buffer
            int    cchWideChar        // size of buffer
);

//--- WCHAR 定义在tchar.h中 ----
void charTest()
{
    TCHAR    tc1[16] ;　　//=  _T("后来");
    WCHAR    tc2[8] = {0x540E, 0x6765, 0, 0, 0, 0, 0, 0};
//    memset(tc2, 0, 20);
//  MultiByteToWideChar(CP_ACP, 0, tc1, 4, (LPWSTR)tc2, 4);
    WideCharToMultiByte(CP_ACP, 0 ,(WCHAR*)tc2, 2, tc1, sizeof(tc1), 0, 0);　　
    
    cout<<"tc1 "<<tc1<<sizeof(tc1)<<" "<<strlen(tc1)<<endl;
    PrintHex(tc1);
    cout<<endl;
    cout<<"tc2 "<<sizeof(tc2)<<" "<<wcslen((LPWSTR)tc2)<<endl;
    PrintHex(tc2);
    cout<<endl;
}
```





##遍历一个分区下面的文件列表的思路

1. 定位DBR，通过DBR可以得知“$MFT”的起始簇号及簇的大小。
2. 定位“$MFT”，找到“$MFT”后，在其中寻找根目录的文件记录，一般在5号文件记录。
3. 在90H属性中得到B+树索引的根节点文件信息，重点在A0属性上。通过属性中的“Run List”定位到其数据流。
4. 从“Run List”定位到起始簇后，再分析索引项可以得到文件名等信息。
5. 从索引项中可以获取“$MFT”的参考号，然后进入到“$MFT”找到对应的文件记录。
6. 然后再根据80H属性中的数据流就可以找到文件真正的数据了。



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








## 对于NTFS 文件系统的研究

​	NTFS分区的最开始的16个扇区是分区引导扇区，用于保存分区引导代码，接下来是主文件表(MFT),如果MFT所在的磁盘扇区出现损坏，NTFS文件系统会将MFT转移到到硬盘的其他扇区，这样就保证了NTFS文件系统和Windows操作系统的正常运行。比之先前的FAT16和FAT32文件系统的FAT(文件分配表)，FAT只能固定在分区引导扇区的后面，一旦该扇区，整个文件系统就会瘫痪，NTFS文件系统显然要先进的多了。不过这种移动MFT的做法却也并非十全十美，如果分区引导代码中指向MFT的部分出现错误，那么NTFS文件系统便会不知道到哪里寻找MFT ，从而会报告“磁盘没有格式化”这样的错误信息。为了避免这样的问题发生，分区引导代码中会包含一段校验程序，专门负责侦错。

​	MFT中的文件记录大小一般是固定的，不管簇的大小是多少，均为1KB(0x400)。文件记录在MFT文件记录数组中物理上是连续的，且从0开始编号，所以，NTFS是预定义文件系统。MFT仅供系统本身组织、架构文件系统使用，这在NTFS中称为元数据（metadata，是存储在卷上支持文件系统格式管理的数据。它不能被应用程序访问，只能为系统提供服务）。其中最基本的前16个记录是操作系统使用的非常重要的元数据文件。这些元数据文件的名字都以\$ 开始，所以是隐藏文件，在Windows 2000/XP中不能使用dir命令（甚至加上/ah参数）像普通文件一样列出。

​	NTFS使用逻辑簇号(Logical Cluster Number,LCN)和虚拟簇号(Virtual Cluster Number, VCN)来对簇进行定位。LCN是对整个卷中所有的簇从头到尾进行的简单编号。用卷因子乘以LCN，NTFS就能够得到卷上的物理字节偏移量，从而得到物理磁盘地址。VCN则是对属于特定文件的簇从头到尾进行编号，以便于引用文件中的数据。VCN可以映射成LCN，而不必要求在物理上连续。

​	在NTFS卷上，跟随在BPB后的数据字段形成一个扩展BPB。这些字段中的数据使得Ntldr能够在启动过程中找到主文件表MFT（Master File Tabl ）。在NTFS卷上，MFT并不像在FAT 16卷和FAT 32卷上一样，被放在一个预定义的扇区中。由于这个原因，如果在MTF的正常位置中有坏扇区的话，就可以把MFT移到别的位置。但是，如果该数据被破坏，就找不到MFT的位置，Windows 2000假设该卷没有被格式化。因此，如果一个ntfs的卷提示未格式化，可能并未破坏MFT,依据BPB的各字段的意思是可以重建BPB的。

​	NTFS中文件通过主文件表（MFT）确定其在磁盘上的位置。MFT是一个数据库，由一系列文件记录组成。卷中每一个文件都有一个文件记录，其中第一个文件记录称作基本文件记录，里面存储有其他扩展文件记录的信息。主文件表也有自己的文件记录。

​	NTFS卷上的每个文件都各有一个唯一的64位的文件引用号（File Reference Number，也称文件索引号）。文件引用号由两部分组成：文件号和文件顺序号。文件号48位，对应该文件在MFT中的位置；文件顺序号随着文件记录的重用而增加（考虑到NTFS一致性检查）。NTFS目录只是一个简单的文件名和文件引用号的索引。如果目录的属性列表小于一个记录长，那么该目录所有信息存储在MFT的记录中，否则大于一个记录长的使用B+树结构进行管理（B+树便于大型目录文件和子目录的快速查找）。

​	MFT的基本文件记录中有一指针，指向一个存储非常驻的索引缓冲，包括该目录下所有下一级子目录和文件的外部簇。

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

​	NTFS文件系统被创建时，会同时建立一些重要的系统信息。这些系统信息也全是以文件的形式存在，被称为元文件。元文件的文件名都以“ $”符号开头，表示其为隐藏的系统文件，用户不可直接访问。但是能够被特殊的工具列出，如NFI.EXE。

​	NTFS中的所有的一切皆文件，MFT也是一个文件。同时MFT也有一个它自己的entry，MFT中的第一个entry叫做\$MFT，它描述了MFT在磁盘上的位置。实际上，这是唯一一个描述MFT在磁盘上的位置的地方；也就是说，***你需要处理这个entry才能知道MFT的布局和大小***。

例如：

![image](https://images0.cnblogs.com/blog/646229/201501/242257382662732.png)

在Microsoft实现的NTFS中，MFT开始保持尽量小尺寸，当需要更多的entry时扩展MFT。理论上说，操作系统可以在创建文件系统的时候创建固定数量的entry，但是Microsoft实现动态特性允许很方便的通过分卷扩展文件系统容量。Microsoft在MFT entry创建后不删除它们。

### MFT entry的内容

​	每一个MFT entry的大小定义在引导扇区中，但是Microsoft使用的所有版本都使用1024字节大小。数据结构开始的42个字节包含12个域，剩余的982字节没有特定的结构，可以使用属性来填充。每一个MFT entry的第一个域是签名，一个标准的entry是ASCII字符串“FILE”，如果一个entry里面发现了错误，它可能含有字符串 “BAAD”，还有一个标志域用来标识这个entry是否使用、这个entry是否是一个目录。一个MFT entry的分配状态也可以通过\$MFT文件的\$BITMAP属性来检查。如果一个文件无法把它的所有属性放进一个entry，它可以使用多个entry。当这种情况发生的时候第一个entry被称为基本文件记录，或者基本MFT entry，后续的每一个entry都在它的固定域保存有基本entry的地址。



### MFT entry地址

​	每一个MFT entry都使用一个48位的顺序地址，第一个entry地址是0。MFT最大地址随着MFT的增长而增大，它的值由\$MFT的大小除以每一个entry的大小来计算(**???没明白**)。Microsoft称这个顺序地址为文件编号(file number). 每一个MFT entry还有一个16位的顺序号，当entry分配的时候这个顺序号增长。例如，想象一下MFT entry 313的顺序号是1，entry 313的文件删除了，然后这个entry被分配给一个新的文件。当这个entry被重新分配的时候，他得到一个新的顺序号2. MFT entry和顺序号组合起来，顺序号放在高16位，组成一个64位的文件参考地址。



![image](https://images0.cnblogs.com/blog/646229/201501/242257394389189.png)

MFT 使用文件参考地址来引用MFT entry,这是由于顺序号使得检查文件系统是否损坏变得容易。例如，如果系统在一个文件的数据结构被分配的时候崩溃了，顺序号可以用来判断一个数据结构是否包含有MFT entry，是因为上一个文件使用了它还是它是新文件的一部分。我们也可以用它来恢复被删除的内容。例如，如果我们有一个未分配的数据结构，里面有文件参考号，我们可以判断这个数据结构被使用以来MFT entry是否被重新分配过。顺序号在信息挖掘中有很大的作用，但是我们主要研究的是文件号，或者说是MFT entry的地址。

​	Microsoft保留了前16个MFT entry作为文件系统元文件(Microsoft文档声称只保留了前16个entry，但是实际上第一个用户文件或者是目录从entry 24开始。entry 17 到23作为缓冲entry防止预留的不够用导致溢出。)这些个保留并且未被使用的entry被置为分配状态并且只有基本的信息。 每一个文件系统元文件都列在根目录，通常情况下普通用户是看不到他们的。

 一些元文件的具体含义如下：

- **$MFT:** 它其实是整个主文件表，也就是将整个MFT看做一个文件。
- **$MFTMirr:** MFT前几个MFT项的备份，NTFS也将其作为一个文件看待。
- **$LogFile:** 日志文件。是NFTS为了实现可恢复性和安全性而设计的。当系统运行时，NFTS就会在日志文件中记录所有影响NTFS卷结构的操作，包括文件的创建和改变目录结构的命令，例如复制，从而在系统失败时能够恢复NTFS卷。
- **$Volume:** 卷文件，包含卷名、被格式化的NTFS版本和一个标明该磁盘是否损坏的标志位。
- **$AttDef:** 属性定义列表，存放了卷支持的每种属性的名字和类型。并指出他们是否可以被索引和恢复等。
- **$Root:** 根目录文件，保存了存放于该卷根目录下的所有的文件和目录的索引。访问了一个文件之后，NTFS就保留了该文件的MFT引用，第二次就能够直接进行对该文件的访问了。
- **$Bitmap:** 位图文件，NFTS的分配状态都存放在位图文件当中，它的数据属性的每一个bit对应文件系统中的一个簇，用以描述簇的分配情况。由于该文件可以很容易的被扩大，所以NTFS卷可以很方便的动态扩大，而FAT格式的文件系统由于涉及到FAT表的变化，所以不能够随意的对分区进行调整。
- **$Boot：** 引导文件，DBR扇区就是引导文件的第一个扇区。
- **$BadClus：** 坏簇记录文件。
- **$Quota：** 早期的NT系统中记录磁盘配额信息。
- **$Secure：** 安全文件。它存储了整个卷的安全描述符数据库。NTFS文件和目录都有各自的安全描述符，为了节省空间，NTFS将具有相同描述符的文件和目录存放在一个公共文件中。
- **$UpCase:** 大小写字符转换表文件。
- **$Extend metadata directory:** 扩展元数据目录。
- **$Extend\$Reparse：** 重解析点文件.
- **$Extend\$UsnJrnl：** 变更日志文件。
- **$Extend\$Quota:** 配额管理文件。
- **$Extend\$ObjId：**对象ID文件。


每一个MFT记录都对应着不同的文件，如果一个文件具有多个属性或者是分散存储，那么就可能需要多个文件记录。其中第一个记录称为基本文件记录(Basic File Record)。MFT中的第一个记录就是MFT本身。由于MFT文件本身的重要性为确保文件系统结构的可靠性，系统专门为他准备了一个镜像文件\$MFTMirr.

​	NTFS访问卷的过程如下：

- NTFS访问某个卷时，它必须要“装载”该卷：NTFS会查看引导文件($Boot元数据文件定义的文件)，找到MFT的物理磁盘地址。
- 它就从文件记录的数据属性中获得VCN到LCN的映射信息，并存储在内存当中。这个映射信息定位了MFT的运行(run或者是extent)在磁盘上的位置。
- NTFS在打开几个元数据文件的MFT记录，并打开这些文件。如果有必要NTFS开始执行它的文件系统恢复操作。在NTFS打开了剩余的元数据文件后，用户就可以开始访问该卷了。

NTFS中第一个扇区DBR是一个文件，也是\$ROOT文件的第一个扇区。\$MFT也是文件，记录\$MFT文件信息就是它本身。\$MFT的文件记录第一项是\$MFT,第八项是\$ROOT。

系统通过DBR找到\$MFT，然后由\$MFT定位和确定\$ROOT。在所有的NTFS分区中，\$BOOT占用前16扇区。引导分区中\$BOOT的代码量一般占用7个扇区（0~6扇区），后面为空，这些代码是系统引导代码。引导分区和非引导分区的1~6扇区内容一致，区别是第0个扇区。如果启动分区\$BOOT文件损坏，可以用其他分区的\$BOOT文件恢复。

![MFT与DBR的关系.PNG](http://upload-images.jianshu.io/upload_images/6128001-3ab13e25934af0d3.PNG?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

\$MFT的起始位置由DBR的BPB参数确定，\$MFTMirr的起始位置也由DBR的BPB参数确定。在NTFS中，除了DBR本身是预设的，其他文件信息都存储在\$MFT中。

### MFT entry 属性概念

​	MFT entry只有很少的内部结构，它的大部分的空间都被用来存储属性，属性是用来存储特定类型数据的数据结构。 属性有许多种，每种都有其独特的内部结构。这是NTFS相较于其他的文件系统的不同之处之一。大部分文件系统仅仅用来读写文件内容，而NTFS读写属性，属性的一种包含有文件的内容。

​	尽管所有的属性都保存着不同类型的数据，所有的属性都有两个部分：头部和内容。下图显示了一个MFT entry有四个头部(三个属性头部，一个MFT entry头部)和内容对。头部是所有属性通用的(大小固定)。内容和属性类型相关，并且可以是任意的大小。

![image](https://images0.cnblogs.com/blog/646229/201501/312253060814892.png)

#### 属性头部

​	头部标识了属性的类型、大小和名称。头部还有是否压缩和加密的标识。属性类型是一个数值的标识用来标识数据的类型，一个MFT entry可以有多个相同类型的属性。有一些属性可以被赋予一个名字，以UTF-16的unicode字符串的形式存储在属性头部。

![MFTEntryAttribut](http://img.blog.csdn.net/20180112190513952?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

- 常驻属性的属性头

| 偏移(16进制) | 长度(Byte) | 常用值(16进制)         | 含义                     |
| -------- | -------- | ----------------- | ---------------------- |
| 00-03    | 4        |                   | 属性类型                   |
| 04-07    | 4        | 48 00 00 00(72字节) | 该属性的总长度(属性头+属性体)       |
| 08       | 1        | 00                | 是否为常驻属性，00表示为常驻        |
| 09       | 1        | 00                | 属性名的长度，长度为2*NameLength |
| 0A-0B    | 2        | 1800              | 属性名开始的偏移               |
| 0C-0D    | 2        | 00                | 压缩、加密、稀疏标志             |
| 0E-0F    | 2        | 00                | 属性ID                   |
| 10-13    | 4        | Length            | 属性体的长度                 |
| 14-15    | 2        | 18                | 属性体开始的偏移               |
| 16       | 1        |                   | 索引标志                   |
| 17       | 1        |                   | 填充                     |
| 18       | Length   |                   | 属性体开始                  |

- 非常驻属性的属性头

| 偏移(16进制) | 长度   | 常用值           | 含义                |
| -------- | ---- | ------------- | ----------------- |
| 00-03    | 4    |               | 属性类型              |
| 04-07    | 4    |               | 属性长度              |
| 08       | 1    | 01            | 是否常驻属性，01表示是非常驻属性 |
| 09       | 1    | 00            | 属性名长度             |
| 0A-0B    | 2    |               | 属性名开始的偏移量         |
| 0C-0D    | 2    |               | 压缩、加密、稀疏标志        |
| 0D-0E    | 2    |               | 属性ID              |
| 10-17    | 8    |               | 起始虚拟簇号VCN         |
| 18-1F    | 8    |               | 结束虚拟簇号VCN         |
| 20-21    | 2    | 40(即从第64字节开始) | Data run的偏移地址     |
| 22-23    | 2    |               | 压缩单位的大小，2的N次方     |
| 24-27    | 4    |               | 不使用               |
| 28-2F    | 8    |               | 属性分配大小            |
| 30-3F    | 8    |               | 属性的实际大小           |
| 38-3F    | 8    |               | 属性的原始大小           |
| 40       |      |               | Data run 的信息      |



#### 属性内容

​	属性的内容可以是任意格式任意大小。例如，一种属性被用存储文件的内容，所以其大小可能是几个MB乃至数个GB。将这么多的数据存储在仅仅有1024个字节大小的MFT entry中是不切实际的。为了解决这个问题，NTFS提供了两种存储属性内容的位置。常驻属性将它的内容和头部一同存储在MFT entry当中。这仅仅适用于小属性，非常驻属性将其内容存储在文件系统的簇当中。属性的头部中标识了这个属性是常驻属性还是非常驻属性。如果一个属性是常驻的，则其内容紧跟头部。如果一个属性是非常驻的，头部将会给出簇地址。

​	非常驻属性会被保存在cluster runs当中，由连续的簇构成，而每一个run由起始簇地址和run长度来描述。例如，如果一个属性分配了48、49、50、51和52这几个簇，则它有一个run，这个run开始于簇48，长度为5.如果这个属性还分配了簇80和81，则它有第二个run，起始于80长度为2。第三个run可能起始于簇56长度为4.如下图所示：

![image](https://images0.cnblogs.com/blog/646229/201501/312253088474636.png)

​	NTFS使用了VCN到LCN的映射来描述非常驻属性的run。上图中，这个属性run显示了VCN地址0-4映射到了LCN地址48-52, VCN地址5-6映射到了LCN地址80-81，VCN地址7-10映射到了LCN地址56-59。



### 标准属性类型

​	每一种属性类型都被定义了一个编号，微软使用这个编号对一个entry中的属性进行排序。标准属性被赋予了缺省的类型值，但我们可以在\$AttrDef文件系统元文件中重定义。除了编号之外，每一个属性类型有一个名字，都是大写并且以“\$”开头。几乎每一个被分配的MFT entry都有\$FILE_NAME和\$STANDARD_INFORMATION类型的属性。每一个文件都有一个\$DATA属性，其包含有文件的内容。如果内容尺寸超过大约700字节，他就会变成一个非常驻属性，保存到到外面的簇中。当一个文件含有多于一个\$DATA 属性时，这些多出的属性有时被称为附加数据流(alternate data streams，ADS)。当创建文件时创建的缺省\$DATA并没有名字，但是多出的\$DATA属性必须有。注意属性名字和类型名字是不同的。例如，\$DATA是属性类型名，属性的名字可能是“fred”。

​	每一个目录都有一个\$INDEX_ROOT属性，这个属性含有其包含的文件和目录的信息。如果这个目录很大，\$INDEX_ALLOCATION和\$BITMAP属性也用于存储信息。令事情更加复杂的是，一个目录也可以在\$INDEX_ROOT之外还含有额外的\$DATA属性。也就是说，目录可以同时存储文件内容和其所包含的文件和子目录的列表。\$DATA属性可以存储应用程序或者是用户想要存储的任何数据。一个用户的\$INDEX_ROOT和\$INDEX_ALLOCATION属性通常其名字是`$I30`。



#### 基础MFT entry

​	一个文件最多可以有65536个属性（由于标识符是16位的），所以其可能需要多于一个MFT entry来保存它的所有的属性的头部（即便是非常驻属性也需要它们的头部保存在MFT entry内部）。当附加的MFT entry被分配给一个文件的时候，原来的MFT entry被称为基础MFT entry。非基础entry会在它们的MFT entry域中保存有基础MFT entry的地址。

​	基础MFT entry有一个\$ATTRIBUTE_LIST类型的属性，这个属性含有文件的每一个属性和MFT地址，以便于查找到它们。非基础MFT entry没有\$FILE_NAME和\$STANDARD_INFORMATION属性。



稀疏属性

NTFS可以降低文件的空间需求，这是通过将某些非常驻$DATA属性的数据保存为稀疏来实现的。稀疏属性是那些全为零的簇没有写入磁盘的属性。作为替代，一个特殊的run用来保存零簇（译注：含有全零数据的簇）。一般来说，一个run含有起始簇位置和长度，但是一个稀疏run只含有长度没有起始位置。有一个标志指出一个属性是稀疏的。

例如，假设一个文件占用了12个簇。起始的五个簇非零，接下来的三个簇都是零，最后的四个簇非零。当它保存为一个普通的属性，一个长度为12的run将会被创建出来以保存这个文件，如图11.8A所示。当保存为稀疏属性，将会有三个run被创建出来，并且只有9个簇被实际分配，如下图所示。

一个12簇长的文件被保存为（A）普通布局或，（B）稀疏布局，有三个簇在稀疏run中：

![image](https://images0.cnblogs.com/blog/646229/201501/312312583949448.png)







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

​	以上引导扇区最为关键的字节数是

- 0B-0C（每扇区字节数） 

- 0B-0C（每扇区字节数） 

- 0D（每簇扇区数）

- 28-2F（文件系统扇区总和） 

- 30-37（MFT起始簇号，根据这个起始簇号来找到MFT的位置，假设此处的内容为0000 0C00 0000 0000，那么MFT的偏移量为0x0C0000(**小端模式**)，而根目录在MFT中的第六个文件记录）

- 38-3F（MFT备份的起始簇号） 

- 40（每MFT项大小）

- 44（每个索引的簇数）

当数据发生不可预料的损坏时，可以根据以上信息重建分区表，定位数据区，恢复MFT，重建DBR，这些关键字节码的用处不言而喻。

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



##### MFT布局

![MFT布局](http://img.blog.csdn.net/20180117201032032?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)



### NTFS 中建立文件的过程

​	假设我们要加里一个文件“\子目录1\file.txt”,假设子目录1已经存在于根目	录下。要建立的文件的大小为7000个字节，每一个簇的大小为4096个字节。

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

### 文件删除原理

​	当文件被彻底删除以后，系统会将其主文件表MFT记录收回，并加上删除标记，同时将该文件所占用的数据区也一并收回，并把$Bitmap元文件中的簇的表示信息使用清空，并在对应的位置上设置为0。对于删除了的文件的文件记录和数据区所占用的所有空间都可以再分配给其他的文件使用。

​	NTFS中的文件删除以后，只要原先的数据空间没有被新的数据所覆盖的话，只要FILE记录还存在，则将很容易找到文件数据的详细存储位置，并能将这些数据准确的恢复出来。如果删除的文件包含在其他的文件夹当中，那么文件被删除以后，这个文件夹也要将该文件的目录项删除。

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

对于数据的恢复，可以根据不同的删除原理进行数据恢复：

### 文件删除后的恢复 

​	NTFS文件系统中。在文件被彻底删除之后，系统会将此文件记录中的状态字节由文件使用标志“01”变为文件删除标志“00”。而其文件名称、文件的大小以及Run List等重要属性都不会发生变化。而且数据区中的内容也不会发生变化，因此可以将文件数据恢复但是。如果该文件不是使用连续的簇号来存储文件数据，或者文件的数据区已被新的数据所占用，即原数据区被覆盖，那么。文件就很难被恢复出来。如果数据是使用连续的簇号存储。且数据区中的数据没有被新数据覆盖，那么，就可以使用WinHex等常用恢复软件进行恢复只需要选择数据区的全部内容并进行复制，然后将该文件的数据区中所有十六进制值保存为一个新文件，即可实现文件的恢复。

### 文件格式化后的恢复

​	文件的格式化其实是给分区重新建立文件系统，格式化以后，分析分区的MFT文件可以发现，文件的记录和内容还是完好的存在的，其数据区也没有改变。所以，通过runlist就可以找到这些数据，并有可能将其恢复出来。但是如果文件记录也被破坏了，文件就很难被恢复。











# B+树在NFTS中的应用

## 卷(volume)

​	NTFS的结构首先从卷开始。卷对应于一个逻辑分区，当你将一个磁盘或者是磁盘的一部分格式化为NTFS，卷将被创建起来。一个磁盘可以有一个卷或者是好几个卷。NTFS独立的处理每一个卷。卷对应的就是C盘、D盘、F盘等。

## 簇(cluster)

​	在一个NTFS卷中，簇的大小，或者说是簇因子(cluster factor),是由NTFS格式化程序自动决定的。默认的簇因子随卷大小的不同而不同，但它是物理扇区的整数倍，而且总是2的幂。硬盘的读写以扇区为基本单位，NTFS的读写以簇为基本单位，通常为2个扇区。

## 逻辑簇号(LCN)

​	NTFS使用一个存储在MFT中的LCN来指向文件在磁盘上的实际的物理位置，即对簇进行定位。LCN是从一个卷的开始直到结束的所有簇的一个编号。NTFS依赖于逻辑号（LCN）来引用物理位置。假定逻辑簇号为lcn，每簇的扇区数为SPC(簇因子)，卷的隐含扇区数为HS，簇的绝对扇区号为AS，那么逻辑簇号与磁盘的扇区号的对应关系为：**LCN\*SPC+HS = AS** 。

## 虚拟簇号(VCN)

​	VCN(Virtual Cluster Number)是一个与非常驻属性相关联的概念。VCN从文件属性流的第一个运行的第一个簇(VCN 0)到最后一个运行的最后一个簇进行编号，当某个运行列表非常大，文件属性不能够放在一个文件记录中时，描述文件的文件属性就会被存储在几个文件记录当中，运行列表也分成几个小片。起始VCN域和结束VCN域都用于定位某个文件记录指示----即运行列表----运行所指定的VCN。 VCN是带符号的整数，且不必是物理上连续的。它们可以被映射到该卷上的任何LCN编号，由Data Runs完成这个映射。

## 运行列表

​	在NTFS中，运行列表用于记录文件/目录在数据区簇的分配情况，对于每一个文件/目录，其运行列表是由一系列的运行定位表组成的，每个定位表确定一个运行在卷中的具体位置，***每个定位表存储一个相对于上一个定位表中LCN的偏移量（可正可负）和该运行的簇数，对于第一个定位表的偏移，看做是相对于卷的起始LCN值0*** 。通过和树形结构中的StartVCN和LastVCN成员配合可以容易的得到VCN-LCN的映射表。NTFS文件系统利用VCN-LCN间的不同映射方式实现稀疏文件存储和数据压缩等功能。

​	data run由若干个run组成，以0结束。每一个run包括三个部分：头，簇数，簇号。簇号用VCN，第一个VCN是相对于0簇。

​	例如一个文件的数据属性的运行列表为：

`31 01 FC B0 12 21 18 BD 49 21 2F 7B A7 00` 

将其分解为4个Run：

31 01 FCBO12， 21 18 BD49， 21 2F 7BA7， 00

即文件存储在3个连续的块当中：

1. Run1：0x12B0FC簇开始的一个簇。
2. Run2：0x12B0FC+0x49BD = 0x12FAB9开始的0x18个簇。这里将VCN 0x49BD换算成LCN，它相对于前一地址0x12B0FC。
3. Run3：***注意这里的VCN是一个负数***，0xA77B = - 0x5885, 0x12FAB9-0x5885 = 0x12A234。即文件的第三块在0x12A234簇开始的0x2F个簇。也可以使用符号扩展的方法直接相加：0x12FAB9 + 0xFFA77B = 0x12A234，丢弃最高进位1。
4. Run4：0x00 ，DataRun的结束。



表示该文件内容由两个运行组成，在卷的具体位置用LCN链表示为：0x4C920 --> 0x4C931 --> 0x4c935 





## NTFS文件系统结构

### 1.1 主文件表(Master File Table)

​	MFT是NTFS卷结构的核心所在，以文件的记录来实现对文件的管理，每一个文件记录都对应着不同的文件，大小固定为1KB。文件记录由两部分组成，一部分是文件记录头，一部分是属性列表。每一个文件都是用一个文件引用号(参考号)来标识这是一个64bit的数，它由文件号(低48位)和文件顺序号(高16bit)所组成。文件号记录着文件的文件记录在MFT中的位置，文件的顺序号则记录着文件记录位置被重复使用的次数，即MFT文件记录被重复使用一次，则文件的顺序号加1，这样可以让NTFS完成一致性检查。

​	在NTFS中，一个目录是它所包含的文件的名字索引和文件的引用的组合。如果目录所有属性的总长度比MFT的一条文件记录的长度小，那么目录的所有信息将会保存在这条MFT记录当中，该记录成为基本文件记录。对于较大的目录，则是通过B+树来实现目录的存储结构。

​	当目录的属性值存放在MFT表的基本文件记录中时，该属性就是常驻属性。常驻属性的属性值存放在属性名的后面。如果一个目录的属性值太大，不能存放在基本文件记录当中，那么就会从Data区为该属性值分配空间。这些存储空间通常称为一个运行(run),用来存放属性值，存放在运行中的属性是非常驻属性。

该属性是实现NTFS的B+树索引的根节点，它总是常驻属性。该属性的结构如下图：	MFT中的基本文件记录有一个指针指向大小为2KB的运行(4KB是对应着4KB的簇尺寸)，那是一个非常驻索引缓冲区，包含着下一层的目录或文件。

### 1.2 索引缓冲区结构

​	索引缓冲区是NTFS中的B+树目录管理中的重要结构，每一个索引缓冲区在NTFS中的大小一般是4KB,其位置和大小由目录的文件记录中A0H属性的数据流(runlist)定义。





-------

**小文件的文件记录：**

​	小目录的索引根属性可以包括其中所有的文件和目录的索引，如下：

![](http://upload-images.jianshu.io/upload_images/6128001-4bfc07087da4cccd.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

**大文件的文件记录:**

​	大文件或者是大目录的所有属性，就不可能都常驻在MFT中。如果一个属性(如文件数据属性)太大而不能够放在只有1KB的MFT文件记录中，那么NTFS将从MFT之外分配区域。这些区域通常称为一个运行（run）或一个盘区（extent），它们可用来存储属性值，如文件数据。如果以后属性值又增加，那么NTFS将会再分配一个运行，以便用来存储额外的数据。值存储在运行中而不是在MFT文件记录中的属性称为非常驻属性（nonresident attribute）。NTFS决定了一个属性是常驻还是非常驻的；而属性值的位置对访问它的进程而言是透明的。

​	当一个属性为非常驻时，如大文件的数据，它的头部包含了NTFS需要在磁盘上定位该属性值的有关信息。下图显示了一个存储在两个运行中的非常驻属性。

![存储在两个运行中的非常驻属性](http://upload-images.jianshu.io/upload_images/6128001-54600b67a9e0d9a7.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

​	一个大目录也可能包括非常驻属性(或属性部分)，参见下图。在该例中，MFT文件记录没有足够空间来存储大目录的文件索引。其中，一部分索引存放在索引根属性中，而另一部分则存放在叫作“索引缓冲区”（index buffer）的非常驻运行中。这里，索引根、索引分配以及位图属性都是简化表示的，这些属性将在后面详细介绍。对目录而言，索引根的头及部分值应是常驻的。

![大目录的MFT记录](http://upload-images.jianshu.io/upload_images/6128001-3b3bd65ffc1443f4.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

​	当一个文件（或目录）的属性不能放在一个MFT文件记录中，而需要分开分配时，NTFS通过VCN-LCN之间的映射关系来记录运行（run）或盘区情况。LCN用来为整个卷中的簇按顺序从0到n进行编号，而VCN则用来对特定文件所用的簇按逻辑顺序从0到m进行编号。下图显示了一个非常驻数据属性的运行所使用的VCN与LCN编号。

![非常驻数据属性的VCN](http://upload-images.jianshu.io/upload_images/6128001-bf2ed07b7d90b446.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

当该文件含有超过2个运行时，则第三个运行从VCN8开始，数据属性头部含有前两个运行VCN的映射，这便于NTFS对磁盘文件分配的查询。为了便于NTFS快速查找，具有多个运行文件的常驻数据属性头中包含了VCN-LCN的映射关系，参见下图

![非常驻数据属性的VCN-LCN映射](http://upload-images.jianshu.io/upload_images/6128001-743558ea40982e14.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

虽然数据属性常常因太大而存储在运行中，但是其他属性也可能因MFT文件记录没有足够空间而需要存储在运行中。另外，如果一个文件有太多的属性而不能存放在MFT记录中，那么第二个MFT文件记录就可用来容纳这些额外的属性（或非常驻属性的头）。在这种情况下，一个叫作“属性列表”（attribute list）的属性就加进来。属性列表包括文件属性的名称和类型代码以及属性所在MFT的文件引用。属性列表通常用于太大或太零散的文件，这种文件因VCN-LCN映射关系太大而需要多个MFT文件记录。具有超过200个运行的文件通常需要属性列表。

