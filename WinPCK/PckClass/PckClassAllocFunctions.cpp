//////////////////////////////////////////////////////////////////////
// PckClassAllocFunctions.cpp: PCK�ļ����ܹ����е��ڴ��������ͷ��ӹ��� 
//
// �˳����� �����/stsm/liqf ��д��pck�ṹ�ο���ˮ��pck�ṹ.txt����
// �ο����������Դ����в��ڶ�pck�ļ��б�Ĳ���
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2015.5.19
//////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4996 )


#include "PckClassAllocFunctions.h"

namespace NPckClassAllocFuncs
{

	void* AllocMemory(size_t	sizeStuct)
	{
		void*		lpMallocNode;

		if(NULL == (lpMallocNode = malloc(sizeStuct))) {
			//m_PckLog.PrintLogEL(TEXT_MALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
			return lpMallocNode;
		}
		//��ʼ���ڴ�
		memset(lpMallocNode, 0, sizeStuct);

		return lpMallocNode;

	}

	VOID DeAllocMultiNodes(LPPCK_PATH_NODE lpThisNode)
	{

		LPPCK_PATH_NODE		lpThisNodePtr = lpThisNode;
		//BOOL				bFinish = FALSE;

		while(NULL != lpThisNode) {

			lpThisNodePtr = lpThisNode;
			if(NULL != lpThisNodePtr->child) {
				DeAllocMultiNodes(lpThisNodePtr->child);
			}
			lpThisNode = lpThisNodePtr->next;

			free(lpThisNodePtr);

		}

	}
}

VOID CPckClass::DeleteNode(LPPCK_PATH_NODE lpNode)
{
	lpNode = lpNode->child->next;

	while(NULL != lpNode) {
		if(NULL == lpNode->child) {
			lpNode->lpPckIndexTable->bSelected = TRUE;
		} else {
			DeleteNode(lpNode);
		}

		lpNode = lpNode->next;
	}

}
