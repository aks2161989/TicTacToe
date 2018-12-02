// TicTacToe.cpp : Defines the entry point for the application.
//


#include "stdafx.h"
#include "TicTacToe.h"
#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")

#define MAX_LOADSTRING 100
#define IDM_NEW 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND g_hWndToolbar;
HWND hWndComboBox1, hWndComboBox2;
HWND btn_hwnd[9];
HWND g_hWndStatusbar;

//Constants
const int btn_id_0 = 0,
btn_id_1 = 1,
btn_id_2 = 2,
btn_id_3 = 3,
btn_id_4 = 4,
btn_id_5 = 5,
btn_id_6 = 6,
btn_id_7 = 7,
btn_id_8 = 8;


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// Toolbar code below...
HIMAGELIST g_hImageList = NULL;

HWND CreateSimpleToolbar(HWND hWndParent)
{
	// Declare and initialize local constants.
	const int ImageListID = 0;
	const int numButtons = 1;
	const int bitmapSize = 16;

	const DWORD buttonStyles = BTNS_BUTTON;

	// Create the toolbar.
	HWND hWndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
		WS_CHILD | TBSTYLE_WRAPABLE | TBSTYLE_EX_DRAWDDARROWS, 0, 0, 0, 0,
		hWndParent, NULL, hInst, NULL);

	if (hWndToolbar == NULL)
		return NULL;

	// Create the image list.
	g_hImageList = ImageList_Create(bitmapSize, bitmapSize,
		ILC_COLOR16 | ILC_MASK,
		numButtons, 0);

	// Set the image list.
	SendMessage(hWndToolbar, TB_SETIMAGELIST,
		(WPARAM)ImageListID,
		(LPARAM)g_hImageList);

	// Load the button images.
	SendMessage(hWndToolbar, TB_LOADIMAGES,
		(WPARAM)IDB_STD_SMALL_COLOR,
		(LPARAM)HINST_COMMCTRL);

	// Initialize button info.
	// IDM_NEW, IDM_OPEN, and IDM_SAVE are application-defined command constants.

	TBBUTTON tbButtons[numButtons] =
	{
		{
			MAKELONG(STD_FILENEW, ImageListID), IDM_NEW, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)L"New"
		}
	};

	// Add buttons.
	SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessage(hWndToolbar, TB_ADDBUTTONS, (WPARAM)numButtons, (LPARAM)&tbButtons);

	// Drop-down list code below
	// CREATE first COMBO BOX
	hWndComboBox1 = CreateWindow(
		WC_COMBOBOX,
		TEXT(""),
		CBS_DROPDOWN | CBS_HASSTRINGS | WS_VISIBLE | WS_CHILD | WS_BORDER,
		40, 5, 100, 100,
		hWndToolbar, NULL, NULL, NULL);

	// ADD 3 ITEMS
	SendMessage(
		hWndComboBox1,
		(UINT)CB_ADDSTRING,
		(WPARAM)0, (LPARAM)TEXT("Easy"));
	SendMessage(
		hWndComboBox1,
		(UINT)CB_ADDSTRING,
		(WPARAM)0, (LPARAM)TEXT("Medium"));
	SendMessage(
		hWndComboBox1,
		(UINT)CB_ADDSTRING,
		(WPARAM)0, (LPARAM)TEXT("Impossible"));

	// Create combo box 2
	hWndComboBox2 = CreateWindow(
		WC_COMBOBOX,
		TEXT(""),
		CBS_DROPDOWN | CBS_HASSTRINGS | WS_VISIBLE | WS_CHILD | WS_BORDER,
		147, 5, 45, 100,
		hWndToolbar, NULL, NULL, NULL);

	// ADD 2 ITEMS
	SendMessage(
		hWndComboBox2,
		(UINT)CB_ADDSTRING,
		(WPARAM)0, (LPARAM)TEXT("X"));
	SendMessage(
		hWndComboBox2,
		(UINT)CB_ADDSTRING,
		(WPARAM)0, (LPARAM)TEXT("O"));
	//Drop-down list code ends

	// Resize the toolbar, and then show it.
	SendMessage(hWndToolbar, TB_AUTOSIZE | TB_SETEXTENDEDSTYLE, 0, 0);
	ShowWindow(hWndToolbar, TRUE);

	return	hWndToolbar;
}
// Toolbar code ends.

