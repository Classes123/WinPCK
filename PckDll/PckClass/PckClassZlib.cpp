
#include "zlib.h"
#include "libdeflate.h"
#include <Windows.h>
#include "PckClassZlib.h"
#include <assert.h>
#include "PckClassLog.h"

#pragma warning ( disable : 4267 )

CPckClassZlib::CPckClassZlib()
{
	init_compressor();
}

CPckClassZlib::~CPckClassZlib()
{}

/*
��ѹ���ļ�ʱ���е��ã�Ŀǰ���õĺ���ΪUpdatePckFile��ReCompress��ѹ���ļ�����ʱ������
*/
void CPckClassZlib::init_compressor()
{
	if (NULL != m_lpPckParams) {
		m_compress_level = m_lpPckParams->dwCompressLevel;

		if ((0 > m_compress_level) || (Z_MAX_COMPRESSION < m_compress_level))
			m_lpPckParams->dwCompressLevel = m_compress_level = Z_Default_COMPRESSION;
	}
	else {
		m_compress_level = Z_Default_COMPRESSION;
	}

	if(Z_Default_COMPRESSION < m_compress_level) {

		m_PckCompressFunc.compressBound = compressBound_libdeflate;
		m_PckCompressFunc.compress = compress_libdeflate;

	} else {

		m_PckCompressFunc.compressBound = compressBound_zlib;
		m_PckCompressFunc.compress = compress_zlib;

	}
}

int CPckClassZlib::check_zlib_header(void *data)
{
	char cDeflateFlag = (*(char*)data) & 0xf;
	if(Z_DEFLATED != cDeflateFlag)
		return 0;

	unsigned short header = _byteswap_ushort(*(unsigned short*)data);
	return (0 == (header % 31));
}

unsigned long CPckClassZlib::compressBound(unsigned long sourceLen)
{
	return m_PckCompressFunc.compressBound(sourceLen);
}

int	CPckClassZlib::compress(void *dest, unsigned long *destLen, const void *source, unsigned long sourceLen)
{
	return m_PckCompressFunc.compress(dest, destLen, source, sourceLen, m_compress_level);
}

unsigned long CPckClassZlib::compressBound_zlib(unsigned long sourceLen)
{
	return ::compressBound(sourceLen);
}

int	CPckClassZlib::compress_zlib(void *dest, unsigned long *destLen, const void *source, unsigned long sourceLen, int level)
{
	assert(0 != *destLen);
	int rtnc = compress2((Bytef*)dest, destLen, (Bytef*)source, sourceLen, level);
	assert(0 != *destLen);
	assert(rtnc == Z_OK);
	return (rtnc == Z_OK);
}

unsigned long CPckClassZlib::compressBound_libdeflate(unsigned long sourceLen)
{
	return libdeflate_zlib_compress_bound(NULL, sourceLen);
}

int	CPckClassZlib::compress_libdeflate(void *dest, unsigned long *destLen, const void *source, unsigned long sourceLen, int level)
{
	struct libdeflate_compressor* compressor;
	compressor = libdeflate_alloc_compressor(level);

	assert(0 != *destLen);

	*destLen = libdeflate_zlib_compress(compressor, source, sourceLen, dest, *destLen);
	libdeflate_free_compressor(compressor);

	assert(0 != *destLen);

	if(!(*destLen))
		return 0;

	return 1;
}

int CPckClassZlib::decompress(void *dest, unsigned long  *destLen, const void *source, unsigned long sourceLen)
{
	assert(0 != *destLen);
	int rtnd = uncompress((Bytef*)dest, destLen, (Bytef*)source, sourceLen);
	assert(0 != *destLen);
	assert(rtnd == Z_OK);
	return (rtnd == Z_OK);
}

//���ֻ��Ҫ��ѹһ��������
int CPckClassZlib::decompress_part(void *dest, unsigned long  *destLen, const void *source, unsigned long sourceLen, unsigned long  fullDestLen)
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

	CPckClassLog m_PckLog;

	unsigned long partlen = *destLen;
	assert(0 != *destLen);
	int rtn = uncompress((Bytef*)dest, destLen, (Bytef*)source, sourceLen);
	assert(0 != *destLen);

	if(Z_OK != rtn && !((Z_BUF_ERROR == rtn) && ((partlen == (*destLen)) || ((*destLen) < fullDestLen)))) {
		char *lpReason;
		switch(rtn) {
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

//��ȡ����ѹ����Ĵ�С�����Դ��СС��һ��ֵ�Ͳ�ѹ��
unsigned long CPckClassZlib::GetCompressBoundSizeByFileSize(unsigned long &dwFileClearTextSize, unsigned long &dwFileCipherTextSize, unsigned long dwFileSize)
{
	if(PCK_BEGINCOMPRESS_SIZE < dwFileSize) {
		dwFileClearTextSize = dwFileSize;
		dwFileCipherTextSize = compressBound(dwFileSize);
	} else {
		dwFileCipherTextSize = dwFileClearTextSize = dwFileSize;
	}

	return dwFileCipherTextSize;
}