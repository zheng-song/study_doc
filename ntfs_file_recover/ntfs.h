
#ifndef NTFS_H_INCLUDED
#define NTFS_H_INCLUDED

#define _WIN32_WINNT 0x0400
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <winioctl.h>
#define BUF_LEN 4096
#define FSCTL_CREATE_USN_JOURNAL CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 57, METHOD_NEITHER, FILE_ANY_ACCESS) // CREATE_USN_JOURNAL_DATA,
#define FSCTL_QUERY_USN_JOURNAL CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 61, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_ENUM_USN_DATA CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 44, METHOD_NEITHER, FILE_ANY_ACCESS)
#define FSCTL_DELETE_USN_JOURNAL  CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 62, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_READ_USN_JOURNAL CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 46, METHOD_NEITHER, FILE_ANY_ACCESS)
#define FSCTL_READ_FILE_USN_DATA CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 58, METHOD_NEITHER, FILE_ANY_ACCESS)
#define FSCTL_QUERY_USN_JOURNAL CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 61, METHOD_BUFFERED, FILE_ANY_ACCESS)


// **NTFS RECORD HEADER**
typedef struct {
	ULONG Type;  		// 4B 'ELIF','XDNI','DAAB','ELOH','DKHC'
	USHORT UsaOffset;	// 更新序列号数组偏移，校验值地址
	USHORT UsaCount;	// 1+n 1为校验值个数 n为待替换值的个数 fixup
	USN Usn; 			//USN == LONLONG 8B 每一次记录被修改，USN都会变化
}NTFS_RECORD_HEADER, *PNTFS_RECORD_HEADER; // 16B


//****FILE RECORD_HEADER****
typedef struct {
	NTFS_RECORD_HEADER Ntfs;   	// Ntfs.type 总是'ELIF'
	USHORT SequenceNumber;		// FileReferenceNumber的高16位  i是低48位 0~total_file_count-1  
	// 用于记录主文件表记录被重复使用的次数 
	USHORT LinkCount;			// 记录硬连接的数目，只出现在基本文件记录中
	USHORT AttributeOffset;		// 第一个属性的偏移 
	USHORT Flags;				// 0x0001 = InUse, 0x0002 = Directory
	ULONG BytesInUse;			// 记录头和属性的总长度，即文件记录的实际长度文件,即记录在磁盘上实际占用的字节空间。
	ULONG BytesAllocated;		// 总共分配给记录的长度
	ULONGLONG BaseFileRecord;	// 基本文件记录中的文件索引号
	//对于基本文件记录，其值为0，如果不为0，则是一个主文件表的文件索引号，指向所属的基本文件记录中的文件记录号，
	//在基本文件记录中包含有扩展文件记录的信息，存储在“属性列表ATTRIBUTE_LIST”属性中。
	USHORT NextAttributeNumber;	// 下一属性ID	
}FILE_RECORD_HEADER, *PFILE_RECORD_HEADER; // 42B


//文件记录中按属性非降序（因为有时连续多个相同的属性）排列，   
//属性的值即为 字节流   
typedef enum {
	//诸如只读、存档等文件属性；   
	//时间戳：文件创建时、最后一次修改时；   
	//多少目录指向该文件（硬链接计数hard link count）   
	AttributeStandardInformation = 0x10, //Resident_Attributes 常驻属性  
	//当一个文件要求多个MFT文件记录时 会有该属性   
	//属性列表，包括构成该文件的这些属性，以及每个属性所在的MFT文件记录的文件引用   
	AttributeAttributeList = 0x20,//由于属性值可能会增长，可能是非驻留属性  

	//文件名属性可以有多个：   
	//1.长文件名自动为其短文件名(以便MS-DOS和16位程序访问)   
	//2.当该文件存在硬链接时   
	AttributeFileName = 0x30, //常驻  

	//一个文件或目录的64字节标识符，其中低16字节对于该卷来说是唯一的   
	//链接-跟踪服务将对象ID分配给外壳快捷方式和OLE链接源文件。   
	//NTFS提供了相应的API，因为文件和目录可以通过其对象ID，而不是通过其文件名打开   
	AttributeObjectId = 0x40, //常驻  
																														   //为与NTFS以前版本保持向后兼容   
	//所有具有相同安全描述符的文件或目录共享同样的安全描述   
	//以前版本的NTFS将私有的安全描述符信息与每个文件和目录存储在一起   
	AttributeSecurityDescriptor = 0x50,//出现于$Secure元数据文件中  

	//保存了该卷的版本和label信息   
	AttributeVolumeName = 0x60, //仅出现于$Volume元数据文件中   
	AttributeVolumeInformation = 0x70,//仅出现于$Volume元数据文件中  

	//文件内容，一个文件仅有一个未命名的数据属性，但可有额外多个命名数据属性   
	//即一个文件可以有多个数据流，目录没有默认的数据属性，但可有多个可选的命名的数据属性   
	AttributeData = 0x80,//由于属性值可能会增长，可能是非驻留属性  

	//以下三个用于实现大目录的文件名分配和位图索引   
	AttributeIndexRoot = 0x90,//常驻   
	AttributeIndexAllocation = 0xA0,
	AttributeBitmap = 0xB0,

	//存储了一个文件的重解析点数据，NTFS的交接(junction)和挂载点包含此属性   
	AttributeReparsePoint = 0xC0,

	//以下两个为扩展属性，现已不再被主动使用，之所以提供是为与OS/2程序保持向后兼容   
	AttributeEAInformation = 0xD0,
	AttributeEA = 0xE0,

	AttributePropertySet = 0xF0,
	AttributeLoggedUtilityStream = 0x100,
	AttributeEnd = 0xFFFFFFFF
} ATTRIBUTE_TYPE, *PATTRIBUTE_TYPE;


