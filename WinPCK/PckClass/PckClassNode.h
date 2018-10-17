#pragma once
#include "PckClassIndex.h"

#define INVALID_NODE	( -1 )

class CPckClassNode :
	protected virtual CPckClassIndex
{
public:
	CPckClassNode();
	~CPckClassNode();

	//��PckIndex�ļ�����·�����������Node
	void			ParseIndexTableToNode(LPPCKINDEXTABLE lpMainIndexTable);
	//������ͬ�Ľڵ�
	LPPCK_PATH_NODE	FindFileNode(const LPPCK_PATH_NODE lpBaseNode, wchar_t* lpszFile);

	//ɾ��һ���ڵ�
	virtual VOID	DeleteNode(LPPCK_PATH_NODE lpNode);
	virtual VOID	DeleteNode(LPPCKINDEXTABLE lpIndex);

#if 0
	//������һ���ڵ�
	virtual	BOOL	RenameNode(LPPCK_PATH_NODE lpNode, char* lpszReplaceString);
#endif
protected:
	BOOL	RenameNodeEnum(LPPCK_PATH_NODE lpNode, size_t lenNodeRes, char* lpszReplaceString, size_t lenrs, size_t lenrp);
	BOOL	RenameNode(LPPCK_PATH_NODE lpNode, size_t lenNodeRes, char* lpszReplaceString, size_t lenrs, size_t lenrp);
public:
#if 0
	virtual	BOOL	RenameIndex(LPPCK_PATH_NODE lpNode, char* lpszReplaceString);
	virtual	BOOL	RenameIndex(LPPCKINDEXTABLE lpIndex, char* lpszReplaceString);
#endif

	//������һ���ڵ�
	virtual	BOOL	RenameNode(LPPCK_PATH_NODE lpNode, wchar_t* lpszReplaceString);
	virtual	BOOL	RenameIndex(LPPCK_PATH_NODE lpNode, wchar_t* lpszReplaceString);
	virtual	BOOL	RenameIndex(LPPCKINDEXTABLE lpIndex, wchar_t* lpszReplaceString);

protected:
	//������ļ���
	BOOL			CheckNewFilename();

	//��ȡnode·��
	BOOL	GetCurrentNodeString(char*szCurrentNodePathString, LPPCK_PATH_NODE lpNode);
public:
	BOOL	GetCurrentNodeString(wchar_t *szCurrentNodePathString, LPPCK_PATH_NODE lpNode);
protected:
	BOOL	FindDuplicateNodeFromFileList(const LPPCK_PATH_NODE lpNodeToInsertPtr, DWORD &_in_out_FileCount);

private:

	LPPCK_PATH_NODE		m_lpRootNode;		//PCK�ļ��ڵ�ĸ��ڵ�

	//��PckIndex�ļ�����·�����������Node
	BOOL	AddFileToNode(LPPCKINDEXTABLE	lpPckIndexNode);

	//�ͷ��ڴ�
	VOID	DeAllocMultiNodes(LPPCK_PATH_NODE lpThisNode);
};