// Statusbar code below
// Description:
// Creates a status bar and divides it into the specified number of parts.
// Parameters:
//	hwndParent - parent window for the status bar.
//	idStatus - child window identifier of the status bar.
//	hinst - handle to the application instance.
//	cParts - number of parts into which to divide the status bar.
// Returns:
//	The handle to the status bar.
//
HWND DoCreateStatusBar(HWND hwndParent, int idStatus, HINSTANCE hinst, int cParts)
{
	HWND hwndStatus;
	RECT rcClient;
	HLOCAL hloc;
	PINT paParts;
	int i, nWidth;

	// Ensure that the common control DLL is loaded.
	InitCommonControls();

	// Create the status bar.
	hwndStatus = CreateWindowEx(
		0,							// no extended styles
		STATUSCLASSNAME,			// name of status bar class
		L"Idle",				// no text when first created
		SBARS_SIZEGRIP |			// includes a sizing grip
		WS_CHILD | WS_VISIBLE,		// creates a visible child window
		0, 0, 0, 0,					// ignores size and position
		hwndParent,					// handle to parent window
		(HMENU)idStatus,			// child window identifier
		hinst,						// handle to application instance
		NULL);						// no window creation data

	// Get the coordinates of the parent window's client area.
	GetClientRect(hwndParent, &rcClient);

	// Allocate an array for holding the right edge coordinates.
	hloc = LocalAlloc(LHND, sizeof(int) * cParts);
	paParts = (PINT)LocalLock(hloc);

	// Calculate the right edge coordinate for each part, and 
	// copy the coordinates to the array.
	nWidth = rcClient.right / cParts;
	int rightEdge = nWidth;
	for (i = 0; i < cParts; i++) {
		paParts[i] = rightEdge;
		rightEdge += nWidth;
	}

	// Tell the status bar to create the window parts.
	SendMessage(hwndStatus, SB_SETPARTS, (WPARAM)cParts, (LPARAM)
		paParts);

	// Free the array, and return.
	LocalUnlock(hloc);
	LocalFree(hloc);
	return hwndStatus;
}
// Statusbar code ends

// Set font
bool CALLBACK SetFont(HWND child, LPARAM font)
{
	SendMessage(child, WM_SETFONT, font, true);
	return true;
}
// Set font code ends here

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_TICTACTOE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TICTACTOE));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TICTACTOE));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TICTACTOE);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
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
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED |WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT, 0, 218, 325, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_CREATE:
		g_hWndToolbar = CreateSimpleToolbar(hWnd);

		btn_hwnd[0] = CreateWindowA("Button", "O", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 10, 55, 60, 60, hWnd, (HMENU)btn_id_0, hInst, NULL);
		btn_hwnd[1] = CreateWindowA("Button", "1", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 70, 55, 60, 60, hWnd, (HMENU)btn_id_1, hInst, NULL);
		btn_hwnd[2] = CreateWindowA("Button", "2", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 130, 55, 60, 60, hWnd, (HMENU)btn_id_2, hInst, NULL);
		btn_hwnd[3] = CreateWindowA("Button", "3", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 10, 115, 60, 60, hWnd, (HMENU)btn_id_3, hInst, NULL);
		btn_hwnd[4] = CreateWindowA("Button", "4", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 70, 115, 60, 60, hWnd, (HMENU)btn_id_4, hInst, NULL);
		btn_hwnd[5] = CreateWindowA("Button", "5", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 130, 115, 60, 60, hWnd, (HMENU)btn_id_5, hInst, NULL);
		btn_hwnd[6] = CreateWindowA("Button", "6", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 10, 175, 60, 60, hWnd, (HMENU)btn_id_6, hInst, NULL);
		btn_hwnd[7] = CreateWindowA("Button", "7", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 70, 175, 60, 60, hWnd, (HMENU)btn_id_7, hInst, NULL);
		btn_hwnd[8] = CreateWindowA("Button", "X", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 130, 175, 60, 60, hWnd, (HMENU)btn_id_8, hInst, NULL);

		g_hWndStatusbar = DoCreateStatusBar(hWnd, 1, hInst, 1);
		
		// Set default GUI font after creating all child windows 
		EnumChildWindows(hWnd, (WNDENUMPROC)SetFont, (LPARAM)GetStockObject(DEFAULT_GUI_FONT));
		// Set default font code ends here

		break;
	case WM_SIZE:
		SendMessage(g_hWndToolbar, TB_AUTOSIZE, 0, 0); 

		// Code to automatically resize status bar on dragging window STARTS here
		int iStatusBarWidths[1];
		iStatusBarWidths[0] = -1;
		SendMessage(g_hWndStatusbar, SB_SETPARTS, (WPARAM)1, (LPARAM)iStatusBarWidths);
		SendMessage(g_hWndStatusbar, WM_SIZE, 0, 0);
		// Code to automatically resize status bar on dragging window ENDS here

		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
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
