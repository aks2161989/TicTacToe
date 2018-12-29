// TicTacToe.cpp : Defines the entry point for the application.

#include "stdafx.h"
#include<cstdlib>
#include "TicTacToe.h"
#include <CommCtrl.h>
#include "GameLogic.h" // All backend code
#include <ctime>
#pragma comment(lib, "comctl32.lib")

#define MAX_LOADSTRING 100
#define IDM_NEW 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND g_hWndToolbar;
HWND hWndComboBox1, hWndComboBox2;				// Declare 2 combo boxes 
HWND btn_hwnd[9];								// An array of 9 buttons



//Constants
const int btn_id_0 = 10,
btn_id_1 = 11,
btn_id_2 = 12,
btn_id_3 = 13,
btn_id_4 = 14,
btn_id_5 = 15,
btn_id_6 = 16,
btn_id_7 = 17,
btn_id_8 = 18,
toolbar_id = 123,
combobox1_id = 456,
combobox2_id = 789,
statusbar_id = 111;

int intDifficulty = 1; // Difficulty level in int 
TicTacToe* t = new TicTacToe(intDifficulty); // Instantiate a tic tac toe game

TCHAR comboBoxDifficulty[256] = _T("Medium"); // Difficulty level chosen by the player in the first combo box. Default value = "Medium"