typedef struct {
	ATTRIBUTE_TYPE AttributeType;
	ULONG AttributeLength; 			//本属性长度（包含属性值）
	BOOLEAN Nonresident;			//本属性是不是驻留属性 0x00=Resident, 0x01 = NonResident
	UCHAR NameLength;				//属性名的名称长度
	USHORT NameOffset;				//属性名偏移
	USHORT Flags;	 				// 0x00FF=compressed  0x4000=encrypted  0x8000=Sparse
	USHORT AttributeID;
}ATTRIBUTE, *PATTRIBUTE;


typedef struct {
	ATTRIBUTE Attribute;
	ULONG ContentLength; 		//属性值长度   
	USHORT ContentOffset; 	//属性值偏移   
//	USHORT Flags; 			// 索引标志 0x0001 = Indexed 和结构图里描述不一致，结构图中为unused  
	USHORT unused;
} RESIDENT_ATTRIBUTE, *PRESIDENT_ATTRIBUTE;



// 这个结构的定义和图片里结构的描述不一致
// TODO
typedef struct {
	ATTRIBUTE Attribute;
	ULONGLONG LowVcn;
	ULONGLONG HighVcn;
	USHORT RunArrayOffset;
	UCHAR CompressionUnit;
	UCHAR AlignmentOrReserved[5];
	ULONGLONG AllocatedSize;
	ULONGLONG DataSize;
	ULONGLONG InitializedSize;
	ULONGLONG CompressedSize;    // Only when compressed   
} NONRESIDENT_ATTRIBUTE, *PNONRESIDENT_ATTRIBUTE;



typedef struct {
	ATTRIBUTE_TYPE AttributeType; 			//属性类型
	USHORT Length;							//本记录长度
	UCHAR NameLength;						//属性名长度
	UCHAR NameOffset;						//属性名偏移
	ULONGLONG LowVcn;						//起始VCN
	ULONGLONG FileReferenceNumber;			//属性的文件参考号
	USHORT AttributeNumber;					//标识
	USHORT AlignmentOrReserved[3];
} ATTRIBUTE_LIST, *PATTRIBUTE_LIST;


typedef struct { //文件名属性的值区域   
	ULONGLONG DirectoryFileReferenceNumber; 	// Parent Directory 父目录的FRN   
	ULONGLONG CreateTime;						// Date Created
	ULONGLONG ModifiedTime;						// Date Modified
	ULONGLONG LastWriteTime; 					// Date MFT Mofified最后一次MFT更新时间   
	ULONGLONG LastAccessTime;					// Date Accessed
	ULONGLONG LogicalFileSize; 					// Logical File Size   
	ULONGLONG SizeOnDisk; 						//    
	ULONG Flags;
	ULONG ReparseValue;
	UCHAR NameLength;
	UCHAR NameType; 							// 0x0  WIN32 0x01  DOS 0x02  WIN32&DOS 0x3   
	WCHAR Name[1];
} FILENAME_ATTRIBUTE, *PFILENAME_ATTRIBUTE;


typedef struct {
	GUID ObjectId;
	union {
		struct {
			GUID BirthVolumeId;
			GUID BirthObjectId;
			GUID DomainId;
		};
		UCHAR ExtendedInfo[48];
	};
} OBJECTID_ATTRIBUTE, *POBJECTID_ATTRIBUTE;


typedef struct {
	ULONG Unknown[2];
	UCHAR MajorVersion;
	UCHAR MinorVersion;
	USHORT Flags;
} VOLUME_INFORMATION, *PVOLUME_INFORMATION;


typedef struct {
	ULONG EntriesOffset;
	ULONG IndexBlockLength;
	ULONG AllocatedSize;
	ULONG Flags; // 0x00 = Small directory, 0x01 = Large directory
} DIRECTORY_INDEX, *PDIRECTORY_INDEX;


