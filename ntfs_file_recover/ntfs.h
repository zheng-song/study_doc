
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
	ULONG Length;
	BOOLEAN Nonresident;
	UCHAR NameLength;
	USHORT NameOffset;
	USHORT Flags;	 //0x0001 = Compressed
	USHORT AttributeNumber;
}ATTRIBUTE, *PATTRIBUTE;


typedef struct {
	ATTRIBUTE Attribute;
	ULONG ValueLength; //����ֵ����   
	USHORT ValueOffset; //����ֵƫ��   
	USHORT Flags; // ������־ 0x0001 = Indexed   
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


typedef struct { //�ļ������Ե�ֵ����   
	ULONGLONG DirectoryFileReferenceNumber; //��Ŀ¼��FRN   
	ULONGLONG CreationTime;
	ULONGLONG ChangeTime;
	ULONGLONG LastWriteTime; // ���һ��MFT����ʱ��   
	ULONGLONG LastAccessTime;
	ULONGLONG AllocatedSize; // δ��   
	ULONGLONG DataSize; // ż�����ļ���СGetFileSize��ͬ   
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
/* �ýṹ�嵱�ж������NTFS��Boot Sector�ṹ
 * 
 */
typedef struct { //512B   
	UCHAR Jump[3];			//����3���ֽ�   
	UCHAR Format[8]; 		//��N��'T' 'F' 'S' 0x20 0x20 0x20 0x20   
	USHORT BytesPerSector;	//ÿ�����ж����ֽ� һ��Ϊ512B 0x200   
	UCHAR SectorsPerCluster;//ÿ���ж��ٸ�����   
	USHORT BootSectors;		//   
	UCHAR Mbz1;				//����0   
	USHORT Mbz2;			//����0   
	USHORT Reserved1;		//����0   
	UCHAR MediaType;		//������������Ӳ��Ϊ0xf8   
	USHORT Mbz3;			//��Ϊ0   
	USHORT SectorsPerTrack;	//ÿ����������һ��Ϊ0x3f   
	USHORT NumberOfHeads;	//��ͷ��   
	ULONG PartitionOffset;	//�÷����ı��ˣ����÷���ǰ������������ һ��Ϊ�ŵ�������0x3f 63��   
	ULONG Reserved2[2];
	ULONGLONG TotalSectors;	//�÷�����������   
	ULONGLONG MftStartLcn;	//MFT�����ʼ�غ�LCN   
	ULONGLONG Mft2StartLcn;	//MFT���ݱ����ʼ�غ�LCN   
	ULONG ClustersPerFileRecord;//ÿ��MFT��¼����������  ��¼���ֽڲ�һ��Ϊ��ClustersPerFileRecord*SectorsPerCluster*BytesPerSector  
	ULONG ClustersPerIndexBlock;//ÿ��������Ĵ���   
	LARGE_INTEGER VolumeSerialNumber;//�����к�   
	UCHAR Code[0x1AE];
	USHORT BootSignature;
} BOOT_BLOCK, *PBOOT_BLOCK;

#pragma pack(pop) 






#if 0
struct MY_USN_RECORD{
	DWORDLONG FileReferenceNumber;			//�ļ�������
	DWORDLONG ParentFileReferenceNumber;	//���ļ�������
	LARGE_INTEGER TimeStamp;				//ʱ��
	DWORD Reason;							//ԭ��
	WCHAR FileName[MAX_PATH];
};


typedef struct{
	DWORD RecordLength;					//����USN��¼����
	WORD   MajorVersion;                //���汾
	WORD   MinorVersion;                //�ΰ汾
	DWORDLONG FileReferenceNumber;		//�ļ�������
	DWORDLONG ParentFileReferenceNumber;//���ļ�������
	USN Usn;							//USN��һ��Ϊint64���ͣ�
	LARGE_INTEGER TimeStamp;            //ʱ���
	DWORD Reason;						//ԭ��
	DWORD SourceInfo;					//Դ��Ϣ
	DWORD SecurityId;					//��ȫ
	DWORD FileAttributes;               //�ļ����ԣ��ļ���Ŀ¼��
	WORD   FileNameLength;				//�ļ�������
	WORD   FileNameOffset;              //�ļ���ƫ����
	WCHAR FileName[1];					//�ļ�����һλ��ָ��
} USN_RECORD, *PUSN_RECORD;


typedef struct{
	DWORDLONG UsnJournalID;				//USN��־ID
	USN FirstUsn;						//��һ��USN��¼��λ��
	USN NextUsn;						//��һ��USN��¼��Ҫд���λ��
	USN LowestValidUsn;					//��С����Ч��USN��FistUSNС�ڸ�ֵ��
	USN MaxUsn;							//USN���ֵ
	DWORDLONG MaximumSize;				//USN��־����С����Byte�㣩
	DWORDLONG AllocationDelta;			//USN��־ÿ�δ������ͷŵ��ڴ��ֽ���
} USN_JOURNAL_DATA, *PUSN_JOURNAL_DATA;



typedef struct{
	DWORDLONG StartFileReferenceNumber;	//��ʼ�ļ�����������һ�ε��ñ���Ϊ0
	USN LowUsn;							//��СUSN����һ�ε��ã����Ϊ0
	USN HighUsn;						//���USN
} MFT_ENUM_DATA, *PMFT_ENUM_DATA;


typedef struct{
	USN StartUsn;						//�����USN��¼��ʼλ�ã�����һ�ζ�ȡUSN��־��LastUsnֵ��
	DWORD ReasonMask;					//ԭ���ʶ
	DWORD ReturnOnlyOnClose;			//ֻ���ڼ�¼�ر�ʱ�ŷ���
	DWORDLONG Timeout;					//�ӳ�ʱ��
	DWORDLONG BytesToWaitFor;			//��USN��־��С���ڸ�ֵʱ����
	DWORDLONG UsnJournalID;				//USN��־ID
} READ_USN_JOURNAL_DATA, *PREAD_USN_JOURNAL_DATA;


typedef struct{
	DWORDLONG MaximumSize;				//NTFS�ļ�ϵͳ�����USN��־������С���ֽڣ�
	DWORDLONG AllocationDelta;			//USN��־ÿ�δ������ͷŵ��ڴ��ֽ���
} CREATE_USN_JOURNAL_DATA, *PCREATE_USN_JOURNAL_DATA;


typedef struct{
	DWORDLONG UsnJournalID;				//USN��־ID
	DWORD DeleteFlags;					//ɾ����־
} DELETE_USN_JOURNAL_DATA, *PDELETE_USN_JOURNAL_DATA;
#endif // if 0



#endif	// NTFS_H_INCLUDED