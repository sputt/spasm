#include "stdafx.h"

#define _ERRORS_CPP
#include "errors.h"

#include "spasm.h"
#include "console.h"
#include "list.h"

typedef struct tagERRORINSTANCE
{
	LPSTR lpszFileName;
	int line_num;					//-1 for no line
	DWORD dwErrorCode;
	int nSession;
	bool fSuppressErrors;
	bool fIsWarning;
	int nPrintSession;
	LPTSTR lpszErrorText;
	LPTSTR lpszAnnotation;
} ERRORINSTANCE, *LPERRORINSTANCE;

typedef struct _errorlist {
	LPERRORINSTANCE data;
	struct _errorlist *next;
} errorlist_t;

static errorlist_t *g_ErrorList2;
static int g_nErrorSession = 0;

#define g_ErrorList g_ErrorList2

static void PrintSPASMError(const LPERRORINSTANCE lpError)
{
	assert(lpError != NULL);
	if ((lpError->dwErrorCode != SPASM_ERR_SUCCESS) || (lpError->lpszErrorText != NULL))
	{
		save_console_attributes();
		set_console_attributes(lpError->fIsWarning ? COLOR_YELLOW : COLOR_RED);
		if (lpError->lpszAnnotation != NULL)
		{
			printf("%s\n", lpError->lpszAnnotation);
		}

		printf("%s\n", lpError->lpszErrorText);
#ifdef WINVER
		OutputDebugString(lpError->lpszErrorText);
		OutputDebugString(_T("\n"));
#endif
		restore_console_attributes();
	}
}

static LPERRORINSTANCE AllocErrorInstance()
{
	LPERRORINSTANCE lpErr = (LPERRORINSTANCE) malloc(sizeof(ERRORINSTANCE));
	lpErr->dwErrorCode = SPASM_ERR_SUCCESS;
	lpErr->line_num = -1;
	lpErr->lpszFileName = NULL;
	lpErr->fSuppressErrors = suppress_errors;
	lpErr->nSession = g_nErrorSession;
	lpErr->nPrintSession = -1;
	lpErr->lpszErrorText = NULL;
	lpErr->lpszAnnotation = NULL;
	return lpErr;
}

static void FreeErrorInstance(LPERRORINSTANCE lpErr)
{
	if (lpErr->lpszErrorText != NULL)
	{
		free(lpErr->lpszErrorText);
	}
	free(lpErr);
}

int StartSPASMErrorSession(void)
{
	LPERRORINSTANCE lpLastInstance = (LPERRORINSTANCE) g_ErrorList->data;
	if (lpLastInstance->nSession == -1)
	{
		lpLastInstance->nSession = g_nErrorSession;
		lpLastInstance->fSuppressErrors = suppress_errors;
	}
	else
	{
		LPERRORINSTANCE lpErr = AllocErrorInstance();
		g_ErrorList = (errorlist_t *) list_prepend((list_t *) g_ErrorList, lpErr);
	}
	suppress_errors = true;
	return g_nErrorSession++;
}

int GetSPASMErrorSessionErrorCount(int nSession)
{
	int nCount = 0;
	list_t *pList = (list_t *) g_ErrorList;
	while (((LPERRORINSTANCE) pList->data)->nSession != nSession)
	{
		if (((LPERRORINSTANCE) pList->data)->dwErrorCode != SPASM_ERR_SUCCESS)
		{
			nCount++;
		}
		pList = pList->next;
	}
	assert(pList != NULL);
	return nCount;
}

bool IsSPASMErrorSessionFatal(int nSession)
{
	bool fIsFatal = false;
	list_t *pList = (list_t *) g_ErrorList;
	while (((LPERRORINSTANCE) pList->data)->nSession != nSession)
	{
		DWORD dwError = ((LPERRORINSTANCE) pList->data)->dwErrorCode;
		if (!(dwError == SPASM_ERR_LOCAL_LABEL_FORWARD_REF ||
			  dwError == SPASM_ERR_LABEL_NOT_FOUND ||
			  dwError == SPASM_ERR_INDEX_OFFSET_EXCEEDED ||
			  dwError == SPASM_ERR_SUCCESS))
		{
			fIsFatal = true;
			break;
		}
		pList = pList->next;
		assert(pList != NULL);
	}
	return fIsFatal;
}

static void ReplayErrorRecursive(const list_t *pList, int nSession)
{
	if (((LPERRORINSTANCE) pList->data)->nSession == nSession)
		return;

	ReplayErrorRecursive(pList->next, nSession);
	PrintSPASMError((LPERRORINSTANCE) pList->data);
}

void ReplaySPASMErrorSession(int nSession)
{
	list_t *pList = (list_t *) g_ErrorList;
	while (((LPERRORINSTANCE) pList->data)->nSession != nSession)
	{
		((LPERRORINSTANCE) pList->data)->nPrintSession = nSession;
		pList = pList->next;
		assert(pList != NULL);
	}

	if (((LPERRORINSTANCE) pList->data)->fSuppressErrors == true)
	{
		return;
	}

	ReplayErrorRecursive((list_t *) g_ErrorList, nSession);
}

bool IsErrorInSPASMErrorSession(int nSession, DWORD dwErrorCode)
{
	list_t *pList = (list_t *) g_ErrorList;
	while (((LPERRORINSTANCE) pList->data)->nSession != nSession)
	{
		LPERRORINSTANCE lpError = (LPERRORINSTANCE) pList->data;
		if (lpError->dwErrorCode == dwErrorCode)
		{
			return true;
		}
		pList = pList->next;
		assert(pList != NULL);
	}
	return false;
}

