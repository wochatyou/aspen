// Texter.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Texter.h"

#define S8      int8_t
#define S16     int16_t
#define S32     int32_t
#define S64     int64_t

#define U8      uint8_t
#define U16     uint16_t
#define U32     uint32_t
#define U64     uint64_t

// define useful macros from windowsx.h
#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lParam)	((int)(short)LOWORD(lParam))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lParam)	((int)(short)HIWORD(lParam))
#endif

#define MAX_LOADSTRING 100

template <class T>
void SafeRelease(T** ppT)
{
    if (nullptr != *ppT)
    {
        (*ppT)->Release();
        *ppT = nullptr;
    }
}


// Global Variables:
BOOL g_isInside = FALSE;
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

ID2D1Factory* g_pD2DFactory = nullptr;
IDWriteFactory* g_pDWriteFactory = nullptr;
IDWriteTextFormat* g_pTextFormat = nullptr;
IDWriteTextLayout* g_pTextLayout = nullptr;
ID2D1HwndRenderTarget* g_pD2DRenderTarget = nullptr;
ID2D1Bitmap* g_pixelBitmap = nullptr;
ID2D1SolidColorBrush* g_pTextBrush = nullptr;
HCURSOR g_hCursorIBeam = nullptr;

int L, T, R, B;

wchar_t testxt[] = {
    0x6c38,0x548c,0x4e5d,0x5e74,0xff0c,0x5c81,0x5728,0x7678,0x4e11,0xff0c,0x66ae,0x6625,0x4e4b,0x521d,0xff0c,0x4f1a,
    0x4e8e,0x4f1a,0x7a3d,0x5c71,0x9634,0x4e4b,0x5170,0x4ead,0xff0c,0x4fee,0x798a,0x4e8b,0x4e5f,0x3002,0x7fa4,0x8d24,
    0x6bd5,0x81f3,0xff0c,0x5c11,0x957f,0x54b8,0x96c6,0x3002,0x6b64,0x5730,0x6709,0x5d07,0x5c71,0x5cfb,0x5cad,0xff0c,
    0x8302,0x6797,0x4fee,0x7af9,0xff1b,0x53c8,0x6709,0x6e05,0x6d41,0x6fc0,0x6e4d,0xff0c,0x6620,0x5e26,0x5de6,0x53f3,
    0xff0c,0x5f15,0x4ee5,0x4e3a,0x6d41,0x89de,0x66f2,0x6c34,0xff0c,0x5217,0x5750,0x5176,0x6b21,0x3002,0x867d,0x65e0,
    0x4e1d,0x7af9,0x7ba1,0x5f26,0x4e4b,0x76db,0xff0c,0x4e00,0x89de,0x4e00,0x548f,0xff0c,0x4ea6,0x8db3,0x4ee5,0x7545,
    0x53d9,0x5e7d,0x60c5,0x3002,0x662f,0x65e5,0x4e5f,0xff0c,0x5929,0x6717,0x6c14,0x6e05,0xff0c,0x60e0,0x98ce,0x548c,
    0x7545,0x3002,0x4ef0,0x89c2,0x5b87,0x5b99,0x4e4b,0x5927,0xff0c,0x4fef,0x5bdf,0x54c1,0x7c7b,0x4e4b,0x76db,0xff0c,0x000a,0x000a,
    0x6240,0x4ee5,0x6e38,0x76ee,0x9a8b,0x6000,0xff0c,0x8db3,0x4ee5,0x6781,0x89c6,0x542c,0x4e4b,0x5a31,0xff0c,0x4fe1,
    0x53ef,0x4e50,0x4e5f,0x3002,0x592b,0x4eba,0x4e4b,0x76f8,0x4e0e,0xff0c,0x4fef,0x4ef0,0x4e00,0x4e16,0xff0c,0x6216,
    0x53d6,0x8bf8,0x6000,0x62b1,0xff0c,0x609f,0x8a00,0x4e00,0x5ba4,0x4e4b,0x5185,0xff1b,0x6216,0x56e0,0x5bc4,0x6240,
    0x6258,0xff0c,0x653e,0x6d6a,0x5f62,0x9ab8,0x4e4b,0x5916,0x3002,0x867d,0x8da3,0x820d,0x4e07,0x6b8a,0xff0c,0x9759,
    0x8e81,0x4e0d,0x540c,0xff0c,0x5f53,0x5176,0x6b23,0x4e8e,0x6240,0x9047,0xff0c,0x6682,0x5f97,0x4e8e,0x5df1,0xff0c,
    0x5feb,0x7136,0x81ea,0x8db3,0xff0c,0x4e0d,0x77e5,0x8001,0x4e4b,0x5c06,0x81f3,0x3002,0x53ca,0x5176,0x6240,0x4e4b,
    0x65e2,0x5026,0xff0c,0x60c5,0x968f,0x4e8b,0x8fc1,0xff0c,0x611f,0x6168,0x7cfb,0x4e4b,0x77e3,0x3002,0x5411,0x4e4b,
    0x6240,0x6b23,0xff0c,0x4fef,0x4ef0,0x4e4b,0x95f4,0xff0c,0x5df2,0x4e3a,0x9648,0x8ff9,0xff0c,0x72b9,0x4e0d,0x80fd,
    0x4e0d,0x4ee5,0x4e4b,0x5174,0x6000,0x3002,0x51b5,0x4fee,0x77ed,0x968f,0x5316,0xff0c,0x7ec8,0x671f,0x4e8e,0x5c3d,
    0x3002,0x53e4,0x4eba,0x4e91,0xff1a,0x201c,0x6b7b,0x751f,0x4ea6,0x5927,0x77e3,0x3002,0x201d,0x5c82,0x4e0d,0x75db,
    0x54c9,0xff01,0x6bcf,0x89c8,0x6614,0x4eba,0x5174,0x611f,0x4e4b,0x7531,0xff0c,0x82e5,0x5408,0x4e00,0x5951,0xff0c,
    0x672a,0x5c1d,0x4e0d,0x4e34,0x6587,0x55df,0x60bc,0xff0c,0x4e0d,0x80fd,0x55bb,0x4e4b,0x4e8e,0x6000,0x3002,0x56fa,
    0x77e5,0x4e00,0x6b7b,0x751f,0x4e3a,0x865a,0x8bde,0xff0c,0x9f50,0x5f6d,0x6b87,0x4e3a,0x5984,0x4f5c,0x3002,0x540e,
    0x4e4b,0x89c6,0x4eca,0xff0c,0x4ea6,0x72b9,0x4eca,0x4e4b,0x89c6,0x6614,0x3002,0x60b2,0x592b,0xff01,0x6545,0x5217,
    0x53d9,0x65f6,0x4eba,0xff0c,0x5f55,0x5176,0x6240,0x8ff0,0xff0c,0x867d,0x4e16,0x6b8a,0x4e8b,0x5f02,0xff0c,0x6240,
    0x4ee5,0x5174,0x6000,0xff0c,0x5176,0x81f4,0x4e00,0x4e5f,0x3002,0x540e,0x4e4b,0x89c8,0x8005,0xff0c,0x4ea6,0x5c06,
    0x6709,0x611f,0x4e8e,0x65af,0x6587,0x3002,0
};

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    HRESULT hr = S_OK;
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    g_hCursorIBeam = ::LoadCursor(NULL, IDC_IBEAM);

    // Initialize Direct2D and DirectWrite
    g_pD2DFactory = nullptr;
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pD2DFactory);
    if (S_OK != hr || nullptr == g_pD2DFactory)
    {
        MessageBox(NULL, _T("The calling of D2D1CreateFactory() is failed"), _T("WoChat Error"), MB_OK);
        return (-1);
    }

    g_pDWriteFactory = nullptr;
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&g_pDWriteFactory));
    if (S_OK != hr || nullptr == g_pDWriteFactory)
    {
        MessageBox(NULL, _T("The calling of DWriteCreateFactory() is failed"), _T("Wochat Error"), MB_OK);
        return (-1);
    }

    g_pTextFormat = nullptr;
    hr = g_pDWriteFactory->CreateTextFormat(
        L"Microsoft Yahei",
        NULL,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        16.0f,
        L"en-US",
        &g_pTextFormat
    );

    if (S_OK != hr || nullptr == g_pTextFormat)
    {
        MessageBox(NULL, _T("The calling of CreateTextFormat() is failed"), _T("Wochat Error"), MB_OK);
        return (-1);
    }

    L = 100, T = 100, R = 500, B = 400;
    g_pTextLayout = nullptr;
    hr = g_pDWriteFactory->CreateTextLayout(
        testxt,
        (UINT32)wcslen(testxt),
        g_pTextFormat,
        (FLOAT)(R - L),
        (FLOAT)(B - T),
        &g_pTextLayout
    );

    if (S_OK != hr || nullptr == g_pTextLayout)
    {
        MessageBox(NULL, _T("The calling of CreateTextLayout() is failed"), _T("Wochat Error"), MB_OK);
        return (-1);
    }

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TEXTER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TEXTER));

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

    SafeRelease(&g_pTextLayout);
    SafeRelease(&g_pTextFormat);
    SafeRelease(&g_pDWriteFactory);
    SafeRelease(&g_pD2DFactory);

    return (int) msg.wParam;
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

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TEXTER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TEXTER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT DoPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

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
    static BOOL bHandled = FALSE;
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
        bHandled = TRUE;
        break;
    case WM_MOUSEMOVE:
        {
            OnMouseMove(hWnd, message, wParam, lParam);
        }
        bHandled = TRUE;
        break;
    case WM_SETCURSOR:
        bHandled = FALSE;
        if(g_isInside)
        {
            bHandled = TRUE;
        }
        break;
    case WM_SIZE:
        {
            if (SIZE_MINIMIZED != wParam)
            {
                SafeRelease(&g_pixelBitmap);
                SafeRelease(&g_pTextBrush);
                SafeRelease(&g_pD2DRenderTarget);
            }
        }
        bHandled = TRUE;
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            DoPaint(hWnd, message, wParam, lParam);
            EndPaint(hWnd, &ps);
        }
        bHandled = TRUE;
        break;
    case WM_DESTROY:
        SafeRelease(&g_pixelBitmap);
        SafeRelease(&g_pTextBrush);
        SafeRelease(&g_pD2DRenderTarget);
        PostQuitMessage(0);
        bHandled = TRUE;
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    if(!bHandled)
        return DefWindowProc(hWnd, message, wParam, lParam);
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

