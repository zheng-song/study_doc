#pragma once

///ntfs.h

/* NTFS RECORD HEADER */
typedef struct {             // 16B
	ULONG Type;              // NTFS��¼�����ͣ�FILE/BAAD/INDX/CHKD/HOLE
	USHORT UsaOffset;        // Update Sequence Array �ڸýṹ�е�ƫ����
	USHORT UsaCount;         // usa ��ֵ�ĸ���
	USN Usn;                 // The Update Sequence Number of the NTFS record
} NTFS_RECORD_HEADER, *PNTFS_RECORD_HEADER;

/* FILE RECORD_HEADER */
typedef struct {                   // 42B
	NTFS_RECORD_HEADER Ntfs;       // when Type = FILE
	USHORT SequenceNumber;         // MFT entry �����ô��� 
	USHORT LinkCount;              // The number of directory links to the MFT entry
	USHORT AttributesOffset;       // ��һ�������ڴ� MFT entry �е�ƫ��
	USHORT Flags;                  // 0x0001 = The MFT entry is InUse, 0x0002 = The MFT entry represents a Directory
	ULONG BytesInUse;              // �ѱ��� MFT entry ʹ�õ��ֽ���
	ULONG BytesAllocated;          // ������� MFT entry ���ֽ���
	ULONGLONG BaseFileRecord; /*If the MFT entry contains attributes that overflowed a base MFT entry,
							  this member contains the file reference number of the base entry; otherwise , it contains zero.*/
	USHORT NextAttributeNumber;    // The number that will be assigned to the next attribute added to the MFT entry
} FILE_RECORD_HEADER, *PFILE_RECORD_HEADER;

/* �������� */
typedef enum {
	AttributeStandardInformation = 0x10, // always resident
	AttributeAttributeList = 0x20,       //always nonresident
	AttributeFileName = 0x30,            // always resident

	/*һ���ļ���Ŀ¼��64�ֽڱ�ʶ�������е�16�ֽڶ��ڸþ���˵��Ψһ�ģ�
	NTFS�ṩ����Ӧ��API����Ϊ�ļ���Ŀ¼����ͨ�������ID��������ͨ�����ļ�����*/ 
	AttributeObjectId = 0x40,            // always resident

	AttributeSecurityDescriptor = 0x50,

	/*�����˸þ�İ汾��label��Ϣ*/
	AttributeVolumeName = 0x60,
	AttributeVolumeInformation = 0x70,

	AttributeData = 0x80,

	/*����ʵ�ִ�Ŀ¼���ļ��������λͼ����*/
	AttributeIndexRoot = 0x90,
	AttributeIndexAllocation = 0xA0,
	AttributeBitmap = 0xB0,

	AttributeReparsePoint = 0xC0,

	/*��������Ϊ��չ���ԣ����Ѳ��ٱ�����ʹ�ã�֮�����ṩ��Ϊ��OS/2���򱣳�������*/
	AttributeEAInformation = 0xD0,
	AttributeEA = 0xE0,

	AttributePropertySet = 0xF0,
	AttributeLoggedUtilityStream = 0x100
} ATTRIBUTE_TYPE, *PATTRIBUTE_TYPE;

/* ���� */
typedef struct {
	ATTRIBUTE_TYPE AttributeType;
	ULONG Length;           // �����Գ�פ���ֵ��ֽڴ�С
	BOOLEAN Nonresident;    // true = �ǳ�פ����
	UCHAR NameLength;       // ����������������������ַ���С
	USHORT NameOffset;      // ���������ڲ�ƫ�ƣ��ֽڣ�������ΪUnicode
	USHORT Flags;           // 0x0001 = ���Ա�ѹ��
	USHORT AttributeNumber; // ������ʵ�������ֱ�ʶ��
} ATTRIBUTE, *PATTRIBUTE;

/* ��פ���� */
typedef struct {
	ATTRIBUTE Attribute;
	ULONG ValueLength;      // ����ֵ���ֽڴ�С
	USHORT ValueOffset;     // ����ֵ���ڲ�ƫ�ƣ��ֽ�
	USHORT Flags;           // 0x0001 = The attribute is Indexed
} RESIDENT_ATTRIBUTE, *PRESIDENT_ATTRIBUTE;

/* �ǳ�פ���� */
typedef struct {
	ATTRIBUTE Attribute;
	ULONGLONG LowVcn;
	ULONGLONG HighVcn;
	USHORT RunArrayOffset;            // run array ���ڲ�ƫ������run array �洢 VCN �� LCN ֮��Ķ�Ӧ��ϵ
	UCHAR CompressionUnit;            // ���ó�ԱΪ0��������δ��ѹ��
	UCHAR AlignmentOrReserved[5];
	ULONGLONG AllocatedSize;          // �����˶��ٴ��̿ռ��������ɸ�����ֵ���ֽ�
	ULONGLONG DataSize;               // ����ֵ���ֽڴ�С��������ֵ��ѹ�����ϡ�裬���ܻ�� AllocatedSize ��
	ULONGLONG InitializedSize;        // ����ֵ�ĳ�ʼ�ֽڴ�С
	ULONGLONG CompressedSize;         // ����ֵ��ѹ������ֽڴ�С��ֻ�е����Ա�ѹ��ʱ�����иó�Ա
} NONRESIDENT_ATTRIBUTE, *PNONRESIDENT_ATTRIBUTE;

