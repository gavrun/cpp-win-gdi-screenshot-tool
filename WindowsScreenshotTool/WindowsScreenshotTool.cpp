// WindowsScreenshotTool.cpp : Defines the entry point for the application.
//
// Windows Screenshot Tool - Phase 1 Skeleton
// Pure Win32/GDI version


//#include "framework.h"
#include "WindowsScreenshotTool.h"

#include "ConfigManager.h"
#include "CaptureEngine.h"
#include "FileManager.h"
#include "OverlayWindow.h"

#include <windows.h>
#include <string>
#include <fstream>
#include <sstream>
#include <shlobj.h>

//#define MAX_LOADSTRING 100

// Initialize GDI+
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")


// Global Variables:
//HINSTANCE hInst;                                // current instance
//WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
//WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name


// Forward declarations of functions included in this code module:
//ATOM                MyRegisterClass(HINSTANCE hInstance);
//BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
//INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


// Global config instance
ConfigManager config;


// ----------------- HotkeyListener -----------------
// HotkeyListener registers global hotkeys via Win32 RegisterHotKey
class HotkeyListener {
public:
    void Register(HWND hwnd) {
        // ID 1 Full-screen
        RegisterHotKey(hwnd, 1, MOD_WIN | MOD_SHIFT, 'Q');
        // ID 2 Region capture
        RegisterHotKey(hwnd, 2, MOD_WIN | MOD_SHIFT, 'Z');

        // Note: Silent fail if registration conflicts

        // DEBUG
        //BOOL fullOK = RegisterHotKey(hwnd, 1, MOD_WIN | MOD_SHIFT, 'Q');
        //if (!fullOK) {
        //    MessageBoxW(NULL, L"Full-screen hotkey registration failed", L"Debug", MB_OK);
        //}
        // DEBUG
        //BOOL regionOK = RegisterHotKey(hwnd, 2, MOD_WIN | MOD_SHIFT, 'A');
        //if (!regionOK) {
        //    MessageBoxW(NULL, L"Region hotkey registration failed", L"Debug", MB_OK);
        //}
        //BOOL regionOK = RegisterHotKey(hwnd, 2, MOD_WIN | MOD_SHIFT, 'Z');
        //if (!regionOK) {
        //    MessageBoxW(NULL, L"Region hotkey registration failed", L"Debug", MB_OK);
        //}
    }
};


// ----------------- WinMain TEMPLATE -----------------
//int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
//                     _In_opt_ HINSTANCE hPrevInstance,
//                     _In_ LPWSTR    lpCmdLine,
//                     _In_ int       nCmdShow)
//{
//    UNREFERENCED_PARAMETER(hPrevInstance);
//    UNREFERENCED_PARAMETER(lpCmdLine);
//
//    // TODO: Place code here.
//
//    // Initialize global strings
//    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
//    LoadStringW(hInstance, IDC_WINDOWSSCREENSHOTTOOL, szWindowClass, MAX_LOADSTRING);
//    MyRegisterClass(hInstance);
//
//    // Perform application initialization:
//    if (!InitInstance (hInstance, nCmdShow))
//    {
//        return FALSE;
//    }
//
//    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSSCREENSHOTTOOL));
//
//    MSG msg;
//
//    // Main message loop:
//    while (GetMessage(&msg, nullptr, 0, 0))
//    {
//        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
//        {
//            TranslateMessage(&msg);
//            DispatchMessage(&msg);
//        }
//    }
//
//    return (int) msg.wParam;
//}


// ----------------- WinMain & Message Loop -----------------
// Entry point for Windows GUI app using wide-character strings
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int) {
    
    // Initialize configuration
    //ConfigManager config;
    config.Load();

    // Register a message-only window for receiving hotkey events
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"ScreenshotToolClass";
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, L"ScreenshotToolClass", L"", 0, 
        0, 0, 0, 0, 
        HWND_MESSAGE, NULL, hInstance, NULL
    );
    // No visible UI

    // Initialize GDI+ 
    Gdiplus::GdiplusStartupInput gdipInput;
    ULONG_PTR gdipToken;
    Gdiplus::GdiplusStartup(&gdipToken, &gdipInput, nullptr);

    // Register global hotkeys
    HotkeyListener hotkeys;
    hotkeys.Register(hwnd);

    // Main message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Shut down GDI+
    Gdiplus::GdiplusShutdown(gdipToken);

    return 0;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//ATOM MyRegisterClass(HINSTANCE hInstance)