void AddSPASMErrorSessionAnnotation(int nSession, LPCTSTR lpszFormat, ...)
{
	va_list valist;
	va_start(valist, lpszFormat);

	TCHAR szBuffer[256];
	TCHAR szDescription[128] = _T("An error occurred");

	StringCchVPrintf(szDescription, ARRAYSIZE(szDescription), lpszFormat, valist);
	StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), _T("%s:%d: %s"),
		curr_input_file, line_num, szDescription);

	va_end(valist);

	list_t *pList = (list_t *) g_ErrorList;
	while (pList != NULL)
	{
		LPERRORINSTANCE lpErr = (LPERRORINSTANCE) pList->data;
		if (lpErr->nSession >= nSession)
		{
			lpErr->lpszAnnotation = _tcsdup(szBuffer);
		}
		pList = pList->next;
	}
}

void EndSPASMErrorSession(int nSession)
{
	list_t *pList = (list_t *) g_ErrorList;
	
	list_t *pPrev = NULL;
	while (((LPERRORINSTANCE) pList->data)->nSession != nSession)
	{
		LPERRORINSTANCE lpErr = (LPERRORINSTANCE) pList->data;
		if (lpErr->nPrintSession > nSession || lpErr->nPrintSession == -1)
		{
		   if (pPrev == NULL)
		   {
			   g_ErrorList = (errorlist_t *) pList->next;
		   }
		   else
		   {
			   pPrev->next = pList->next;
		   }
		}
		else
		{
			pPrev = pList;
		}
		
		pList = pList->next;
		assert(pList != NULL);
	}
	((LPERRORINSTANCE) pList->data)->nSession = -1;
	suppress_errors = ((LPERRORINSTANCE) pList->data)->fSuppressErrors;
}


void ClearSPASMErrorSessions(void)
{
	list_free((list_t *) g_ErrorList, true);
	g_nErrorSession = 0;
	suppress_errors = false;

	LPERRORINSTANCE lpErr = (LPERRORINSTANCE) malloc(sizeof(ERRORINSTANCE));
	lpErr->dwErrorCode = SPASM_ERR_SUCCESS;
	lpErr->line_num = -1;
	lpErr->lpszFileName = NULL;
	lpErr->fSuppressErrors = false;
	lpErr->nSession = -1;
	lpErr->nPrintSession = -1;

	g_ErrorList = (errorlist_t *) list_prepend(NULL, lpErr);
}

#ifdef _TEST
DWORD GetLastSPASMError()
{
	list_t *pList = (list_t *) g_ErrorList;
	while (pList != NULL)
	{
		LPERRORINSTANCE lpError = (LPERRORINSTANCE) pList->data;
		if (lpError->dwErrorCode != SPASM_ERR_SUCCESS)
		{
			return lpError->dwErrorCode;
		}
		pList = pList->next;
	}
	return SPASM_ERR_SUCCESS;
}

int GetLastSPASMErrorLine()
{
	list_t *pList = (list_t *) g_ErrorList;
	while (pList != NULL)
	{
		LPERRORINSTANCE lpError = (LPERRORINSTANCE) pList->data;
		if (lpError->dwErrorCode != SPASM_ERR_SUCCESS)
		{
			return lpError->line_num;
		}
		pList = pList->next;
	}
	return SPASM_ERR_SUCCESS;
}
#endif


static void SetLastSPASMProblem(DWORD dwErrorCode, bool fIsWarning, va_list valist)
{
	if (dwErrorCode == SPASM_ERR_SUCCESS)
	{
		return;
	}

	LPERRORINSTANCE lpErr = AllocErrorInstance();
	lpErr->dwErrorCode = dwErrorCode;
	lpErr->line_num = line_num;
	lpErr->lpszFileName = _strdup(curr_input_file);
	lpErr->fSuppressErrors = suppress_errors;
	lpErr->fIsWarning = fIsWarning;
	
	TCHAR szBuffer[256];
	TCHAR szDescription[128] = _T("An error occurred");

	for (int i = 0; i < ARRAYSIZE(g_ErrorCodes); i++)
	{
		if (g_ErrorCodes[i].dwCode == lpErr->dwErrorCode)
		{
			StringCchVPrintf(szDescription, ARRAYSIZE(szDescription),
				g_ErrorCodes[i].lpszDescription, valist);
			break;
		}
	}

	LPCTSTR lpszProblemType = (fIsWarning) ? _T("warning") : _T("error");

	if (lpErr->line_num != -1)
	{
		StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), _T("%s:%d: %s SE%03X: %s"),
			lpErr->lpszFileName, lpErr->line_num, lpszProblemType, lpErr->dwErrorCode, szDescription);
	}
	else
	{
		StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), _T("%s: %s SE%03X: %s"),
			lpErr->lpszFileName, lpszProblemType, lpErr->dwErrorCode, szDescription);
	}

	lpErr->lpszErrorText = _strdup(szBuffer);

	g_ErrorList = (errorlist_t *) list_prepend((list_t *) g_ErrorList, (LPVOID) lpErr);

	if (suppress_errors == false)
	{
		PrintSPASMError(lpErr);
	}
}

void SetLastSPASMWarning(DWORD dwErrorCode, ...)
{
	va_list valist;
	va_start(valist, dwErrorCode);

	SetLastSPASMProblem(dwErrorCode, true, valist);

	va_end(valist);
}

void SetLastSPASMError(DWORD dwErrorCode, ...)
{
	va_list valist;
	va_start(valist, dwErrorCode);

	SetLastSPASMProblem(dwErrorCode, false, valist);

	va_end(valist);
}
