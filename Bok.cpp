// Bok.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Bok.h"

#define MAX_LOADSTRING 100
#define NI_CALLBACK WM_USER+101

// Global Variables:
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HINSTANCE hInst;								// current instance

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
HWND				MyCreateWindow(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

#define USE_LL 1
LRESULT _stdcall BokDllHookLL(int code, WPARAM wParam, LPARAM lParam);
void MyCreateTray(HWND wnd);
void MyDestroyTray(HWND wnd);
HICON iconok = NULL;
extern BOOL lock;
extern BOOL lock_enter;
extern BOOL lock_neg;

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	
	iconok = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OK32));

	if(iconok == NULL) return 1;
	HHOOK hook = NULL;
#if 1
	PROC proc = (PROC)BokDllHookLL;
	hook = SetWindowsHookEx(WH_KEYBOARD_LL,(HOOKPROC)proc,NULL,NULL);
	if(hook==NULL) return 1;
#endif

	//if(hook!=NULL) UnhookWindowsHookEx(hook); hook=NULL;
	
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	hInst = hInstance;
	HWND hWnd = MyCreateWindow(hInstance, nCmdShow);
	if (hWnd == NULL) return 1;
	MyCreateTray(hWnd);

   //Never show the Window
   //ShowWindow(hWnd, nCmdShow);
   //UpdateWindow(hWnd);

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BOK));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	if(hook) UnhookWindowsHookEx(hook);
	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= 0;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;//default
	wcex.hCursor		= NULL;//whatever
	wcex.hbrBackground	= NULL;//whatever
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
HWND MyCreateWindow(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_BOK, szWindowClass, MAX_LOADSTRING);

	// Initialize global strings
	MyRegisterClass(hInstance);

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return NULL;
   }

   return hWnd;
}

void MyCreateTray(HWND wnd) {
   NOTIFYICONDATA nd;
   memset(&nd, 0, sizeof(nd));
   nd.cbSize = sizeof(NOTIFYICONDATA);
   nd.hWnd = wnd;
   nd.uID = 0;
   nd.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP;
   nd.uCallbackMessage = NI_CALLBACK;
   wcscpy_s(nd.szTip,_T("Book on Keyboard"));
   nd.hIcon = iconok;
   Shell_NotifyIcon(NIM_ADD, &nd);
}

void MyDestroyTray(HWND wnd) {
   NOTIFYICONDATA nd;
   memset(&nd, 0, sizeof(nd));
   nd.cbSize = sizeof(NOTIFYICONDATA);
   nd.hWnd = wnd;
   nd.uID = 0;
   nd.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP;
   nd.uCallbackMessage = NI_CALLBACK;
   wcscpy_s(nd.szTip,_T("Book on Keyboard"));
   nd.hIcon = iconok;
   Shell_NotifyIcon(NIM_DELETE, &nd);
}


void OnRightClick(HWND hWnd) {
	HMENU menu = GetSubMenu(LoadMenu(hInst, MAKEINTRESOURCE(IDC_BOK)),0);
	POINT cursor;
	CheckMenuItem(menu, 1, MF_BYPOSITION|(lock_enter?MF_CHECKED:MF_UNCHECKED));	
	CheckMenuItem(menu, 2, MF_BYPOSITION|(lock_neg?MF_CHECKED:MF_UNCHECKED));	
	EnableMenuItem(menu, 1, MF_BYPOSITION|(!lock?MF_GRAYED:MF_ENABLED));
	EnableMenuItem(menu, 2, MF_BYPOSITION|(!lock?MF_GRAYED:MF_ENABLED));
	CheckMenuItem(menu, 0, MF_BYPOSITION|(lock?MF_CHECKED:MF_UNCHECKED));
	//Avoding popup menu doesn't disappear! It works and I don't know why!
	SetForegroundWindow(hWnd);
	GetCursorPos(&cursor);
	TrackPopupMenu(menu, TPM_LEFTALIGN, cursor.x, cursor.y,
		0, hWnd, NULL);
	DestroyMenu(menu);
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case NI_CALLBACK:
		if(wParam != 0) break;
		switch (lParam)
		{
		case WM_RBUTTONUP:
			OnRightClick(hWnd);
			break;
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			//DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), NULL, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_BLOCK:
			lock = !lock;
			break;
		case IDM_BLOCKENTER:
			lock_enter = !lock_enter;
			break;
		case IDM_BLOCKNEG:
			lock_neg = !lock_neg;
			break;
		case IDM_HELPCONT:
			ShellExecute(NULL, _T("open"), _T("help.html"), NULL, NULL, NULL);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_DESTROY:
		MyDestroyTray(hWnd);
		PostQuitMessage(0);
		break;
	//case WM_PAINT:
		//hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		//EndPaint(hWnd, &ps);
		//break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