//{
//    WNDCLASSEXW wcex;
//
//    wcex.cbSize = sizeof(WNDCLASSEX);
//
//    wcex.style          = CS_HREDRAW | CS_VREDRAW;
//    wcex.lpfnWndProc    = WndProc;
//    wcex.cbClsExtra     = 0;
//    wcex.cbWndExtra     = 0;
//    wcex.hInstance      = hInstance;
//    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSSCREENSHOTTOOL));
//    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
//    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
//    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSSCREENSHOTTOOL);
//    wcex.lpszClassName  = szWindowClass;
//    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
//
//    return RegisterClassExW(&wcex);
//}

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
//BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
//{
//   hInst = hInstance; // Store instance handle in our global variable
//
//   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
//      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
//
//   if (!hWnd)
//   {
//      return FALSE;
//   }
//
//   ShowWindow(hWnd, nCmdShow);
//   UpdateWindow(hWnd);
//
//   return TRUE;
//}

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

//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    switch (message)
//    {
//    case WM_COMMAND:
//        {
//            int wmId = LOWORD(wParam);
//            // Parse the menu selections:
//            switch (wmId)
//            {
//            case IDM_ABOUT:
//                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
//                break;
//            case IDM_EXIT:
//                DestroyWindow(hWnd);
//                break;
//            default:
//                return DefWindowProc(hWnd, message, wParam, lParam);
//            }
//        }
//        break;
//    case WM_PAINT:
//        {
//            PAINTSTRUCT ps;
//            HDC hdc = BeginPaint(hWnd, &ps);
//            // TODO: Add any drawing code that uses hdc here...
//            EndPaint(hWnd, &ps);
//        }
//        break;
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        break;
//    default:
//        return DefWindowProc(hWnd, message, wParam, lParam);
//    }
//    return 0;
//}

// ----------------- WndProc -----------------
// Main window message handler
// - Responds to WM_HOTKEY only (no visible window UI)
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_HOTKEY) {
        if (wParam == 1) {
            // FullScreen hotkey pressed
            HBITMAP bmp = CaptureEngine::CaptureFull();
            std::wstring fn = GenerateTimestampedFilename(config.outputFolder);
            FileManager::SavePng(bmp, fn);
            DeleteObject(bmp); // Clean up GDI object
        }
        else if (wParam == 2) {
            // RegionCapture hotkey pressed
            HBITMAP frozen = CaptureEngine::CaptureFull();
            RECT sel;
            
            // DEBUG
            //MessageBoxW(NULL, L"Hotkey hit!", L"Debug", MB_OK);

            bool wasAborted = false;
            
            OverlayWindow ow;
            bool owOk = ow.Show(frozen, sel, wasAborted);
            // DEBUG
            OutputDebugString(owOk ? L"[DEBUG] Overlay Show returned true\n"
                                   : L"[DEBUG] Overlay Show returned false\n");

            if (owOk && !wasAborted) {
                OutputDebugString(L"[DEBUG] Cropping region\n");
                HBITMAP cropped = CaptureEngine::CaptureRegion(sel, frozen);
                if (cropped) {
                    OutputDebugString(L"[DEBUG] Cropped, now saving file\n");
                    std::wstring fn = GenerateTimestampedFilename(config.outputFolder);
                    FileManager::SavePng(cropped, fn);
                    DeleteObject(cropped);
                }
                else {
                    OutputDebugString(L"[DEBUG] Crop returned null bitmap\n");
                }
            }
            else {
                OutputDebugString(L"[DEBUG] Skipping crop (aborted or owOk was false)\n");
            }
            DeleteObject(frozen);
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}


// Message handler for about box.
//INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    UNREFERENCED_PARAMETER(lParam);
//    switch (message)
//    {
//    case WM_INITDIALOG:
//        return (INT_PTR)TRUE;
//sum
//    case WM_COMMAND:
//        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
//        {
//            EndDialog(hDlg, LOWORD(wParam));
//            return (INT_PTR)TRUE;
//        }
//        break;
//    }
//    return (INT_PTR)FALSE;
//}

