
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
	ULONG Type;
	USHORT UsaOffset;
	USHORT UsaCount;
	USN Usn; //USN == LONLONG
}NTFS_RECORD_HEADER, *PNTFS_RECORD_HEADER; // 16B


//****FILE RECORD_HEADER****
typedef struct {
	NTFS_RECORD_HEADER Ntfs;
	USHORT SequenceNumber;
	USHORT LinkCount;
	USHORT AttributeOffset;
	USHORT Flags;
	ULONG BytesInUse;
	ULONG BytesAllocated;
	ULONGLONG BaseFileRecord;
	USHORT NextAttributeNumber;
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
	ULONG Length;
	BOOLEAN Nonresident;
	UCHAR NameLength;
	USHORT NameOffset;
	USHORT Flags;	 //0x0001 = Compressed
	USHORT AttributeNumber;
}ATTRIBUTE, *PATTRIBUTE;


typedef struct {
	ATTRIBUTE Attribute;
	ULONG ValueLength; //属性值长度   
	USHORT ValueOffset; //属性值偏移   
	USHORT Flags; // 索引标志 0x0001 = Indexed   
} RESIDENT_ATTRIBUTE, *PRESIDENT_ATTRIBUTE;


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
	ULONGLONG CreationTime;
	ULONGLONG ChangeTime;
	ULONGLONG LastWriteTime;
	ULONGLONG LastAccessTime;
	ULONG FileAttributes;
	ULONG AlignmentOrReservedOrUnknown[3];
	ULONG QuotaId;                // NTFS 3.0
	ULONG SecurityId;       // NTFS 3.0
	ULONGLONG QuotaCharge;  // NTFS 3.0
	USN Usn;                      // NTFS 3.0
} STANDARD_INFORMATION, *PSTANDARD_INFORMATION;


typedef struct {
	ATTRIBUTE_TYPE AttributeType;
	USHORT Length;
	UCHAR NameLength;
	UCHAR NameOffset;
	ULONGLONG LowVcn;
	ULONGLONG FileReferenceNumber;
	USHORT AttributeNumber;
	USHORT AlignmentOrReserved[3];
} ATTRIBUTE_LIST, *PATTRIBUTE_LIST;


