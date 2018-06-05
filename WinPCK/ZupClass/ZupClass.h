//////////////////////////////////////////////////////////////////////
// ZupClass.h: ���ڽ����������繫˾��zup�ļ��е����ݣ�����ʾ��List��
// ͷ�ļ�
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2012.5.23
//////////////////////////////////////////////////////////////////////

#include "PckClass.h"
#include "ZupHeader.h"
#include "DictHash.h"

#if !defined(_ZUPCLASS_H_)
#define _ZUPCLASS_H_

class CZupClass : public CPckClass
{
public:

	//CZupClass();
	CZupClass(LPPCK_RUNTIME_PARAMS inout);
	virtual ~CZupClass();

	BOOL	Init(LPCTSTR	szFile);

	CONST	LPPCKINDEXTABLE		GetPckIndexTable();
	CONST	LPPCK_PATH_NODE		GetPckPathNode();

	//�½�pck�ļ�
	virtual BOOL	CreatePckFile(LPTSTR szPckFile, LPTSTR szPath) { m_PckLog.PrintLogE(TEXT_NOTSUPPORT);return FALSE; }

	//�ؽ�pck�ļ�
	virtual BOOL	RebuildPckFile(LPTSTR szRebuildPckFile, BOOL bUseRecompress) { m_PckLog.PrintLogE(TEXT_NOTSUPPORT);return FALSE; }

public:
	//����pck�ļ�//��bug
	virtual BOOL	UpdatePckFile(LPTSTR szPckFile, TCHAR (*lpszFilePath)[MAX_PATH], int nFileCount, LPPCK_PATH_NODE lpNodeToInsert) { m_PckLog.PrintLogE(TEXT_NOTSUPPORT);return FALSE; }

	//�������ļ�
	virtual BOOL	RenameFilename() { m_PckLog.PrintLogE(TEXT_NOTSUPPORT);return FALSE; }

	//ɾ��һ���ڵ�
	virtual VOID	DeleteNode(LPPCK_PATH_NODE lpNode) { m_PckLog.PrintLogE(TEXT_NOTSUPPORT);}

	//������һ���ڵ�
	virtual BOOL	RenameNode(LPPCK_PATH_NODE lpNode, char* lpszReplaceString){m_PckLog.PrintLogE(TEXT_NOTSUPPORT);return FALSE;}

	virtual VOID	RenameIndex(LPPCK_PATH_NODE lpNode, char* lpszReplaceString) { m_PckLog.PrintLogE(TEXT_NOTSUPPORT); }
	virtual VOID	RenameIndex(LPPCKINDEXTABLE lpIndex, char* lpszReplaceString) { m_PckLog.PrintLogE(TEXT_NOTSUPPORT); }

	//Ԥ���ļ�
	BOOL	GetSingleFileData(LPVOID lpvoidFileRead, LPPCKINDEXTABLE lpZupFileIndexTable, char *buffer, size_t sizeOfBuffer = 0);


protected:

	LPPCK_PATH_NODE				m_lpRootNodeZup;
	LPPCKINDEXTABLE				m_lpZupIndexTable;

	CDictHash					*m_lpDictHash;

	CPckClassNode				m_classNodeZup;


protected:

	virtual void	BuildDirTree();
	BOOL	BuildZupBaseDict();
	void	DecodeFilename(char *_dst, wchar_t *_wdst, char *_src);

	_inline void	DecodeDict(LPZUP_FILENAME_DICT lpZupDict);
	VOID	AddDict(char *&lpszStringToAdd);
	//VOID	EnumAndDecodeNode(LPPCK_PATH_NODE lpNodeToDecodeZup, LPPCK_PATH_NODE lpNodeToDecodePck);

	LPPCKINDEXTABLE GetBaseFileIndex(LPPCKINDEXTABLE lpIndex, LPPCKINDEXTABLE lpZeroBaseIndex);

};

#endif