typedef struct {
	ULONGLONG FileReferenceNumber;
	USHORT Length;
	USHORT AttributeLength;
	ULONG Flags; // 0x01 = Has trailing VCN, 0x02 = Last entry
				 // FILENAME_ATTRIBUTE Name;
				 // ULONGLONG Vcn; // VCN in IndexAllocation of earlier entries
} DIRECTORY_ENTRY, *PDIRECTORY_ENTRY;


typedef struct {
	ATTRIBUTE_TYPE Type;
	ULONG CollationRule;
	ULONG BytesPerIndexBlock;
	ULONG ClustersPerIndexBlock;
	DIRECTORY_INDEX DirectoryIndex;
} INDEX_ROOT, *PINDEX_ROOT;


#pragma pack(push,1)   
typedef struct { //512B   
	UCHAR Jump[3];			//跳过3个字节   
	UCHAR Format[8]; 		//‘N’'T' 'F' 'S' 0x20 0x20 0x20 0x20   
	USHORT BytesPerSector;	//每扇区有多少字节 一般为512B 0x200   
	UCHAR SectorsPerCluster;//每簇有多少个扇区
	USHORT res; 			//保留
	UCHAR Mbz1;				//保留0
	USHORT Mbz2;			//保留0   
	USHORT Reserved1;		//保留0   
	UCHAR MediaType;		//介质描述符，硬盘为0xf8   
	USHORT Mbz3;			//总为0   
	USHORT SectorsPerTrack;	//每道扇区数，一般为0x3f   
	USHORT NumberOfHeads;	//磁头数   
	ULONG PartitionOffset;	//该分区的偏移（即该分区前的隐含扇区数 一般为磁道扇区数0x3f 63）   
	ULONG Reserved2[2];
	ULONGLONG TotalSectors;	//该分区总扇区数   
	ULONGLONG MftStartLcn;	//MFT表的起始簇号LCN   
	ULONGLONG Mft2StartLcn;	//MFT备份表的起始簇号LCN   
	ULONG ClustersPerFileRecord;//每个MFT记录包含几个簇  
	//记录的字节不一定为：ClustersPerFileRecord*SectorsPerCluster*BytesPerSector  
	ULONG ClustersPerIndexBlock;//每个索引块的簇数   
	LARGE_INTEGER VolumeSerialNumber;//卷序列号   
	UCHAR Code[0x1AE];
	USHORT BootSignature;
} BOOT_BLOCK, *PBOOT_BLOCK;

#pragma pack(pop) 








/* 
// 和图片描述中定义的不一样
typedef struct {
	ULONGLONG CreationTime;		// Date created
	ULONGLONG ModifiedTime;		// Date Modified
	ULONGLONG LastWriteTime;	// Date MFT record modified
	ULONGLONG LastAccessTime;	// Date Accessed
	ULONG FileAttributes;
	ULONG AlignmentOrReservedOrUnknown[3];
	ULONG QuotaId;                // NTFS 3.0
	ULONG SecurityId;       // NTFS 3.0
	ULONGLONG QuotaCharge;  // NTFS 3.0
	USN Usn;                      // NTFS 3.0
} STANDARD_INFORMATION, *PSTANDARD_INFORMATION;

// 按图片中的描述定义如下
typedef enum{
	ReadOnly = 0x0001,
	Hidden 	= 0x0002,
	System 	= 0x0004,
	res1   	= 0x0008,
	res2 	= 0x0010,
	Archive	= 0x0020,
	Device 	= 0x0040,
	Normal 	= 0x0080,
	Temporary = 0x0100,
	SparseFile = 0x0200,
	ReparsePoint = 0x0400,
	Compressed 	= 0x0800,
	Offline 	= 0x1000,
	NotIndex = 0x2000,
	Encrypted = 0x4000,
	res3 = 0x8000
}FileNameANDStdInfoTYPE



typedef struct {
	ULONGLONG CreationTime;		// Date created
	ULONGLONG ModifiedTime;		// Date Modified
	ULONGLONG LastWriteTime;	// Date MFT record modified
	ULONGLONG LastAccessTime;	// Date Accessed
	ULONG Flags;
	ULONG MaxVersions;
	ULONG VersionNum;
	ULONG ClassID;
	ULONG OwerID;
	ULONG SecurityId;       // NTFS 3.0
	ULONGLONG QuotaCharge;  // NTFS 3.0
	USN Usn;                // NTFS 3.0  Update Sequence Number(USN)
	ULONGLONG Unused;
} STANDARD_INFORMATION, *PSTANDARD_INFORMATION;


*/
	
#endif	// NTFS_H_INCLUDED