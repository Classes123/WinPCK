#pragma once

class CPckClassNode
{
public:
	CPckClassNode();
	~CPckClassNode();


	LPPCK_PATH_NODE		GetRootNode();
	//��PckIndex�ļ�����·�����������Node
	LPPCK_PATH_NODE		ParseIndexTableToNode();
	//������ͬ�Ľڵ�
	LPPCK_PATH_NODE		FindFileNode(LPPCK_PATH_NODE lpBaseNode, char* lpszFile);

	//ɾ��һ���ڵ�
	VOID	DeleteNode(LPPCK_PATH_NODE lpNode);

	//������һ���ڵ�
	BOOL	RenameNode(LPPCK_PATH_NODE lpNode, char* lpszReplaceString);
protected:
	BOOL	RenameNodeEnum(LPPCK_PATH_NODE lpNode, size_t lenNodeRes, char* lpszReplaceString, size_t lenrs, size_t lenrp);
	BOOL	RenameNode(LPPCK_PATH_NODE lpNode, size_t lenNodeRes, char* lpszReplaceString, size_t lenrs, size_t lenrp);
public:
	VOID	RenameIndex(LPPCK_PATH_NODE lpNode, char* lpszReplaceString);
	VOID	RenameIndex(LPPCKINDEXTABLE lpIndex, char* lpszReplaceString);

	//��ȡnode·��
	BOOL	GetCurrentNodeString(char*szCurrentNodePathString, LPPCK_PATH_NODE lpNode);

	//Indexes
	LPPCKINDEXTABLE		AllocPckIndexTableByFileCount(DWORD dwFileCount);
	//��lpPckIndexTable->cFileIndex.szFilename �е�Ansi�ַ�ת��־Unicode��д��lpPckIndexTable->cFileIndex.szwFilename��
	void				GenerateUnicodeStringToIndex();

private:

	LPPCKINDEXTABLE		m_lpPckIndexTable;	//PCK�ļ�������
	PCK_PATH_NODE		m_cRootNode;		//PCK�ļ��ڵ�ĸ��ڵ�
	DWORD				m_dwFileCount;		//�ļ�����

	//��PckIndex�ļ�����·�����������Node
	BOOL	AddFileToNode(LPPCKINDEXTABLE	lpPckIndexNode);

	void*	AllocMemory(size_t	sizeStuct);

	//�ͷ��ڴ�
	VOID	DeAllocMultiNodes(LPPCK_PATH_NODE lpThisNode);
};

