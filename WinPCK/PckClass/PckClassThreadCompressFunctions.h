VOID CPckClass::COMPRESSTHREADFUNC(VOID* pParam)
{
	CPckClass *pThis = (CPckClass*)pParam;

	//char	szFileMappingNameSpaceFormat[16];
	//char	szFileMappingNameSpace[32];
	int		level = pThis->m_lpPckParams->dwCompressLevel;

	//memcpy(szFileMappingNameSpaceFormat, pThis->m_szMapNameRead, 16);
	//strcat_s(szFileMappingNameSpaceFormat, 16, "%d");

	EnterCriticalSection(&g_mt_threadID);
	//sprintf_s(szFileMappingNameSpace, 32, szFileMappingNameSpaceFormat, mt_threadID++);
	mt_threadID++;
	LeaveCriticalSection(&g_mt_threadID);

	BYTE	*bufCompressData = (BYTE*)1;

	LPBYTE				lpBufferToRead;
	PCKFILEINDEX		pckFileIndex;

	//��ʼ
	LPFILES_TO_COMPRESS			lpfirstFile = pThis->m_firstFile;
	PCKINDEXTABLE_COMPRESS		cPckIndexTableComp;

	memset(pckFileIndex.szFilename, 0, MAX_PATH_PCK_260);

	//patch 140424
	CMapViewFileRead	*lpFileRead = new CMapViewFileRead();

	while (NULL != lpfirstFile->next)
	{

#pragma region CriticalSectionCompressedflag
#ifdef _DEBUG
		assert(0 == lpfirstFile->dwCompressedflag || 1 == lpfirstFile->dwCompressedflag);
#endif

		EnterCriticalSection(&g_dwCompressedflag);

		if (0 != lpfirstFile->dwCompressedflag) {
			LeaveCriticalSection(&g_dwCompressedflag);
			lpfirstFile = lpfirstFile->next;
			continue;
		}

		lpfirstFile->dwCompressedflag = 1;

		LeaveCriticalSection(&g_dwCompressedflag);
#pragma endregion

		DWORD dwMallocSize = pThis->GetCompressBoundSizeByFileSize(&pckFileIndex, lpfirstFile->dwFileSize);

		//�ж�ʹ�õ��ڴ��Ƿ񳬹����ֵ
		pThis->detectMaxAndAddMemory(dwMallocSize);

#if TARGET_PCK_MODE_COMPRESS == PCK_MODE_COMPRESS_CREATE
			memcpy(pckFileIndex.szFilename, lpfirstFile->lpszFileTitle, lpfirstFile->nBytesToCopy);
#elif TARGET_PCK_MODE_COMPRESS == PCK_MODE_COMPRESS_ADD
			memcpy(mystrcpy(pckFileIndex.szFilename, mt_szCurrentNodeString), lpfirstFile->lpszFileTitle, lpfirstFile->nBytesToCopy - mt_nCurrentNodeStringLen);
#endif
		if (!*(mt_lpbThreadRunning))
			break;

		//����ļ���СΪ0�����������ļ�����
		if (0 != pckFileIndex.dwFileClearTextSize)
		{
			//�ļ���Ϊ0ʱ�Ĵ���
			//��Ҫ����ѹ�����ļ�
			if (NULL == (lpBufferToRead = lpFileRead->OpenMappingAndViewAllRead(lpfirstFile->szFilename)))
			{
				*(mt_lpbThreadRunning) = FALSE;
				lpFileRead->clear();
				break;
			}

			if (NULL == (bufCompressData = (BYTE*)malloc(dwMallocSize))) {
				*(mt_lpbThreadRunning) = FALSE;
				lpFileRead->clear();
				break;
			}

			if (PCK_BEGINCOMPRESS_SIZE < pckFileIndex.dwFileClearTextSize)
			{
				pThis->compress(bufCompressData, &pckFileIndex.dwFileCipherTextSize,
					lpBufferToRead, pckFileIndex.dwFileClearTextSize, level);
			}
			else {
				memcpy(bufCompressData, lpBufferToRead, pckFileIndex.dwFileClearTextSize);
			}

			lpFileRead->clear();

		}
		else {
			bufCompressData = (BYTE*)1;
		}

		memset(&cPckIndexTableComp, 0, sizeof(PCKINDEXTABLE_COMPRESS));

		EnterCriticalSection(&g_cs);

		//����������ʾ���ļ�����
		(*mt_lpdwCount)++;

#if TARGET_PCK_MODE_COMPRESS == PCK_MODE_COMPRESS_ADD
			//���ظ����ļ�
			if (NULL != lpfirstFile->samePtr)
			{
				//�ظ����ļ�����д����ļ���СС�ڱ����ǵ��ļ���С��ʹ����������
				if (pckFileIndex.dwFileCipherTextSize <= lpfirstFile->samePtr->cFileIndex.dwFileCipherTextSize)
				{
					cPckIndexTableComp.dwAddressOfDuplicateOldDataArea = lpfirstFile->samePtr->cFileIndex.dwAddressOffset;
				}

				lpfirstFile->samePtr->cFileIndex.dwFileCipherTextSize = pckFileIndex.dwFileCipherTextSize;
				lpfirstFile->samePtr->cFileIndex.dwFileClearTextSize = pckFileIndex.dwFileClearTextSize;

				cPckIndexTableComp.bInvalid = TRUE;

			}
#endif

		LeaveCriticalSection(&g_cs);

		cPckIndexTableComp.dwMallocSize = dwMallocSize;

#ifdef _DEBUG
		if (1 == (int)bufCompressData) {
			logOutput(__FUNCTION__, formatString("file��bufCompressData(0x%08x):0, \t%s\r\n", (int)bufCompressData, lpfirstFile->szFilename));
		}
		else {
			logOutput(__FUNCTION__, formatString("file��bufCompressData(0x%08x):0x%016llx, \t%s\r\n", (int)bufCompressData, _byteswap_uint64(*(unsigned __int64*)bufCompressData), lpfirstFile->szFilename));
		}
#endif

#if TARGET_PCK_MODE_COMPRESS == PCK_MODE_COMPRESS_CREATE

		pThis->putCompressedDataQueue(bufCompressData, &cPckIndexTableComp, &pckFileIndex);

#elif TARGET_PCK_MODE_COMPRESS == PCK_MODE_COMPRESS_ADD
		if (!cPckIndexTableComp.bInvalid)
		{
			//�������
			pThis->putCompressedDataQueue(bufCompressData, &cPckIndexTableComp, &pckFileIndex);
		}
		else {

			//�������
			pThis->putCompressedDataQueue(bufCompressData, &cPckIndexTableComp, &lpfirstFile->samePtr->cFileIndex);
		}
#endif
		//��һ���ļ��б�
		lpfirstFile = lpfirstFile->next;
	}

	delete lpFileRead;

	EnterCriticalSection(&g_mt_threadID);

	mt_threadID--;
	if (0 == mt_threadID) {
		pThis->m_PckLog.PrintLogN(TEXT_LOG_FLUSH_CACHE);
		SetEvent(mt_evtAllCompressFinish);
	}

	LeaveCriticalSection(&g_mt_threadID);

	return;
}

#undef COMPRESSTHREADFUNC
#undef COMPRESSSINGLETHREADFUNC
#undef TARGET_PCK_MODE_COMPRESS