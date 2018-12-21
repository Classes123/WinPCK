//////////////////////////////////////////////////////////////////////
// PckDefines.h: ���ڽ����������繫˾��pck�ļ��е����ݣ�����ʾ��List��
// ͷ�ļ�
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2015.5.13
//////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include <vector>
using namespace std;

typedef unsigned __int64	QWORD, *LPQWORD;


#if !defined(_PCKDEFINES_H_)
#define _PCKDEFINES_H_

#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif

#pragma warning ( disable : 4005 )
#pragma warning ( disable : 4995 )


typedef enum _FMTPCK
{
	FMTPCK_PCK = 0,
	FMTPCK_ZUP = 1,
	FMTPCK_CUP = 2,
	FMTPCK_UNKNOWN = 0x7fffffff
}FMTPCK;

typedef enum _PCKVER
{
	PCK_V2020,
	PCK_V2030,
	PCK_VXAJH
}PCK_CATEGORY;

typedef void(*ShowLogA)(const int log_level, const char *str);
typedef void(*ShowLogW)(const int log_level, const wchar_t *str);

#define MAX_PATH			260

//zlib
#define Z_OK				0

//params
#define	MT_MAX_MEMORY				0x80000000	//2048MB
#define MAX_COMPRESS_LEVEL			12
#define Z_DEFAULT_COMPRESS_LEVEL	9

#define PCK_OK					0   /* Successful result */
/* beginning-of-error-codes */
#define PCK_MSG_USERCANCELED	1   /* �û�ȡ�� */
#define PCK_ERROR				2   /* Generic error */
#define PCK_ERR_DISKFULL		3	/* �������� */	//���̿ռ䲻�㣬����ռ䣺%d��ʣ��ռ䣺%d
#define PCK_ERR_VIEWMAP_FAIL	4	/* Mapping ʧ�� */
#define PCK_ERR_VIEW			5	/* View ʧ�� */
#define PCK_ERR_OPENMAPVIEWR	6	/* OpenMappingViewAllRead ʧ�� */
#define PCK_ERR_MALLOC			7	/* �ڴ�����ʧ�� */

/* end-of-error-codes */

//��־

#define LOG_IMAGE_NOTICE				0
#define LOG_IMAGE_INFO					1
#define LOG_IMAGE_WARNING				2
#define LOG_IMAGE_ERROR					3
#define LOG_IMAGE_DEBUG					4
#define LOG_IMAGE_EMPTY					5
#define LOG_IMAGE_COUNT					6

//����ת�ַ�ʱʹ�õ��ַ�������
#define CHAR_NUM_LEN 12

#define	MAX_INDEXTABLE_CLEARTEXT_LENGTH	0x120
#define MAX_TAIL_LENGTH					300

#define	PCK_BEGINCOMPRESS_SIZE			20
#define	MAX_PATH_PCK_256				256
#define	MAX_PATH_PCK_260				260
#define PCK_ADDITIONAL_INFO_SIZE		252
#define PCK_DATA_START_AT				12

#define PCK_ENTRY_TYPE_INDEX			1
#define PCK_ENTRY_TYPE_NODE				2
#define PCK_ENTRY_TYPE_FOLDER			4
#define PCK_ENTRY_TYPE_DOTDOT			8
#define PCK_ENTRY_TYPE_ROOT				16
#define PCK_ENTRY_TYPE_TAIL_INDEX		0x80000000

#define	PCK_ADDITIONAL_INFO				"Angelica File Package"
#define	PCK_ADDITIONAL_INFO_STSM		"\r\nCreate by WinPCK"

//LOG INFO STRING
#define TEXT_LOG_OPENFILE				"���ļ� %s"
#define TEXT_LOG_CLOSEFILE				"�ر��ļ�"
#define	TEXT_LOG_FLUSH_CACHE			"д�뻺����..."
#define	TEXT_LOG_RESTOR_OK				"���ݻָ��ɹ�"