typedef struct { //文件名属性的值区域   
	ULONGLONG DirectoryFileReferenceNumber; //父目录的FRN   
	ULONGLONG CreationTime;
	ULONGLONG ChangeTime;
	ULONGLONG LastWriteTime; // 最后一次MFT更新时间   
	ULONGLONG LastAccessTime;
	ULONGLONG AllocatedSize; // 未明   
	ULONGLONG DataSize; // 偶尔与文件大小GetFileSize不同   
	ULONG FileAttributes;
	ULONG AlignmentOrReserved;
	UCHAR NameLength;
	UCHAR NameType; // 0x0  WIN32 0x01  DOS 0x02  WIN32&DOS 0x3   
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
/* 该结构体当中定义的是NTFS的Boot Sector结构
 * 
 */
typedef struct { //512B   
	UCHAR Jump[3];			//跳过3个字节   
	UCHAR Format[8]; 		//‘N’'T' 'F' 'S' 0x20 0x20 0x20 0x20   
	USHORT BytesPerSector;	//每扇区有多少字节 一般为512B 0x200   
	UCHAR SectorsPerCluster;//每簇有多少个扇区   
	USHORT BootSectors;		//   
	UCHAR Mbz1;				//保留0   
	USHORT Mbz2;			//保留0   
	USHORT Reserved1;		//保留0   
	UCHAR MediaType;		//介质描述符，硬盘为0xf8   
	USHORT Mbz3;			//总为0   
	USHORT SectorsPerTrack;	//每道扇区数，一般为0x3f   
	USHORT NumberOfHeads;	//磁头数   
	ULONG PartitionOffset;	//该分区的便宜（即该分区前的隐含扇区数 一般为磁道扇区数0x3f 63）   
	ULONG Reserved2[2];
	ULONGLONG TotalSectors;	//该分区总扇区数   
	ULONGLONG MftStartLcn;	//MFT表的起始簇号LCN   
	ULONGLONG Mft2StartLcn;	//MFT备份表的起始簇号LCN   
	ULONG ClustersPerFileRecord;//每个MFT记录包含几个簇  记录的字节不一定为：ClustersPerFileRecord*SectorsPerCluster*BytesPerSector  
	ULONG ClustersPerIndexBlock;//每个索引块的簇数   
	LARGE_INTEGER VolumeSerialNumber;//卷序列号   
	UCHAR Code[0x1AE];
	USHORT BootSignature;
} BOOT_BLOCK, *PBOOT_BLOCK;

#pragma pack(pop) 






#if 0
struct MY_USN_RECORD{
	DWORDLONG FileReferenceNumber;			//文件引用数
	DWORDLONG ParentFileReferenceNumber;	//父文件引用树
	LARGE_INTEGER TimeStamp;				//时间
	DWORD Reason;							//原因
	WCHAR FileName[MAX_PATH];
};


typedef struct{
	DWORD RecordLength;					//该条USN记录长度
	WORD   MajorVersion;                //主版本
	WORD   MinorVersion;                //次版本
	DWORDLONG FileReferenceNumber;		//文件引用数
	DWORDLONG ParentFileReferenceNumber;//父文件引用数
	USN Usn;							//USN（一般为int64类型）
	LARGE_INTEGER TimeStamp;            //时间戳
	DWORD Reason;						//原因
	DWORD SourceInfo;					//源信息
	DWORD SecurityId;					//安全
	DWORD FileAttributes;               //文件属性（文件或目录）
	WORD   FileNameLength;				//文件名长度
	WORD   FileNameOffset;              //文件名偏移量
	WCHAR FileName[1];					//文件名第一位的指针
} USN_RECORD, *PUSN_RECORD;


typedef struct{
	DWORDLONG UsnJournalID;				//USN日志ID
	USN FirstUsn;						//第一条USN记录的位置
	USN NextUsn;						//下一条USN记录将要写入的位置
	USN LowestValidUsn;					//最小的有效的USN（FistUSN小于该值）
	USN MaxUsn;							//USN最大值
	DWORDLONG MaximumSize;				//USN日志最大大小（按Byte算）
	DWORDLONG AllocationDelta;			//USN日志每次创建和释放的内存字节数
} USN_JOURNAL_DATA, *PUSN_JOURNAL_DATA;



typedef struct{
	DWORDLONG StartFileReferenceNumber;	//开始文件引用数，第一次调用必须为0
	USN LowUsn;							//最小USN，第一次调用，最好为0
	USN HighUsn;						//最大USN
} MFT_ENUM_DATA, *PMFT_ENUM_DATA;


typedef struct{
	USN StartUsn;						//变更的USN记录开始位置，即第一次读取USN日志的LastUsn值。
	DWORD ReasonMask;					//原因标识
	DWORD ReturnOnlyOnClose;			//只有在记录关闭时才返回
	DWORDLONG Timeout;					//延迟时间
	DWORDLONG BytesToWaitFor;			//当USN日志大小大于该值时返回
	DWORDLONG UsnJournalID;				//USN日志ID
} READ_USN_JOURNAL_DATA, *PREAD_USN_JOURNAL_DATA;


typedef struct{
	DWORDLONG MaximumSize;				//NTFS文件系统分配给USN日志的最大大小（字节）
	DWORDLONG AllocationDelta;			//USN日志每次创建和释放的内存字节数
} CREATE_USN_JOURNAL_DATA, *PCREATE_USN_JOURNAL_DATA;


typedef struct{
	DWORDLONG UsnJournalID;				//USN日志ID
	DWORD DeleteFlags;					//删除标志
} DELETE_USN_JOURNAL_DATA, *PDELETE_USN_JOURNAL_DATA;
#endif // if 0



#endif	// NTFS_H_INCLUDED