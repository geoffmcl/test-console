/*\
* test-con3.c
*
* Copyright (c) 2018 - Geoff R. McLane
* Licence: GNU GPL version 2
*
\*/

#include "test-con3.h"
#include <stdio.h>  // for fopen, stdout, ...
#include "..\sprtf.hxx"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitleW[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClassW[MAX_LOADSTRING];            // the main window class name
CHAR szTitle[MAX_LOADSTRING];                  // The title bar text
CHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// add shorter, clearer message of 'type'
typedef struct tagDW2STG {
    DWORD val;
    char *type;
    char *desc;
}DW2STG, *PDW2STG;

static DW2STG ft2stg[] = {
    { FILE_TYPE_CHAR, "FILE_TYPE_CHAR", "is a character file, typically an LPT device or a console." },
    { FILE_TYPE_DISK, "FILE_TYPE_DISK", "is a disk file." },
    { FILE_TYPE_PIPE, "FILE_TYPE_PIPE", "is a socket, a named pipe, or an anonymous pipe." },
    { FILE_TYPE_REMOTE, "FILE_TYPE_REMOTE", "Unused." },
    { FILE_TYPE_UNKNOWN, "FILE_TYPE_UNKNOWN", "Either the type of the specified file is unknown, or the function failed." },

    // LAST
    { 0, 0 }
};

static char *GetFileTypeStg(DWORD dw)
{
    PDW2STG pdw = &ft2stg[0];
    while (pdw->desc) {
        if (pdw->val == dw)
            return pdw->type;   // pdw->desc;
        pdw++;
    }
    return "Unexpected type";
}

// See Reference
static void sendEnterKey(void)
{
    INPUT ip;
    // Set up a generic keyboard event. 
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0; // hardware scan code for key 
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;
    // Send the "Enter" key 
    ip.ki.wVk = 0x0D; // virtual-key code for the "Enter" key 
    ip.ki.dwFlags = 0; // 0 for key press 
    SendInput(1, &ip, sizeof(INPUT));
    // Release the "Enter" key 
    ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release 
    SendInput(1, &ip, sizeof(INPUT));
}

int attach_console()
{
    int iret = 0;
    DWORD dwMode = 0;
    BOOL bRedON = FALSE;
    // get 'stdout' info
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);  // get stdout handle
    DWORD type = GetFileType(hout);
    char *desc = GetFileTypeStg(type);
    if (hout && !GetConsoleMode(hout, &dwMode))
        bRedON = TRUE;

    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        int dnfro1 = 0;
        // selectively use 'freopen', if not already REDIRECTED!
        if ((type != FILE_TYPE_DISK) && !bRedON) {
            freopen("conout$", "w", stdout);
            dnfro1 = 1;
        }
        SPRTF("\nAttachConsole succeeded... stdout: type %s, %s, bRedON=%s\n", desc, 
            (dnfro1 ? "done fropen" : "is disk file"),
            (bRedON ? "TRUE" : "FALSE") );
    }
    else {
        DWORD err = GetLastError();
        const char *pmsg = (err == ERROR_ACCESS_DENIED) ? "ERROR_ACCESS_DENIED" :
            (err == ERROR_INVALID_HANDLE) ? "ERROR_INVALID_HANDLE" :
            (err == ERROR_GEN_FAILURE) ? "ERROR_GEN_FAILURE" :
            "UNEXPECTED_ERROR";
        SPRTF("\nAttachConsole FAILED... %s (%d)\n", pmsg, err);
        iret = 1;
    }
    return iret;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    int len, cons;
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    set_log_file("tempcon3.txt", 0);
    cons = attach_console();
    add_sys_time(1);

    // Initialize global strings
    len = LoadStringW(hInstance, IDS_APP_TITLE, szTitleW, MAX_LOADSTRING);
    wcstombs(szTitle, szTitleW, len);
    len = LoadStringW(hInstance, IDC_WIN32PROJECT1, szWindowClassW, MAX_LOADSTRING);
    wcstombs(szWindowClass, szWindowClassW, len);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT1));

    MSG msg;
    SPRTF("Started test-con3 app...\n");
    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    SPRTF("Ended test-con3 app...\n");
    if (!cons)
        sendEnterKey();

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

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32PROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCE(IDC_WIN32PROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

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
//  PURPOSE:  Processes messages for the main window.
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
