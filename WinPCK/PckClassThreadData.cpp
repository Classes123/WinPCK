#include "PckClassThread.h"

//�ڶ��߳������л�ȡδѹ���õ�Դ����
FETCHDATA_RET CPckClassThreadWorker::GetUncompressedDataFromFile(CPckClassThreadWorker *pThis, LPDATA_FETCH_METHOD lpDataFetchMethod, PCKINDEXTABLE &pckFileIndex)
{

	while(1) {

		AcquireSRWLockExclusive(&g_mt_LockCompressedflag);
		size_t sizeOfFilesList = lpDataFetchMethod->lpFilesList->size();
		if(0 == sizeOfFilesList) {
			ReleaseSRWLockExclusive(&g_mt_LockCompressedflag);
			return FD_END;
		}

		//LPFILES_TO_COMPRESS		lpfirstFile = lpDataFetchMethod->lpFilesList;
		FILES_TO_COMPRESS cOneFile;
		memcpy(&cOneFile, &lpDataFetchMethod->lpFilesList->back(), sizeof(FILES_TO_COMPRESS));
		lpDataFetchMethod->lpFilesList->pop_back();
		//if(NULL == lpfirstFile->next) {
		//	ReleaseSRWLockExclusive(&g_mt_LockCompressedflag);
		//	return FD_END;
		//}
		//lpDataFetchMethod->lpFileToCompress = lpfirstFile->next;
		ReleaseSRWLockExclusive(&g_mt_LockCompressedflag);

#ifdef _DEBUG
		logOutput(__FUNCTION__, "lpfirstFile_id=%d\r\n", sizeOfFilesList);
#endif
		LPBYTE lpCompressedBuffer = (BYTE*)MALLOCED_EMPTY_DATA;
		pckFileIndex.dwMallocSize = pThis->GetCompressBoundSizeByFileSize(pckFileIndex.cFileIndex.dwFileClearTextSize, pckFileIndex.cFileIndex.dwFileCipherTextSize, cOneFile.dwFileSize);

		memcpy(mystrcpy(pckFileIndex.cFileIndex.szFilename, mt_szCurrentNodeString), cOneFile.lpszFileTitle, cOneFile.nBytesToCopy - mt_nCurrentNodeStringLen);

		//����ļ���СΪ0�����������ļ�����
		if(0 != pckFileIndex.cFileIndex.dwFileClearTextSize) {
			CMapViewFileRead		cFileRead;
			LPBYTE					lpBufferToRead;
			//�ļ���Ϊ0ʱ�Ĵ���
			//��Ҫ����ѹ�����ļ�
			if(NULL == (lpBufferToRead = cFileRead.OpenMappingAndViewAllRead(cOneFile.szFilename))) {
				return FD_ERR;
			}

			//�ж�ʹ�õ��ڴ��Ƿ񳬹����ֵ
			FETCHDATA_RET rtn;
			if(FD_OK != (rtn = pThis->detectMaxAndAddMemory(lpCompressedBuffer, pckFileIndex.dwMallocSize))) {
				return rtn;
			}

			if(PCK_BEGINCOMPRESS_SIZE < pckFileIndex.cFileIndex.dwFileClearTextSize) {
				pThis->compress(lpCompressedBuffer, &pckFileIndex.cFileIndex.dwFileCipherTextSize,
					lpBufferToRead, pckFileIndex.cFileIndex.dwFileClearTextSize);
			} else {
				memcpy(lpCompressedBuffer, lpBufferToRead, pckFileIndex.cFileIndex.dwFileClearTextSize);
			}

		}
		pckFileIndex.compressed_file_data = lpCompressedBuffer;
		return FD_OK;

	}
	return FD_END;
}

