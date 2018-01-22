
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
	USHORT UsaOffset;	// �������к�����ƫ�ƣ�У��ֵ��ַ
	USHORT UsaCount;	// 1+n 1ΪУ��ֵ���� nΪ���滻ֵ�ĸ��� fixup
	USN Usn; 			//USN == LONLONG 8B ÿһ�μ�¼���޸ģ�USN����仯
}NTFS_RECORD_HEADER, *PNTFS_RECORD_HEADER; // 16B


//****FILE RECORD_HEADER****
typedef struct {
	NTFS_RECORD_HEADER Ntfs;   	// Ntfs.type ����'ELIF'
	USHORT SequenceNumber;		// FileReferenceNumber�ĸ�16λ  i�ǵ�48λ 0~total_file_count-1  
	// ���ڼ�¼���ļ����¼���ظ�ʹ�õĴ��� 
	USHORT LinkCount;			// ��¼Ӳ���ӵ���Ŀ��ֻ�����ڻ����ļ���¼��
	USHORT AttributeOffset;		// ��һ�����Ե�ƫ�� 
	USHORT Flags;				// 0x0001 = InUse, 0x0002 = Directory
	ULONG BytesInUse;			// ��¼ͷ�����Ե��ܳ��ȣ����ļ���¼��ʵ�ʳ����ļ�,����¼�ڴ�����ʵ��ռ�õ��ֽڿռ䡣
	ULONG BytesAllocated;		// �ܹ��������¼�ĳ���
	ULONGLONG BaseFileRecord;	// �����ļ���¼�е��ļ�������
	//���ڻ����ļ���¼����ֵΪ0�������Ϊ0������һ�����ļ�����ļ������ţ�ָ�������Ļ����ļ���¼�е��ļ���¼�ţ�
	//�ڻ����ļ���¼�а�������չ�ļ���¼����Ϣ���洢�ڡ������б�ATTRIBUTE_LIST�������С�
	USHORT NextAttributeNumber;	// ��һ����ID	
}FILE_RECORD_HEADER, *PFILE_RECORD_HEADER; // 42B


//�ļ���¼�а����Էǽ�����Ϊ��ʱ���������ͬ�����ԣ����У�   
//���Ե�ֵ��Ϊ �ֽ���   
typedef enum {
	//����ֻ�����浵���ļ����ԣ�   
	//ʱ������ļ�����ʱ�����һ���޸�ʱ��   
	//����Ŀ¼ָ����ļ���Ӳ���Ӽ���hard link count��   
	AttributeStandardInformation = 0x10, //Resident_Attributes ��פ����  
	//��һ���ļ�Ҫ����MFT�ļ���¼ʱ ���и�����   
	//�����б��������ɸ��ļ�����Щ���ԣ��Լ�ÿ���������ڵ�MFT�ļ���¼���ļ�����   
	AttributeAttributeList = 0x20,//��������ֵ���ܻ������������Ƿ�פ������  

	//�ļ������Կ����ж����   
	//1.���ļ����Զ�Ϊ����ļ���(�Ա�MS-DOS��16λ�������)   
	//2.�����ļ�����Ӳ����ʱ   
	AttributeFileName = 0x30, //��פ  

	//һ���ļ���Ŀ¼��64�ֽڱ�ʶ�������е�16�ֽڶ��ڸþ���˵��Ψһ��   
	//����-���ٷ��񽫶���ID�������ǿ�ݷ�ʽ��OLE����Դ�ļ���   
	//NTFS�ṩ����Ӧ��API����Ϊ�ļ���Ŀ¼����ͨ�������ID��������ͨ�����ļ�����   
	AttributeObjectId = 0x40, //��פ  
																														   //Ϊ��NTFS��ǰ�汾����������   
	//���о�����ͬ��ȫ���������ļ���Ŀ¼����ͬ���İ�ȫ����   
	//��ǰ�汾��NTFS��˽�еİ�ȫ��������Ϣ��ÿ���ļ���Ŀ¼�洢��һ��   
	AttributeSecurityDescriptor = 0x50,//������$SecureԪ�����ļ���  

	//�����˸þ�İ汾��label��Ϣ   
	AttributeVolumeName = 0x60, //��������$VolumeԪ�����ļ���   
	AttributeVolumeInformation = 0x70,//��������$VolumeԪ�����ļ���  

	//�ļ����ݣ�һ���ļ�����һ��δ�������������ԣ������ж�����������������   
	//��һ���ļ������ж����������Ŀ¼û��Ĭ�ϵ��������ԣ������ж����ѡ����������������   
	AttributeData = 0x80,//��������ֵ���ܻ������������Ƿ�פ������  

	//������������ʵ�ִ�Ŀ¼���ļ��������λͼ����   
	AttributeIndexRoot = 0x90,//��פ   
	AttributeIndexAllocation = 0xA0,
	AttributeBitmap = 0xB0,

	//�洢��һ���ļ����ؽ��������ݣ�NTFS�Ľ���(junction)�͹��ص����������   
	AttributeReparsePoint = 0xC0,

	//��������Ϊ��չ���ԣ����Ѳ��ٱ�����ʹ�ã�֮�����ṩ��Ϊ��OS/2���򱣳�������   
	AttributeEAInformation = 0xD0,
	AttributeEA = 0xE0,

	AttributePropertySet = 0xF0,
	AttributeLoggedUtilityStream = 0x100,
	AttributeEnd = 0xFFFFFFFF
} ATTRIBUTE_TYPE, *PATTRIBUTE_TYPE;


