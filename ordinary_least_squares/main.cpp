// main.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "main.h"

// Forward declarations of functions included in this code module:
INT_PTR CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void                PaintPlate(Gdiplus::Graphics &graphics);

int APIENTRY WinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPSTR    lpCmdLine,
                     _In_ int       nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // init GDI+
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINWINDOW), nullptr, WndProc);

    return 0;
}

static Gdiplus::GdiPlate* plate;

// Message handler for about box.
INT_PTR CALLBACK WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
        case WM_INITDIALOG: {
            plate = new Gdiplus::GdiPlate(PaintPlate, Gdiplus::Size(500, 500));
            return (INT_PTR)TRUE;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hDlg, &ps);

            plate->Render();
            plate->Draw(hdc, 10, 10);

            EndPaint(hDlg, &ps);
            break;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            }
            break;
        }
        case WM_DESTROY: {
            delete plate;
            break;
        }
    }
    return (INT_PTR)FALSE;
}

// GDIplus paint provider
void PaintPlate(Gdiplus::Graphics &graphics) {
    graphics.Clear(Gdiplus::Color::Blue);
};