#include "CriticalSectionDlg.h"

CriticalSectionDlg* CriticalSectionDlg::ptr = NULL;

CRITICAL_SECTION cs;
HWND hEditText, hEditCount;
TCHAR buf[10];

CriticalSectionDlg::CriticalSectionDlg(void)
{
	ptr = this;
}

CriticalSectionDlg::~CriticalSectionDlg(void)
{
	DeleteCriticalSection(&cs);
}

void CriticalSectionDlg::Cls_OnClose(HWND hwnd)
{
	EndDialog(hwnd, 0);
}

BOOL CriticalSectionDlg::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	hEditCount = GetDlgItem(hwnd, IDC_EDIT2);
	InitializeCriticalSection(&cs);
	return TRUE;
}

DWORD WINAPI WriteToFiles(LPVOID lp)
{
	EnterCriticalSection(&cs);
	GetWindowText(hEditCount, buf, 10);
	int count = _tstoi(buf);
	ofstream out(TEXT("array.txt"));
	if (!out.is_open())
	{
		MessageBox(0, TEXT("Error"), TEXT("Критическая секция"), MB_OK);
		return 1;
	}
	for (int i = 1; i <= 5; i++)
	{
		out << i << ' ';
	}
	out << ' ';
	out.close();
	for (int i = 0; i < count; i++)
	{
		wsprintf(buf, TEXT("array_copy_%d.txt"), i + 1);
		out.open(buf);
		for (int j = 1; j <= 5; j++)
		{
			out << j << ' ';
		}
		out << ' ';
	}
	LeaveCriticalSection(&cs);
	MessageBox(0, TEXT("Поток записал информацию в файл"), TEXT("Критическая секция"), MB_OK);
	return 0;
}

DWORD WINAPI ReadToFiles(LPVOID lp)
{
	EnterCriticalSection(&cs);
	GetWindowText(hEditCount, buf, 10);
	int count = _tstoi(buf);
	ofstream in(TEXT("result_file.txt"));
	ifstream out(TEXT("array.txt"));
	if (!in.is_open())
	{
		MessageBox(0, TEXT("Error"), TEXT("Error"), MB_OK);
		return 1;
	}
	char B[10];
	for (int i = 0; i < 5; i++)
	{
		out >> B[i];
		in << B[i] << ' ';
	}
	out.close();
	for (int i = 0; i < count; i++)
	{
		in << '\t';
		wsprintf(buf, TEXT("array_copy_%d.txt"), i + 1);
		out.open(buf);
		for (int j = 0; j < 5; j++)
		{
			out >> B[j];
			in << B[j] << ' ';
		}
	}
	in.close();
	LeaveCriticalSection(&cs);
	MessageBox(0, TEXT("Поток прочитал информацию из файла"), TEXT("Критическая секция"), MB_OK);
	return 0;
}


void CriticalSectionDlg::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	if (id == IDC_BUTTON1)
	{
		HANDLE hThread = CreateThread(NULL, 0, WriteToFiles, 0, 0, NULL);
		CloseHandle(hThread);
		hThread = CreateThread(NULL, 0, ReadToFiles, 0, 0, NULL);
		CloseHandle(hThread);
	}
}

BOOL CALLBACK CriticalSectionDlg::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
		HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, ptr->Cls_OnCommand);
	}
	return FALSE;
}