LRESULT DoPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HRESULT hr = S_OK;
    RECT rc;
    GetClientRect(hWnd, &rc);

    if (nullptr == g_pD2DRenderTarget)
    {
        D2D1_PIXEL_FORMAT pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);
        D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties = D2D1::RenderTargetProperties();
        renderTargetProperties.dpiX = 96;
        renderTargetProperties.dpiY = 96;
        renderTargetProperties.pixelFormat = pixelFormat;

        D2D1_HWND_RENDER_TARGET_PROPERTIES hwndRenderTragetproperties
            = D2D1::HwndRenderTargetProperties(hWnd, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top));

        hr = g_pD2DFactory->CreateHwndRenderTarget(renderTargetProperties, hwndRenderTragetproperties, &g_pD2DRenderTarget);
        if (S_OK == hr && nullptr != g_pD2DRenderTarget)
        {
            U32 pixel[1] = { 0xFF000000 };
            SafeRelease(&g_pixelBitmap);
            hr = g_pD2DRenderTarget->CreateBitmap(
                D2D1::SizeU(1, 1), pixel, 4, D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
                &g_pixelBitmap);
            if (S_OK == hr && nullptr != g_pixelBitmap)
            {
                hr = g_pD2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0x666666), &g_pTextBrush);
            }
        }
    }

    if (S_OK == hr && nullptr != g_pD2DRenderTarget && nullptr != g_pixelBitmap && nullptr != g_pTextBrush)
    {
        D2D1_RECT_F r;
        //D2D1_RECT_F clipRect;
        //D2D1_RECT_F layoutRect;
        g_pD2DRenderTarget->BeginDraw();
        g_pD2DRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
        g_pD2DRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

        r.left = static_cast<FLOAT>(rc.left); r.top = static_cast<FLOAT>(T - 1); r.right = static_cast<FLOAT>(rc.right); r.bottom = static_cast<FLOAT>(T);
        g_pD2DRenderTarget->DrawBitmap(g_pixelBitmap, &r);

        r.left = static_cast<FLOAT>(rc.left); r.top = static_cast<FLOAT>(B); r.right = static_cast<FLOAT>(rc.right); r.bottom = static_cast<FLOAT>(B+1);
        g_pD2DRenderTarget->DrawBitmap(g_pixelBitmap, &r);
        r.left = static_cast<FLOAT>(L-1); r.top = static_cast<FLOAT>(rc.top); r.right = static_cast<FLOAT>(L); r.bottom = static_cast<FLOAT>(rc.bottom);
        g_pD2DRenderTarget->DrawBitmap(g_pixelBitmap, &r);
        r.left = static_cast<FLOAT>(R); r.top = static_cast<FLOAT>(rc.top); r.right = static_cast<FLOAT>(R+1); r.bottom = static_cast<FLOAT>(rc.bottom);
        g_pD2DRenderTarget->DrawBitmap(g_pixelBitmap, &r);

        if (nullptr != g_pTextLayout && nullptr != g_pTextBrush)
        {
            g_pD2DRenderTarget->DrawTextLayout(D2D1::Point2F(static_cast<FLOAT>(L), static_cast<FLOAT>(T)), g_pTextLayout, g_pTextBrush);
        }

        hr = g_pD2DRenderTarget->EndDraw();
        if (FAILED(hr) || D2DERR_RECREATE_TARGET == hr)
        {
            SafeRelease(&g_pD2DRenderTarget);
        }
    }

    return 0;
}

LRESULT OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    BOOL isTrailingHit = FALSE;
    int xPos = GET_X_LPARAM(lParam);
    int yPos = GET_Y_LPARAM(lParam);
    FLOAT x = (FLOAT)(xPos - L);
    FLOAT y = (FLOAT)(yPos - T);

    DWRITE_HIT_TEST_METRICS htm = { 0 };

    if (nullptr != g_pTextLayout)
    {
        wchar_t xtitle[256 + 1] = { 0 };
        g_pTextLayout->HitTestPoint(x, y, &isTrailingHit, &g_isInside, &htm);
        if (g_isInside)
        {
            swprintf((wchar_t*)xtitle, 256, L"I am in it");
            ::SetCursor(g_hCursorIBeam);
        }
        else
        {
            swprintf((wchar_t*)xtitle, 256, L"I am NOT in it");
        }

        SetWindowTextW(hWnd, (LPCWSTR)xtitle);
    }

    return 0;
}
