#pragma once
#if 1
#include <Windows.h>
#include "PckStructs.h"
template<typename Type>
class LinkList
{
private:
	Type * head;//ͷָ��  
	Type *tail;//βָ��
	Type* AllocMemory();
public:
	LinkList();
	~LinkList();

	BOOL insertNext();
	Type *last();
	Type *first();

};

template<typename Type>
LinkList<Type>::LinkList()
{
	head = AllocMemory();
	tail = head;
	head->next = NULL;
}

template<typename Type>
Type* LinkList<Type>::AllocMemory()
{
	Type*		lpMallocNode;

	if(NULL == (lpMallocNode = (Type*)malloc(sizeof(Type)))) {
		return lpMallocNode;
	}
	//��ʼ���ڴ�
	memset(lpMallocNode, 0, sizeof(Type));

	return lpMallocNode;

}

template<typename Type>
BOOL LinkList<Type>::insertNext()
{
	tail->next = AllocMemory();
#ifdef _DEBUG
	tail->next->id = tail->id + 1;
#endif
	tail = tail->next;
	tail->next = NULL;
	return (NULL != tail);
}

template<typename Type>
Type* LinkList<Type>::last()
{
	return tail;
}

template<typename Type>
Type* LinkList<Type>::first()
{
	return head;
}

template<typename Type>
LinkList<Type>::~LinkList()
{
	Type* Fileinfo;

	while(head != NULL) {
		Fileinfo = head->next;
		free(head);
		head = Fileinfo;
	}
	head = NULL;
}
#endif