#define TEXT_LOG_WORKING_DONE			"�������"

#define	TEXT_LOG_LEVEL_THREAD			"ѹ����=%d:�߳�=%d"

#define	TEXT_LOG_UPDATE_ADD				"����ģʽ"
#define	TEXT_LOG_UPDATE_NEW				"�½�ģʽ"

#define	TEXT_LOG_RENAME					"������(ɾ��)�����ļ�..."
#define	TEXT_LOG_REBUILD				"�ؽ�PCK�ļ�..."
#define	TEXT_LOG_RECOMPRESS				"��ѹ��PCK�ļ�..."


#define	TEXT_LOG_CREATE					"�½�PCK�ļ�:%s..."

#define	TEXT_LOG_COMPRESSOK				"ѹ����ɣ�д������..."

#define	TEXT_LOG_EXTRACT				"��ѹ�ļ�..."



//ERROR STRING
#define	TEXT_ERROR						"����"

#define TEXT_INVALID_VERSION			"��Ч�İ汾ID��"

#define	TEXT_MALLOC_FAIL				"�����ڴ�ʧ�ܣ�"
#define	TEXT_CREATEMAP_FAIL				"�����ļ�ӳ��ʧ�ܣ�"
#define	TEXT_CREATEMAPNAME_FAIL			"ӳ���ļ�\"%s\"ʧ�ܣ�"
#define	TEXT_OPENNAME_FAIL				"���ļ�\"%s\"ʧ�ܣ�"
#define	TEXT_VIEWMAP_FAIL				"����ӳ����ͼʧ�ܣ�"
#define	TEXT_VIEWMAPNAME_FAIL			"�ļ�\"%s\"����ӳ����ͼʧ�ܣ�"

#define TEXT_READ_INDEX_FAIL			"�ļ��������ȡ����"
#define TEXT_UNKNOWN_PCK_FILE			"�޷�ʶ���PCK�ļ���"

#define	TEXT_OPENWRITENAME_FAIL			"��д���ļ�\"%s\"ʧ�ܣ�"
#define	TEXT_READFILE_FAIL				"�ļ���ȡʧ�ܣ�"
#define	TEXT_WRITEFILE_FAIL				"�ļ�д��ʧ�ܣ�"
#define TEXT_WRITE_PCK_INDEX			"д���ļ���������"

#define	TEXT_USERCANCLE					"�û�ȡ���˳���"

#define	TEXT_COMPFILE_TOOBIG			"ѹ���ļ�����"
#define	TEXT_UNCOMP_FAIL				"��ѹ�ļ�ʧ�ܣ�"

#define	TEXT_UNCOMPRESSDATA_FAIL		"�ļ� %s \r\n���ݽ�ѹʧ�ܣ�"
#define	TEXT_UNCOMPRESSDATA_FAIL_REASON	"���ݽ�ѹʧ��: %s"

#define	TEXT_ERROR_OPEN_AFTER_UPDATE	"��ʧ�ܣ��������ϴεĲ����������ļ����𻵡�\r\n�Ƿ��Իָ����ϴδ�״̬��"
#define	TEXT_ERROR_GET_RESTORE_DATA		"��ȡ�ָ���Ϣʱ����"
#define	TEXT_ERROR_RESTORING			"�ָ�ʱ����"
#define	TEXT_ERROR_DUP_FOLDER_FILE		"���ڣ��ļ���=�ļ����������˳���"

#define TEXT_UPDATE_FILE_INFO			"Ԥ����ļ���=%d:Ԥ���ļ���С=%lld, ��ʼ������ҵ..."

#define TEXT_PCK_SIZE_INVALID			"��Ч��PCK�ļ���С"
#define TEXT_VERSION_NOT_FOUND			"û�ҵ���Ч�İ汾"


#define	TEXT_EVENT_WRITE_PCK_DATA_FINISH			"WPckDataF%d"

#endif