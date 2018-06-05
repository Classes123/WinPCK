//////////////////////////////////////////////////////////////////////
// PckClassCompress.cpp: ���ڽ����������繫˾��pck�ļ��е����ݣ�����ʾ��List��
// �ļ�ͷ��β�Ƚṹ��Ķ�ȡ
//
// �˳����� �����/stsm/liqf ��д��pck�ṹ�ο���ˮ��pck�ṹ.txt����
// �ο����������Դ����в��ڶ�pck�ļ��б�Ĳ���
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2017.7.3
//////////////////////////////////////////////////////////////////////

#include "PckClass.h"

#include "zlib.h"
#include "libdeflate.h"

#pragma warning ( disable : 4267 )

void CPckClass::init_compressor()
{

	if(Z_BEST_COMPRESSION < m_lpPckParams->dwCompressLevel){

		m_PckCompressFunc.compressBound = compressBound_libdeflate;
		m_PckCompressFunc.compress = compress_libdeflate;

	}else{

		m_PckCompressFunc.compressBound = compressBound_zlib;
		m_PckCompressFunc.compress = compress_zlib;

	}
}

int CPckClass::check_zlib_header(void *data)
{
	char cDeflateFlag = (*(char*)data) & 0xf;
	if (Z_DEFLATED != cDeflateFlag)
		return 0;

	unsigned short header = _byteswap_ushort(*(unsigned short*)data);
	return (0 == (header % 31));
}

unsigned long CPckClass::compressBound(unsigned long sourceLen)
{
	return m_PckCompressFunc.compressBound(sourceLen);
}

int	CPckClass::compress(void *dest, unsigned long *destLen, const void *source, unsigned long sourceLen, int level)
{
	return m_PckCompressFunc.compress(dest, destLen, source, sourceLen, level);
}

unsigned long CPckClass::compressBound_zlib(unsigned long sourceLen)
{
	return ::compressBound(sourceLen);
}

int	CPckClass::compress_zlib(void *dest, unsigned long *destLen, const void *source, unsigned long sourceLen, int level)
{
	assert(0 != *destLen);
	int rtnc = compress2((Bytef*)dest, destLen, (Bytef*)source, sourceLen, level);
	assert(0 != *destLen);
	assert(rtnc == Z_OK);
	return (rtnc == Z_OK);
}

unsigned long CPckClass::compressBound_libdeflate(unsigned long sourceLen)
{
	return libdeflate_zlib_compress_bound(NULL, sourceLen);
}

int	CPckClass::compress_libdeflate(void *dest, unsigned long *destLen, const void *source, unsigned long sourceLen, int level)
{
	struct libdeflate_compressor* compressor;
	compressor = libdeflate_alloc_compressor(level);

	assert(0 != *destLen);

	*destLen = libdeflate_zlib_compress(compressor, source, sourceLen, dest, *destLen);
	libdeflate_free_compressor(compressor);

	assert(0 != *destLen);

	if (!(*destLen))
		return 0;

	return 1;
}

int CPckClass::decompress(void *dest, unsigned long  *destLen, const void *source, unsigned long sourceLen)
{
	assert(0 != *destLen);
	int rtnd = uncompress((Bytef*)dest, destLen, (Bytef*)source, sourceLen);
	assert(0 != *destLen);
	assert(rtnd == Z_OK);
	return (rtnd == Z_OK);
}

//���ֻ��Ҫ��ѹһ��������
int CPckClass::decompress_part(void *dest, unsigned long  *destLen, const void *source, unsigned long sourceLen, unsigned long  fullDestLen)
{
/*
#define Z_OK            0
#define Z_STREAM_END    1
#define Z_NEED_DICT     2
#define Z_ERRNO        (-1)
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR   (-3)
#define Z_MEM_ERROR    (-4)
#define Z_BUF_ERROR    (-5)
#define Z_VERSION_ERROR (-6)
*/
	unsigned long partlen = *destLen;
	assert(0 != *destLen);
	int rtn = uncompress((Bytef*)dest, destLen, (Bytef*)source, sourceLen);
	assert(0 != *destLen);

	if (Z_OK != rtn && !((Z_BUF_ERROR == rtn) && ((partlen == (*destLen)) || ((*destLen) < fullDestLen)))) {
		char *lpReason;
		switch (rtn) {
		case Z_NEED_DICT:
			lpReason = "��Ҫ�ֵ�";
			break;
		case Z_STREAM_ERROR:
			lpReason = "��״̬����";
			break;
		case Z_DATA_ERROR:
			lpReason = "������Ч";
			break;
		case Z_MEM_ERROR:
			lpReason = "û���㹻���ڴ�";
			break;
		case Z_BUF_ERROR:
			lpReason = "�������ռ䲻��";
			break;
		default:
			lpReason = "��������";
		}
		assert(FALSE);
		m_PckLog.PrintLogEL(TEXT_UNCOMPRESSDATA_FAIL_REASON, lpReason, __FILE__, __FUNCTION__, __LINE__);
		return rtn;
	} else {
		return Z_OK;
	}
}