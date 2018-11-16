#pragma once
#include <Windows.h>



#ifdef _DEBUG
int AtoW(const char *src, WCHAR *dst, int bufsize, int max_len, int cp);
int WtoA(const WCHAR *src, char *dst, int bufsize, int max_len, int cp);
int U8toW(const char *src, WCHAR *dst, int bufsize, int max_len = -1);
int WtoU8(const WCHAR *src, char *dst, int bufsize, int max_len = -1);
#else
int AtoW(const char *src, WCHAR *dst, int bufsize, int max_len, int cp);
int WtoA(const WCHAR *src, char *dst, int bufsize, int max_len, int cp);
int U8toW(const char *src, WCHAR *dst, int bufsize, int max_len = -1);
int WtoU8(const WCHAR *src, char *dst, int bufsize, int max_len = -1);
#endif

class CCharsCodeConv
{
public:
	CCharsCodeConv();
	~CCharsCodeConv();
	//���صĳ��Ȱ�����\0
	virtual int		GetConvertedStrLen() { return m_ConvertedStrLen - 1; }

protected:

	void *	m_buffer;
	int		m_ConvertedStrLen;
	BOOL	m_ok;

};


class CAnsi2Ucs : public CCharsCodeConv
{
public:
	CAnsi2Ucs(int cp) : codepage(cp) {}
	virtual ~CAnsi2Ucs() {};

	const wchar_t *GetString(const char *_src, wchar_t *_dst = NULL, int _dstsize = 0);	//ANSI��UNICODE��ת
	int		GetStrlen(const char *_src, wchar_t *_dst = NULL, int _dstsize = 0);	//ANSI�ַ��ڶ�Ӧ��UNICODE��wcslen�µķ���ֵ

private:
	int		codepage;
};

class CUcs2Ansi : public CCharsCodeConv
{
public:
	CUcs2Ansi(int cp) : codepage(cp) { }
	virtual ~CUcs2Ansi() {};

	const char *GetString(const wchar_t *_src, char *_dst = NULL, int _dstsize = 0);
	int		GetStrlen(const wchar_t *_src, char *_dst = NULL, int _dstsize = 0);	//UNICODE�ַ��ڶ�Ӧ��ANSI��strlen�µķ���ֵ

private:
	int		codepage;
};

class CU82Ucs : public CCharsCodeConv
{
public:
	CU82Ucs(){}
	virtual ~CU82Ucs() {};

	const wchar_t *GetString(const char *_src, wchar_t *_dst = NULL, int _dstsize = 0);	//ANSI��UNICODE��ת
	int		GetStrlen(const char *_src, wchar_t *_dst = NULL, int _dstsize = 0);	//ANSI�ַ��ڶ�Ӧ��UNICODE��wcslen�µķ���ֵ
};

class CUcs2U8 : public CCharsCodeConv
{
public:
	CUcs2U8(){}
	virtual ~CUcs2U8() {};

	const char *GetString(const wchar_t *_src, char *_dst = NULL, int _dstsize = 0);
	int		GetStrlen(const wchar_t *_src, char *_dst = NULL, int _dstsize = 0);	//UNICODE�ַ��ڶ�Ӧ��ANSI��strlen�µķ���ֵ
};