/* �ļ���׼��Ϣ���� */
typedef struct {
	ULONGLONG CreationTime;
	ULONGLONG ChangeTime;
	ULONGLONG LastWriteTime;
	ULONGLONG LastAccessTime;
	ULONG FileAttributes;              // �ļ������ԣ�����ֻ�������ء��鵵��ϵͳ�����ܵ�
	ULONG AlignmentOrReservedOrUnknown[3];
	ULONG QuotaId;                     // NTFS 3.0
	ULONG SecurityId;                  // NTFS 3.0
	ULONGLONG QuotaCharge;             // NTFS 3.0
	USN Usn;                           // NTFS 3.0
} STANDARD_INFORMATION, *PSTANDARD_INFORMATION;

/* �ļ������б����� */
/** 
*������MFT��¼װ�����ļ�������ʱ�����ļ��Ż���attribute list���ԣ�װ���µĿ���ԭ��
* 1. The file has a large numbers of alternate names (�ļ��д���Ӳ����)
* 2. The attribute value is large, and the volume is badly fragmented������ֵ̫�󣬾����ط�ɢ��
* 3. The file has a complex security descriptor (�ļ��İ�ȫ������̫���ӣ��������NTFS 3.0ûӰ��)
* 4. The file has many streams���ļ��кܶ�����
**/
typedef struct {
	ATTRIBUTE_TYPE AttributeType;
	USHORT Length;                      // �����б���Ŀ���ֽڴ�С
	UCHAR NameLength;                   // ����������������������ַ���С
	UCHAR NameOffset;                   // ���������ڲ�ƫ�ƣ��ֽڣ�������ΪUnicode
	ULONGLONG LowVcn;
	ULONGLONG FileReferenceNumber; 
	USHORT AttributeNumber;             // ������ʵ�������ֱ�ʶ��
	USHORT AlignmentOrReserved[3];
} ATTRIBUTE_LIST, *PATTRIBUTE_LIST;

/* �ļ������� */
typedef struct {
	ULONGLONG DirectoryFileReferenceNumber; // ���ڵ��FRN
	ULONGLONG CreationTime;				    // �ļ�����ʱ�䣬�ó�Աֻ�ڸ����ļ���ʱ����
	ULONGLONG ChangeTime;                   // �ļ��������һ�α��޸ĵ�ʱ�䣬�ó�Աֻ�ڸ����ļ���ʱ����
	ULONGLONG LastWriteTime;                // �ļ����һ�α�д��ʱ�䣬�ó�Աֻ�ڸ����ļ���ʱ����
	ULONGLONG LastAccessTime;               // �ļ����һ�α����ʵ�ʱ�䣬�ó�Աֻ�ڸ����ļ���ʱ����
	ULONGLONG AllocatedSize;                // ������ٴ��̿ռ��������ɸ�����ֵ���ó�Աֻ�ڸ����ļ���ʱ����
	ULONGLONG DataSize;                     // ����ֵ���ֽڴ�С���ó�Աֻ�ڸ����ļ���ʱ����
	ULONG FileAttributes;                   // �ļ������ԣ��ó�Աֻ�ڸ����ļ���ʱ����
	ULONG AlignmentOrReserved;    
	UCHAR NameLength;                       // �ļ������ȣ��ַ�
	UCHAR NameType;                         // �ļ������� 0x01 = Long, 0x02 = Short��С��512�ֽڣ�
	WCHAR Name[1];                          // �ļ�����Unicode
} FILENAME_ATTRIBUTE, *PFILENAME_ATTRIBUTE;

/* ����ID���� */
typedef struct {
	GUID ObjectId;                // ������ļ���Ψһ��ʶ��
	union {
		struct {
			GUID BirthVolumeId;   // �ļ���һ�α����������ھ�ID
			GUID BirthObjectId;   // �ļ���һ�α�����ʱ�������ID
			GUID DomainId;        // ����
		};
		UCHAR ExtendedInfo[48];
	};
} OBJECTID_ATTRIBUTE, *POBJECTID_ATTRIBUTE;

/* ����Ϣ */
typedef struct {
	ULONG Unknown[2];
	UCHAR MajorVersion;   // NTFS��Ҫ�汾��
	UCHAR MinorVersion;   // NTFS��Ҫ�汾��
	USHORT Flags;         // 0x0001 = VolumeIsDirty 
} VOLUME_INFORMATION, *PVOLUME_INFORMATION;

