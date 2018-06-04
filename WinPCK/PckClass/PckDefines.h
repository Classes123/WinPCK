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

#include "PckConf.h"

#if !defined(_PCKDEFINES_H_)
#define _PCKDEFINES_H_


typedef unsigned __int64	QWORD, *LPQWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
#define MAX_PATH			260

//zlib
#define Z_OK				0

//����ת�ַ�ʱʹ�õ��ַ�������
#define CHAR_NUM_LEN 12

#define	MAX_INDEXTABLE_CLEARTEXT_LENGTH	0x120
#define MAX_TAIL_LENGTH					300

#define	PCK_BEGINCOMPRESS_SIZE			20
#define	MAX_PATH_PCK_256				256
#define	MAX_PATH_PCK_260				260
#define PCK_ADDITIONAL_INFO_SIZE		252
#define PCK_DATA_START_AT				12




#define	PCK_ADDITIONAL_INFO				"Angelica File Package"
#define	PCK_ADDITIONAL_INFO_STSM		"\r\nCreate by WinPCK"

#if 0
//��־
#define	LOG_BUFFER						1024

#define	LOG_FLAG_ERROR					'E'
#define	LOG_FLAG_WARNING				'W'
#define	LOG_FLAG_INFO					'I'
#define	LOG_FLAG_DEBUG					'D'
#define	LOG_FLAG_NOTICE					'N'

#define LOG_IMAGE_NOTICE				0
#define LOG_IMAGE_INFO					1
#define LOG_IMAGE_WARNING				2
#define LOG_IMAGE_ERROR					3
#define LOG_IMAGE_DEBUG					4
#define LOG_IMAGE_EMPTY					5
#endif

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
#define	TEXT_EVENT_COMPRESS_PCK_DATA_FINISH			"CPckDataF%d"
#define	TEXT_EVENT_PCK_MAX_MEMORY					"PckMaxMem%d"

#define	TEXT_MAP_NAME_READ							"PckRead%d"
#define	TEXT_MAP_NAME_WRITE							"PckWrite%d"


#endif