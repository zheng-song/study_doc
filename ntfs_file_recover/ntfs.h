
#ifndef NTFS_H_INCLUDED
#define NTFS_H_INCLUDED

// **NTFS RECORD HEADER**
typedef struct {
	ULONG Type;  		// 4B 'ELIF','XDNI','DAAB','ELOH','DKHC'
	USHORT UsaOffset;	// �������к�����ƫ��
	USHORT UsaCount;	// �������кŵ��������+1(N)��ͨ��Ϊ3
	USN Usn; 			//USN == LONLONG 8B ÿһ�μ�¼���޸ģ�USN����仯
}NTFS_RECORD_HEADER, *PNTFS_RECORD_HEADER; // 16B

//****FILE RECORD_HEADER****
typedef struct {
	NTFS_RECORD_HEADER Ntfs;   	// Ntfs.type ����'ELIF'
	USHORT SequenceNumber;		// $LogFile Sequence Number  
	USHORT LinkCount;			// ��¼Ӳ���ӵ���Ŀ��ֻ�����ڻ����ļ���¼��
	USHORT AttributeOffset;		// ��һ�����Ե�ƫ�� 
	USHORT Flags;				// 0x0000=deleted file; 0x0001=file, 0x0002=deleted dir; 0x0003= dir;
	ULONG BytesInUse;			// �ѱ��� MFT entry ʹ�õ��ֽ���
	ULONG BytesAllocated;		// ������� MFT entry ���ֽ���
/*��ǰ�ļ���¼�Ļ����ļ���¼�������������ǰ�ļ���¼�ǻ����ļ���¼���ֵΪ0��
 *����ָ������ļ���¼�ļ�¼������ע���ֵ�ĵ�6�ֽ���MFT��¼�ţ���2�ֽ��Ǹ�MFT��¼�����кš�*/
	ULONGLONG BaseFileRecord;	
	USHORT NextAttributeNumber;	// ��һ����ID	
	UCHAR Unused[2];
	ULONG MFTRecordNo;			   // ��MFT�ļ�¼���(��ʼ���0)
//	ULONGLONG Usa; //�������кŵ�λ�úʹ�С��ǰ��� UsaOffset ��UsaCount������.
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

/* �ļ���׼��Ϣ���� */
typedef struct {
	ULONGLONG CreationTime;
	ULONGLONG ChangeTime;
	ULONGLONG LastWriteTime;
	ULONGLONG LastAccessTime;
	ULONG FileAttributes;              // �ļ������ԣ�����ֻ��0001������0002��ϵͳ0004������4000��
	ULONG AlignmentOrReservedOrUnknown[3];
	ULONG QuotaId;                     // NTFS 3.0
	ULONG SecurityId;                  // NTFS 3.0
	ULONGLONG QuotaCharge;             // NTFS 3.0
	USN Usn;                           // NTFS 3.0
} STANDARD_INFORMATION, *PSTANDARD_INFORMATION;


typedef struct {
	ATTRIBUTE_TYPE AttributeType;
	ULONG AttributeLength; 			//�����Գ��ȣ���������ֵ��
	BOOLEAN Nonresident;			//�������ǲ���פ������ 0x00=Resident, 0x01 = NonResident
	UCHAR NameLength;				//�����������Ƴ��� ����Ϊ2*NameLength
	USHORT NameOffset;				//������ƫ��
	USHORT Flags;	 				// ѹ�������ܡ�ϡ��ȵı�ʶ0x00FF=compressed  0x4000=encrypted  0x8000=Sparse
	USHORT AttributeID;				// ������ʵ�������ֱ�ʶ��
}ATTRIBUTE, *PATTRIBUTE;


typedef struct {
	ATTRIBUTE Attribute;
	ULONG ContentLength;	//�����峤��
	USHORT ContentOffset; 	//������ƫ��
// �����Ƿ�����������������������һ������(��Ŀ¼)�Ļ������������־ 0x0001 = Indexed
	UCHAR IndexFlags;
	UCHAR Unused;
} RESIDENT_ATTRIBUTE, *PRESIDENT_ATTRIBUTE;

// ����ṹ�Ķ����ͼƬ��ṹ��������һ��
// TODO
typedef struct {
	ATTRIBUTE Attribute;
	ULONGLONG LowVcn;	   // ��ʼ����غ�VCN
	ULONGLONG HighVcn;	   // ��������غ�VCN
	USHORT RunArrayOffset; // run list�����б��ƫ���� ����ֵΪ40
	UCHAR CompressionUint; // ���ó�ԱΪ0��������δ��ѹ��
	UCHAR Reserved[5];			// 
	ULONGLONG StreamAllocSize;	// Ϊ����ֵ����Ŀռ䣬��λΪ��
	ULONGLONG StreamRealSize;	// ����ֵʵ��ʹ�õĿռ䣬��λΪ��
	ULONGLONG InitializedSize;	// ���Ե�ԭʼ��С��
//	ULONGLONG CompressedSize;    // Only when compressed ѹ����λ�Ĵ�С��2��N�η�   
//��������run list����Ϣ��
} NONRESIDENT_ATTRIBUTE, *PNONRESIDENT_ATTRIBUTE;


/* �ļ������б����� */
/*
 * ������MFT��¼װ�����ļ�������ʱ�����ļ��Ż���attribute list���ԣ�װ���µĿ���ԭ��
 * 1. The file has a large numbers of alternate names (�ļ��д���Ӳ����)
 * 2. The attribute value is large, and the volume is badly fragmented������ֵ̫�󣬾����ط�ɢ��
 * 3. The file has a complex security descriptor (�ļ��İ�ȫ������̫���ӣ��������NTFS 3.0ûӰ��)
 * 4. The file has many streams���ļ��кܶ�����
 */
typedef struct {
	ATTRIBUTE_TYPE AttributeType; 			//��������
	USHORT Length;							//����¼����
	UCHAR NameLength;						//����������(N,Ϊ0��ʾû��������)
	UCHAR NameOffset;						//������ƫ��
	ULONGLONG LowVcn;						//��ʼVCN(���Գ�פʱΪ0)
	ULONGLONG FileReferenceNumber;			//���Ե��ļ��ο���
	USHORT AttributeNumber;					//���Ե�ID(ÿһ�����Զ���һ��Ψһ��ID��)
/*��������2N���ֽڱ�ʾ��������Unicode�룬������������Ļ����������0����*/
	USHORT AlignmentOrReserved[3];
} ATTRIBUTE_LIST, *PATTRIBUTE_LIST;


typedef struct {  
	ULONGLONG DirectoryFileReferenceNumber; 	// ��Ŀ¼���ļ��ο���   
	ULONGLONG CreateTime;						// �ļ�����ʱ��
	ULONGLONG ModifiedTime;						// �ļ��޸�ʱ��
	ULONGLONG LastWriteTime; 					// ���һ��MFT����ʱ��   
	ULONGLONG LastAccessTime;					// ���һ�εķ���ʱ��
	ULONGLONG AllocSize; 						// �ļ�����Ĵ�С   
	ULONGLONG DataSize; 						// �ļ�ʹ�õĴ�С  
	ULONG Flags;								// ��־����Ŀ¼��ѹ�������ص�
	ULONG ReparseValue;							// ����EAS���ص����
	UCHAR NameLength;							// ���ַ��Ƶ��ļ������ȣ�ÿ���ַ�ռ�õ��ֽ����������ռ����
	UCHAR NameType; 							// �ļ����������ռ�0x0  WIN32 0x01  DOS 0x02  WIN32&DOS 0x3   
//	WCHAR Name[1]; // �ⲿ����UNICODE��ʽ��ʾ���ļ���������ͨ��Ϊ2*NameLength��
} FILENAME_ATTRIBUTE, *PFILENAME_ATTRIBUTE;


typedef struct {
	GUID ObjectId;								// ������ļ���Ψһ��ʶ��
	union {
		struct {
			GUID BirthVolumeId;					// �ļ���һ�α����������ھ�ID
			GUID BirthObjectId;					// �ļ���һ�α�����ʱ�������ID
			GUID DomainId;						// ����
		};
		UCHAR ExtendedInfo[48];
	};
} OBJECTID_ATTRIBUTE, *POBJECTID_ATTRIBUTE;


typedef struct {
	ULONG Unknown[2];
	UCHAR MajorVersion;	// NTFS��Ҫ�汾��
	UCHAR MinorVersion;	// NTFS��Ҫ�汾��
	USHORT Flags;		// 0x0001 = VolumeIsDirty
} VOLUME_INFORMATION, *PVOLUME_INFORMATION;



typedef struct {
	ULONGLONG MFTnum;		//�ļ���MFT�ο��ţ�ǰ6B�Ǹ��ļ���MFT��ţ����ڶ�λ���ļ�
	USHORT ItemLen;			//������Ĵ�С
	UCHAR IdxIdentifier[2];	//������־
	UCHAR IsNode[2];		//1--����������һ���ӽڵ㣬0--��Ϊ���һ��
	UCHAR noUse[2];			//���
	ULONGLONG FMFTnum;		//��Ŀ¼��MFT�ο��ţ�0x05��ʾ��Ŀ¼��
	ULONGLONG	CreateTime;
	ULONGLONG FileModifyTime;
	ULONGLONG	MFTModifyTime;
	ULONGLONG LastVisitTime;
	ULONGLONG	FileAllocSize;
	ULONGLONG FileRealSize;
	UCHAR FileIdentifier[8];//�ļ���ʶ
	UCHAR FileNameLen;		//�ļ�������F
	UCHAR FileNameSpace;	//�ļ��������ռ�
//---0x52--- �ļ���������Ϊ2F�����������䵽8�ֽ�
//	ULONGLONG NextItemVCN;
}INDEXITEM, *PINDEXITEM;

/*Ŀ¼��Ŀ*/
typedef struct {
	ULONGLONG FileReferenceNumber;	// ����Ŀ�������ļ���FRN
	USHORT Length;					// ����Ŀ�ֽڴ�С
	USHORT AttributeLength;			// �����������Ե��ֽڴ�С
	ULONG Flags; // 0x01 = Has trailing VCN, 0x02 = Last entry
				 // FILENAME_ATTRIBUTE Name;
				 // ULONGLONG Vcn; // VCN in IndexAllocation of earlier entries
} DIRECTORY_ENTRY, *PDIRECTORY_ENTRY;

typedef struct {
	ULONG EntriesOffset;	// ��һ����¼��DIRECTORY_ENTRY�����ڲ��ֽ�ƫ����
	ULONG IndexBlockLength;	// �ⲿ����������ʹ���е��ֽڴ�С
	ULONG AllocatedSize;	// �����˶��ٴ��̿ռ�����������
	ULONG Flags;			// 0x00 = Small directory, index root��װ�����Ŀ¼, 0x01 = Large directory, index rootװ�������Ŀ¼
} DIRECTORY_INDEX, *PDIRECTORY_INDEX;


/*������ͷ��*/
typedef struct {
	NTFS_RECORD_HEADER Ntfs;	// ����Ϊ INDX �� NTFS_RECORD_HEADER
	ULONGLONG VcnIndex;			//�����������ڷ��������е�VCN
	ULONG ItemOffset;			//��һ�������ƫ��
	ULONG ItemSize;				//�������ʵ�ʴ�С(B)
	ULONG ItemAllocSize;		//������ķ����С(B��������ͷ��)����С��4K
	UCHAR IsNode;				//�ǽڵ���Ϊ1�������ʾ���ӽڵ㡣
	UCHAR Nouse[3];				//���
//	UCHAR USN[2 * UsnUpdateArrSize];    //�˴���һ��ʹ��Ntfs�е�UsnUpdateArrSize*2��С���ֽڵ�USN��
};

/*��Ŀ¼*/
typedef struct {
	ATTRIBUTE_TYPE Type;		// 4B ��������
	ULONG CollationRule;		// 4B �������
	ULONG BytesPerIndexBlock;	// 4B ����������С
	UCHAR ClustersPerIndexBlock;// ÿ������¼�Ĵ���
	UCHAR noUse[3];
	DIRECTORY_INDEX DirectoryIndex;
} INDEX_ROOT, *PINDEX_ROOT;


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

#pragma pack(push,1)   
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
	USHORT SectorsPerTrack;	//ÿ����������Ӳ��һ��Ϊ0x3f   
	USHORT NumberOfHeads;	//Ӳ�̵Ĵ�ͷ��   
	ULONG PartitionOffset;	//�÷�����ƫ�ƣ����÷���ǰ������������ һ��Ϊһ���ŵ�������0x3f 63��   
	ULONG Reserved2[2];
	ULONGLONG TotalSectors;	//�÷�����������   
	ULONGLONG MftStartLcn;	//MFT�����ʼ�غ�LCN   
	ULONGLONG Mft2StartLcn;	//MFT���ݱ����ʼ�غ�LCN   
	ULONG ClustersPerFileRecord;//ÿ��MFT��¼����������  
	//��¼���ֽڲ�һ��Ϊ��ClustersPerFileRecord*SectorsPerCluster*BytesPerSector  
	ULONG ClustersPerIndexBlock;//ÿ��������Ĵ���   
	LARGE_INTEGER VolumeSerialNumber;//�����к�   
	UCHAR Code[0x1AE];
	USHORT BootSignature;			//���� 0xAA55
} BOOT_BLOCK, *PBOOT_BLOCK;

#pragma pack(pop) 
	
#endif	// NTFS_H_INCLUDED