typedef struct {
	ATTRIBUTE_TYPE AttributeType;
	ULONG AttributeLength; 			//�����Գ��ȣ���������ֵ��
	BOOLEAN Nonresident;			//�������ǲ���פ������ 0x00=Resident, 0x01 = NonResident
	UCHAR NameLength;				//�����������Ƴ���
	USHORT NameOffset;				//������ƫ��
	USHORT Flags;	 				// 0x00FF=compressed  0x4000=encrypted  0x8000=Sparse
	USHORT AttributeID;
}ATTRIBUTE, *PATTRIBUTE;


typedef struct {
	ATTRIBUTE Attribute;
	ULONG ContentLength; 		//����ֵ����   
	USHORT ContentOffset; 	//����ֵƫ��   
//	USHORT Flags; 			// ������־ 0x0001 = Indexed �ͽṹͼ��������һ�£��ṹͼ��Ϊunused  
	USHORT unused;
} RESIDENT_ATTRIBUTE, *PRESIDENT_ATTRIBUTE;



// ����ṹ�Ķ����ͼƬ��ṹ��������һ��
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
	ATTRIBUTE_TYPE AttributeType; 			//��������
	USHORT Length;							//����¼����
	UCHAR NameLength;						//����������
	UCHAR NameOffset;						//������ƫ��
	ULONGLONG LowVcn;						//��ʼVCN
	ULONGLONG FileReferenceNumber;			//���Ե��ļ��ο���
	USHORT AttributeNumber;					//��ʶ
	USHORT AlignmentOrReserved[3];
} ATTRIBUTE_LIST, *PATTRIBUTE_LIST;


typedef struct { //�ļ������Ե�ֵ����   
	ULONGLONG DirectoryFileReferenceNumber; 	// Parent Directory ��Ŀ¼��FRN   
	ULONGLONG CreateTime;						// Date Created
	ULONGLONG ModifiedTime;						// Date Modified
	ULONGLONG LastWriteTime; 					// Date MFT Mofified���һ��MFT����ʱ��   
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
	UCHAR Jump[3];			//����3���ֽ�   
	UCHAR Format[8]; 		//��N��'T' 'F' 'S' 0x20 0x20 0x20 0x20   
	USHORT BytesPerSector;	//ÿ�����ж����ֽ� һ��Ϊ512B 0x200   
	UCHAR SectorsPerCluster;//ÿ���ж��ٸ�����
	USHORT res; 			//����
	UCHAR Mbz1;				//����0
	USHORT Mbz2;			//����0   
	USHORT Reserved1;		//����0   
	UCHAR MediaType;		//������������Ӳ��Ϊ0xf8   
	USHORT Mbz3;			//��Ϊ0   
	USHORT SectorsPerTrack;	//ÿ����������һ��Ϊ0x3f   
	USHORT NumberOfHeads;	//��ͷ��   
	ULONG PartitionOffset;	//�÷�����ƫ�ƣ����÷���ǰ������������ һ��Ϊ�ŵ�������0x3f 63��   
	ULONG Reserved2[2];
	ULONGLONG TotalSectors;	//�÷�����������   
	ULONGLONG MftStartLcn;	//MFT�����ʼ�غ�LCN   
	ULONGLONG Mft2StartLcn;	//MFT���ݱ����ʼ�غ�LCN   
	ULONG ClustersPerFileRecord;//ÿ��MFT��¼����������  
	//��¼���ֽڲ�һ��Ϊ��ClustersPerFileRecord*SectorsPerCluster*BytesPerSector  
	ULONG ClustersPerIndexBlock;//ÿ��������Ĵ���   
	LARGE_INTEGER VolumeSerialNumber;//�����к�   
	UCHAR Code[0x1AE];
	USHORT BootSignature;
} BOOT_BLOCK, *PBOOT_BLOCK;

#pragma pack(pop) 








/* 
// ��ͼƬ�����ж���Ĳ�һ��
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

// ��ͼƬ�е�������������
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