int ItemIndex; // Get the index of the combo box item selected by the user

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
	HWND hWndToolbar = CreateWindowExA(0, TOOLBARCLASSNAME, NULL,
		WS_CHILD | TBSTYLE_WRAPABLE | TBSTYLE_EX_DRAWDDARROWS, 0, 0, 0, 0,
		hWndParent, (HMENU)toolbar_id, hInst, NULL);

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
			MAKELONG(STD_FILENEW, ImageListID), IDM_NEW, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)_T("New")
		}
	};

	// Add buttons.
	SendMessageA(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessageA(hWndToolbar, TB_ADDBUTTONS, (WPARAM)numButtons, (LPARAM)&tbButtons);

	// Drop-down list code below
	// CREATE first COMBO BOX
	hWndComboBox1 = CreateWindowA(
		WC_COMBOBOX,
		TEXT(""),
		 CBS_HASSTRINGS | WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWNLIST,
		40, 5, 100, 100,
		hWndToolbar, (HMENU)combobox1_id, NULL, NULL);

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

	// Send the CB_SETCURSEL message to display an initial item (Medium)
	// in the selection field
	SendMessage(hWndComboBox1, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);

	// Create combo box 2
	hWndComboBox2 = CreateWindowA(
		WC_COMBOBOX,
		TEXT(""),
		 CBS_HASSTRINGS | WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWNLIST,
		147, 5, 45, 100,
		hWndToolbar, (HMENU)combobox2_id, NULL, NULL);

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

	// Send the CB_SETCURSEL message to display an initial item (X)
	// in the selection field
	SendMessage(hWndComboBox2, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

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
		_T("Mark a square"),		// text shown on creation
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
	srand(static_cast<unsigned int>(time(0)));
	rand();
	t = new TicTacToe(intDifficulty);

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

void updateAllButtons() // Update text of all buttons
{
	std::string strMSquareContents[9]{ "" };
	int itemIndex = 0;
	for (int outer = 0; outer < 3; outer++)
	{
		for (int inner = 0; inner < 3; inner++)
		{
			if (t->setMSquareContents(outer, inner) == 'X' ||
				t->setMSquareContents(outer, inner) == 'O')
				strMSquareContents[itemIndex] += t->setMSquareContents(outer, inner);
			else
				strMSquareContents[itemIndex] = "";
			itemIndex++;
		}
	}
	for (int i = 0; i < 9; i++)
		SendMessageA(btn_hwnd[i], WM_SETTEXT, 0, (LPARAM)strMSquareContents[i].c_str());
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
	string charToStr = "";
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_FILE_NEWGAME:
		case IDM_NEW: // When the "New" button on toolbar or "File" menu is clicked
			t = new TicTacToe(intDifficulty);
			updateAllButtons();
			SetWindowTextA(g_hWndStatusbar, "Mark a square");
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case btn_id_0: // When button (0, 0) is clicked
			if (t->setMSquareContents(0, 0) != 'X' && t->setMSquareContents(0, 0) != 'O' && !t->getIsWonOrLost())
			{
				t->setPlayerChoice('1');
				t->play();
				updateAllButtons();
			}
			break;
		case btn_id_1: // When button (0, 1) is clicked
			if (t->setMSquareContents(0, 1) != 'X' && t->setMSquareContents(0, 1) != 'O' && !t->getIsWonOrLost())
			{
				t->setPlayerChoice('2');
				t->play();
				updateAllButtons();
			}
			break;
		case btn_id_2: // When button (0, 2) is clicked
			if (t->setMSquareContents(0, 2) != 'X' && t->setMSquareContents(0, 2) != 'O' && !t->getIsWonOrLost())
			{
				t->setPlayerChoice('3');
				t->play();
				updateAllButtons();
			}
			break;
		case btn_id_3: // When button (1, 0) is clicked
			if (t->setMSquareContents(1, 0) != 'X' && t->setMSquareContents(1, 0) != 'O' && !t->getIsWonOrLost())
			{
				t->setPlayerChoice('4');
				t->play();
				updateAllButtons();
			}
			break;
		case btn_id_4: // When button (1, 1) is clicked
			if (t->setMSquareContents(1, 1) != 'X' && t->setMSquareContents(1, 1) != 'O' && !t->getIsWonOrLost())
			{
				t->setPlayerChoice('5');
				t->play();
				updateAllButtons();
			}
			break;
		case btn_id_5: // When button (1, 2) is clicked
			if (t->setMSquareContents(1, 2) != 'X' && t->setMSquareContents(1, 2) != 'O' && !t->getIsWonOrLost())
			{
				t->setPlayerChoice('6');
				t->play();
				updateAllButtons();
			}
			break;
		case btn_id_6: // When button (2, 0) is clicked
			if (t->setMSquareContents(2, 0) != 'X' && t->setMSquareContents(2, 0) != 'O' && !t->getIsWonOrLost())
			{
				t->setPlayerChoice('7');
				t->play();
				updateAllButtons();
			}			
			break;
		case btn_id_7: // When button (2, 1) is clicked
			if (t->setMSquareContents(2, 1) != 'X' && t->setMSquareContents(2, 1) != 'O' && !t->getIsWonOrLost())
			{
				t->setPlayerChoice('8');
				t->play();
				updateAllButtons();
			}
			break;
		case btn_id_8: // When button (2, 2) is clicked
			if (t->setMSquareContents(2, 2) != 'X' && t->setMSquareContents(2, 2) != 'O' && !t->getIsWonOrLost())
			{
				t->setPlayerChoice('9');
				t->play();
				updateAllButtons();
			}
			break;
		case combobox1_id: // When the first combo box is clicked
			if (HIWORD(wParam) == CBN_SELCHANGE) // Proceed only if the users drops down the list and selects an option
			{
				ItemIndex = SendMessageA((HWND)lParam, (UINT)CB_GETCURSEL,
					(WPARAM)0, (LPARAM)0); //   Send CB_GETCURSEL message to get the index of the selected list item.
				(TCHAR)SendMessageA((HWND)lParam, (UINT)CB_GETLBTEXT,
					(WPARAM)ItemIndex, (LPARAM)comboBoxDifficulty); //   Send CB_GETLBTEXT message to get the item.
				if (strcmp("Easy", comboBoxDifficulty) == 0)
					intDifficulty = 0;
				else if (strcmp("Medium", comboBoxDifficulty) == 0)
					intDifficulty = 1;
				if (strcmp("Impossible", comboBoxDifficulty) == 0)
					intDifficulty = 2;
				t = new TicTacToe(intDifficulty);
				updateAllButtons();
				SetWindowTextA(g_hWndStatusbar, "Mark a square");
			}
			break;
		case combobox2_id: // When the second combo box is clicked
			if (HIWORD(wParam) == CBN_SELCHANGE) // Proceed only if the users drops down the list and selects an option
			{
				ItemIndex = SendMessageA((HWND)lParam, (UINT)CB_GETCURSEL,
					(WPARAM)0, (LPARAM)0); //   Send CB_GETCURSEL message to get the index of the selected list item.
				(TCHAR)SendMessageA((HWND)lParam, (UINT)CB_GETLBTEXT,
					(WPARAM)ItemIndex, (LPARAM)comboBoxCharacter); //   Send CB_GETLBTEXT message to get the item.
				t = new TicTacToe(intDifficulty);
				t->setPlayerChar(comboBoxCharacter[0]);
				updateAllButtons();
				SetWindowTextA(g_hWndStatusbar, "Mark a square");
			}
			break;
		default:
			return DefWindowProcW(hWnd, message, wParam, lParam);
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

		btn_hwnd[0] = CreateWindowA("Button", "", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 10, 55, 60, 60, hWnd, (HMENU)btn_id_0, hInst, NULL);
		btn_hwnd[1] = CreateWindowA("Button", "", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 70, 55, 60, 60, hWnd, (HMENU)btn_id_1, hInst, NULL);
		btn_hwnd[2] = CreateWindowA("Button", "", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 130, 55, 60, 60, hWnd, (HMENU)btn_id_2, hInst, NULL);
		btn_hwnd[3] = CreateWindowA("Button", "", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 10, 115, 60, 60, hWnd, (HMENU)btn_id_3, hInst, NULL);
		btn_hwnd[4] = CreateWindowA("Button", "", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 70, 115, 60, 60, hWnd, (HMENU)btn_id_4, hInst, NULL);
		btn_hwnd[5] = CreateWindowA("Button", "", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 130, 115, 60, 60, hWnd, (HMENU)btn_id_5, hInst, NULL);
		btn_hwnd[6] = CreateWindowA("Button", "", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 10, 175, 60, 60, hWnd, (HMENU)btn_id_6, hInst, NULL);
		btn_hwnd[7] = CreateWindowA("Button", "", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 70, 175, 60, 60, hWnd, (HMENU)btn_id_7, hInst, NULL);
		btn_hwnd[8] = CreateWindowA("Button", "", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 130, 175, 60, 60, hWnd, (HMENU)btn_id_8, hInst, NULL);

		updateAllButtons();

		g_hWndStatusbar = DoCreateStatusBar(hWnd, statusbar_id, hInst, 1);
		
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
		return DefWindowProcW(hWnd, message, wParam, lParam);
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
	{ // Setting the title bar icon in the "About" box
		HANDLE hIcon;
		hIcon = LoadImageA(hInst, 
			MAKEINTRESOURCE(IDI_TICTACTOE),
			IMAGE_ICON,
			GetSystemMetrics(SM_CXSMICON),
			GetSystemMetrics(SM_CXSMICON),
			0);
		if (hIcon)
		{ //WM_SETICON message changes or sets the small or large icons of windows
		  // In this case we are setting the small icon, so set wParam to ICON_SMALL
			SendMessageA(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		}
	}
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