/* Ŀ¼���� */
typedef struct {
	ULONG EntriesOffset;     // ��һ����¼��DIRECTORY_ENTRY�����ڲ��ֽ�ƫ����
	ULONG IndexBlockLength;  // �ⲿ����������ʹ���е��ֽڴ�С
	ULONG AllocatedSize;     // �����˶��ٴ��̿ռ�����������
	ULONG Flags;             // 0x00 = Small directory, index root��װ�����Ŀ¼, 0x01 = Large directory, index rootװ�������Ŀ¼
} DIRECTORY_INDEX, *PDIRECTORY_INDEX;

/* Ŀ¼��Ŀ */
typedef struct {
	ULONGLONG FileReferenceNumber;  // ����Ŀ�������ļ���FRN
	USHORT Length;                  // ����Ŀ�ֽڴ�С
	USHORT AttributeLength;         // �����������Ե��ֽڴ�С

	/* 0x01 = Has trailing VCN, 0x02 = Ŀ¼�������һ����Ŀ
	��flags = 0x01,��ô�� directory entry ���8���ֽڱ�ʾ��������Ŀǰһ����Ŀ��Ŀ¼���VCN */
	ULONG Flags; 
} DIRECTORY_ENTRY, *PDIRECTORY_ENTRY;

/* ��Ŀ¼ */
typedef struct {
	ATTRIBUTE_TYPE Type;
	ULONG CollationRule;
	ULONG BytesPerIndexBlock;        // ÿ����������ֽ���
	ULONG ClustersPerIndexBlock;     // ÿ��������Ĵ���
	DIRECTORY_INDEX DirectoryIndex;
} INDEX_ROOT, *PINDEX_ROOT;

/* ������ͷ�� */
typedef struct {
	NTFS_RECORD_HEADER Ntfs;         // ����Ϊ INDX �� NTFS_RECORD_HEADER
	ULONGLONG IndexBlockVcn;         // �������������غ�VCN
	DIRECTORY_INDEX DirectoryIndex;
} INDEX_BLOCK_HEADER, *PINDEX_BLOCK_HEADER;

/* �ؽ����� */
typedef struct {
	ULONG ReparseTag;
	USHORT ReparseDataLength;
	USHORT Reserved;
	UCHAR ReparseData[1];
} REPARSE_POINT, *PREPARSE_POINT;

/* extended attribute information */
typedef struct {
	ULONG EaLength;
	ULONG EaQueryLength;
} EA_INFORMATION, *PEA_INFORMATION;

typedef struct {
	ULONG NextEntryOffset;
	UCHAR Flags;
	UCHAR EaNameLength;
	USHORT EaValueLength;
	CHAR EaName[1];
	// UCHAR EaData[];
} EA_ATTRIBUTE, *PEA_ATTRIBUTE;

/* $AttrDef, �� MFT �е� Record number = 4*/
typedef struct {
	WCHAR AttributeName[64];
	ULONG AttributeNumber;
	ULONG Unknown[2];
	ULONG Flags;
	ULONGLONG MinimumSize;
	ULONGLONG MaximumSize;
} ATTRIBUTE_DEFINITION, *PATTRIBUTE_DEFINITION;

#pragma pack(push, 1)

typedef struct {
	UCHAR Jump[3];                   //����3���ֽ�
	UCHAR Format[8];                 //��N��'T' 'F' 'S' 0x20 0x20 0x20 0x20
	USHORT BytesPerSector;
	UCHAR SectorsPerCluster;
	USHORT BootSectors;
	UCHAR Mbz1;                      //����0
	USHORT Mbz2;                     //����0
	USHORT Reserved1;                //����0
	UCHAR MediaType;                 //������������Ӳ��Ϊ0xf8
	USHORT Mbz3;                     //��Ϊ0
	USHORT SectorsPerTrack;          //ÿ����������һ��Ϊ0x3f
	USHORT NumberOfHeads;            //��ͷ��
	ULONG PartitionOffset;           //�÷�����ƫ�ƣ����÷���ǰ������������ һ��Ϊ�ŵ�������0x3f 63��
	ULONG Reserved2[2];
	ULONGLONG TotalSectors;          //�÷�����������
	ULONGLONG MftStartLcn;           //MFT�����ʼ�غ�LCN
	ULONGLONG Mft2StartLcn;          //MFT���ݱ����ʼ�غ�LCN
	ULONG ClustersPerFileRecord;     //ÿ��MFT��¼���������أ���¼���ֽڲ�һ��Ϊ��ClustersPerFileRecord*SectorsPerCluster*BytesPerSector
	ULONG ClustersPerIndexBlock;     //ÿ��������Ĵ���
	LARGE_INTEGER VolumeSerialNumber; //�����к�
	UCHAR Code[0x1AE];
	USHORT BootSignature;
} BOOT_BLOCK, *PBOOT_BLOCK;

#pragma pack(pop)