FETCHDATA_RET CPckClassThreadWorker::GetUncompressedDataFromPCK(CPckClassThreadWorker *pThis, LPDATA_FETCH_METHOD lpDataFetchMethod, PCKINDEXTABLE &pckFileIndex)
{

	while(1) {

		AcquireSRWLockExclusive(&g_mt_LockCompressedflag);
		if(lpDataFetchMethod->dwProcessIndex >= lpDataFetchMethod->dwtotalIndexCount) {
			ReleaseSRWLockExclusive(&g_mt_LockCompressedflag);
			return FD_END;
		}

		DATA_FETCH_METHOD cDataFetchMethod;
		memcpy(&cDataFetchMethod, lpDataFetchMethod, sizeof(DATA_FETCH_METHOD));
		++(lpDataFetchMethod->lpPckIndexTablePtrSrc);
		++(lpDataFetchMethod->dwProcessIndex);
		ReleaseSRWLockExclusive(&g_mt_LockCompressedflag);

#ifdef _DEBUG
		logOutput(__FUNCTION__, "dwProcessIndex=%d\r\n", cDataFetchMethod.dwProcessIndex);
#endif

		if(cDataFetchMethod.lpPckIndexTablePtrSrc->isInvalid)
			continue;

		LPBYTE				lpBufferToRead;
		//������ѹ�����ݵĽ�ѹ������
		LPBYTE				lpDecompressBuffer = NULL;
		//������ѹ�����ݵ�Դ����
		LPBYTE				lpSourceBuffer = NULL;

		LPPCKINDEXTABLE	lpPckIndexTablePtrSrc = cDataFetchMethod.lpPckIndexTablePtrSrc;
		DWORD dwNumberOfBytesToMap = lpPckIndexTablePtrSrc->cFileIndex.dwFileCipherTextSize;
		DWORD dwFileClearTextSize = lpPckIndexTablePtrSrc->cFileIndex.dwFileClearTextSize;

		memcpy(&pckFileIndex, &lpPckIndexTablePtrSrc->cFileIndex, sizeof(PCKFILEINDEX));

		if(PCK_BEGINCOMPRESS_SIZE < dwFileClearTextSize) {
			pckFileIndex.cFileIndex.dwFileCipherTextSize = pThis->compressBound(dwFileClearTextSize);
		}

		LPBYTE lpCompressedBuffer = (BYTE*)MALLOCED_EMPTY_DATA;
		pckFileIndex.dwMallocSize = pckFileIndex.cFileIndex.dwFileCipherTextSize;

		if(0 != dwFileClearTextSize) {

			//�ж�ʹ�õ��ڴ��Ƿ񳬹����ֵ
			FETCHDATA_RET rtn;
			if(FD_OK != (rtn = pThis->detectMaxAndAddMemory(lpCompressedBuffer, pckFileIndex.dwMallocSize))) {
				return rtn;
			}

			//�ļ�������Ҫ��ѹ��
			if(PCK_BEGINCOMPRESS_SIZE < dwFileClearTextSize) {
				//����Դ���ݵĿռ�
				if(FD_OK != (rtn = pThis->detectMaxAndAddMemory(lpSourceBuffer, dwNumberOfBytesToMap))) {
					return rtn;
				}

				if(FD_OK != (rtn = pThis->detectMaxAndAddMemory(lpDecompressBuffer, dwFileClearTextSize))) {
					return rtn;
				}


				AcquireSRWLockExclusive(&g_mt_LockReadFileMap);
				if(NULL == (lpBufferToRead = mt_lpFileRead->View(lpPckIndexTablePtrSrc->cFileIndex.dwAddressOffset, dwNumberOfBytesToMap))) {
					ReleaseSRWLockExclusive(&g_mt_LockReadFileMap);
					pThis->freeMaxAndSubtractMemory(lpSourceBuffer, dwNumberOfBytesToMap);
					pThis->freeMaxAndSubtractMemory(lpDecompressBuffer, dwFileClearTextSize);

					return FD_ERR;
					break;
				}

				memcpy(lpSourceBuffer, lpBufferToRead, dwNumberOfBytesToMap);
				mt_lpFileRead->UnmapView();
				ReleaseSRWLockExclusive(&g_mt_LockReadFileMap);

				pThis->decompress(lpDecompressBuffer, &dwFileClearTextSize, lpSourceBuffer, dwNumberOfBytesToMap);
				if(dwFileClearTextSize == lpPckIndexTablePtrSrc->cFileIndex.dwFileClearTextSize) {

					pThis->compress(lpCompressedBuffer, &pckFileIndex.cFileIndex.dwFileCipherTextSize, lpDecompressBuffer, dwFileClearTextSize);
				} else {
					memcpy(lpCompressedBuffer, lpSourceBuffer, dwNumberOfBytesToMap);
				}

				pThis->freeMaxAndSubtractMemory(lpSourceBuffer, dwNumberOfBytesToMap);
				pThis->freeMaxAndSubtractMemory(lpDecompressBuffer, dwFileClearTextSize);


			} else {
#pragma region �ļ���С����Ҫѹ��ʱ
				AcquireSRWLockExclusive(&g_mt_LockReadFileMap);
				if(NULL == (lpBufferToRead = mt_lpFileRead->View(lpPckIndexTablePtrSrc->cFileIndex.dwAddressOffset, dwNumberOfBytesToMap))) {
					ReleaseSRWLockExclusive(&g_mt_LockReadFileMap);
					return FD_ERR;
					break;
				}
				memcpy(lpCompressedBuffer, lpBufferToRead, dwNumberOfBytesToMap);
				mt_lpFileRead->UnmapView();
				ReleaseSRWLockExclusive(&g_mt_LockReadFileMap);
#pragma endregion
			}

		}

		pckFileIndex.compressed_file_data = lpCompressedBuffer;
		return FD_OK;

	}
	return FD_END;
}
