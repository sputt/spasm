#include "stdafx.h"

#define TEXT_STREAM_C
#include "CTextStream.h"

#define MAX_PIPE_SIZE (1024 * 1024)

HRESULT CTextStream::FinalConstruct()
{
	HANDLE hRead, hWrite;
	BOOL fResult = CreatePipe(&hRead, &hWrite, NULL, MAX_PIPE_SIZE);
	if (fResult)
	{
		DWORD dwMode = PIPE_READMODE_BYTE; // | PIPE_NOWAIT;
		fResult = SetNamedPipeHandleState(hRead, &dwMode, NULL, NULL);
		m_hRead = hRead;

		g_hStdOut = hWrite;

	}
	return S_OK;
}


STDMETHODIMP CTextStream::get_AtEndOfLine(VARIANT_BOOL *bEOL)
{
	return E_NOTIMPL;
}

STDMETHODIMP CTextStream::get_AtEndOfStream(VARIANT_BOOL *bEOS)
{
	fflush(stdout);

	DWORD dwAvail;
	BOOL fResult = PeekNamedPipe(m_hRead, NULL, 0, NULL, &dwAvail, NULL);
	*bEOS = (dwAvail == 0) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CTextStream::Read(LONG cch, LPBSTR lpbstrResult)
{
	DWORD dwAvail;
	BOOL fResult = PeekNamedPipe(m_hRead, NULL, 0, NULL, &dwAvail, NULL);
	
	if (dwAvail == 0)
	{
		*lpbstrResult = SysAllocString(L"");
		return S_FALSE;
	}

	DWORD dwRead;
	CComHeapPtr<TCHAR> szText;
	szText.Allocate(dwAvail + sizeof(TCHAR));
	fResult = ReadFile(m_hRead, szText, dwAvail, &dwRead, NULL);
	if (fResult)
	{
		szText[dwRead / sizeof(TCHAR)] = _T('\0');
		*lpbstrResult = SysAllocString(_bstr_t(szText));
		return S_OK;
	}
	return E_FAIL;
}

STDMETHODIMP CTextStream::ReadLine(LPBSTR lpbstrLine)
{
	fflush(stdout);

	_bstr_t result;

	TCHAR ch[2] = {_T('\0')};
	DWORD dwRead;
	BOOL fResult;
	do
	{
		fResult = ReadFile(m_hRead, ch, sizeof(TCHAR), &dwRead, NULL);
		if (fResult && dwRead == sizeof(TCHAR) && ch[0] != _T('\r') && ch[0] != _T('\n'))
		{
			result += ch;
		}
	}
	while (ch[0] != _T('\n') && fResult && dwRead > 0);

	*lpbstrLine = SysAllocString(result);
	return S_OK;
}

STDMETHODIMP CTextStream::ReadAll(LPBSTR lpbstrResult)
{
	return Read(LONG_MAX, lpbstrResult);
}

STDMETHODIMP CTextStream::Write(BSTR bstrText)
{
	return E_NOTIMPL;
}

STDMETHODIMP CTextStream::WriteLine(BSTR bstrLine)
{
	return E_NOTIMPL;
}

STDMETHODIMP CTextStream::WriteBlankLines(LONG nLines)
{
	return E_NOTIMPL;
}

STDMETHODIMP CTextStream::Skip(LONG cch)
{
	BSTR bstrLine = NULL;
	HRESULT hr = Read(cch, &bstrLine);
	return hr;
}

STDMETHODIMP CTextStream::SkipLine()
{
	BSTR bstrLine = NULL;
	HRESULT hr = ReadLine(&bstrLine);
	return hr;
}
STDMETHODIMP CTextStream::Close()
{
	return S_FALSE;
}
