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
#include "PckClassLog.h"

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

	//�ؽ�pck�ļ�
	virtual BOOL	RebuildPckFile(LPCTSTR lpszScriptFile, LPCTSTR szRebuildPckFile, BOOL bUseRecompress) { m_PckLog.PrintLogE(TEXT_NOTSUPPORT);return FALSE; }
	virtual BOOL	StripPck(LPCTSTR lpszStripedPckFile, int flag) { m_PckLog.PrintLogE(TEXT_NOTSUPPORT); return FALSE; }

	//�½�������pck�ļ�
	virtual BOOL	UpdatePckFile(LPCTSTR szPckFile, const vector<tstring> &lpszFilePath, const PCK_PATH_NODE* lpNodeToInsert) { m_PckLog.PrintLogE(TEXT_NOTSUPPORT);return FALSE; }

	//�������ļ�
	virtual BOOL	RenameFilename() { m_PckLog.PrintLogE(TEXT_NOTSUPPORT);return FALSE; }

	//ɾ��һ���ڵ�
	virtual VOID	DeleteNode(LPPCK_PATH_NODE lpNode) { m_PckLog.PrintLogE(TEXT_NOTSUPPORT);}
	virtual VOID	DeleteNode(LPPCKINDEXTABLE lpIndex) { m_PckLog.PrintLogE(TEXT_NOTSUPPORT); }

	//������һ���ڵ�
	virtual	BOOL	RenameNode(LPPCK_PATH_NODE lpNode, const wchar_t* lpszReplaceString) { m_PckLog.PrintLogE(TEXT_NOTSUPPORT); return FALSE; }
	virtual	BOOL	RenameIndex(LPPCK_PATH_NODE lpNode, const wchar_t* lpszReplaceString) { m_PckLog.PrintLogE(TEXT_NOTSUPPORT); return FALSE; }
	virtual	BOOL	RenameIndex(LPPCKINDEXTABLE lpIndex, const wchar_t* lpszReplaceString) { m_PckLog.PrintLogE(TEXT_NOTSUPPORT); return FALSE; }

	//Ԥ���ļ�
	virtual BOOL	GetSingleFileData(const PCKINDEXTABLE* const lpZupFileIndexTable, char *buffer, size_t sizeOfBuffer = 0);

private:

	//LPPCK_PATH_NODE				m_lpRootNodeZup;
	LPPCKINDEXTABLE				m_lpZupIndexTable;

	CDictHash					*m_lpDictHash;

private:

	void	BuildDirTree();
	BOOL	BuildZupBaseDict();
	void	DecodeFilename(char *_dst, wchar_t *_wdst, char *_src);

	_inline void	DecodeDict(LPZUP_FILENAME_DICT lpZupDict);
	VOID	AddDict(char *&lpszStringToAdd);

	const PCKINDEXTABLE* GetBaseFileIndex(const PCKINDEXTABLE* lpIndex, const PCKINDEXTABLE* lpZeroBaseIndex);

};

#endif