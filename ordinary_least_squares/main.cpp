// main.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "main.h"

#include <vector>
#include <utility>
#include <Windowsx.h>

#include "gdi_plate.h"
#include "i_paintable.h"
#include "line.h"

using namespace Gdiplus;
using namespace ordinary_least_squares;

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

static GdiPlate* plate;
static std::vector<ordinary_least_squares::IPaintable*> plateShapes;
static std::function<PointF(PointF)> toScreenCoord;

static const Gdiplus::Size size(500, 500);
static const PointF center(250, 250);

static PointF shift(0, 0);
static std::unique_ptr<Point> mouseCoord = nullptr;
static float scaling = 2.0f;

// Message handler for about box.
INT_PTR CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
        case WM_INITDIALOG: {
            plate = new GdiPlate(PaintPlate, size);

            toScreenCoord = [&](PointF point) -> PointF {
                point.X += shift.X;
                point.Y -= shift.Y;

                point.X *= scaling;
                point.Y *= -scaling;

                point.X += center.X;
                point.Y += center.Y;
                return point;
            };

            plate->GetGraphics()->SetSmoothingMode(SmoothingMode::SmoothingModeAntiAlias);

            plateShapes.push_back(new ordinary_least_squares::Line(PointF(-100, 0), PointF(100, 0), Color::White, 2.0f));
            plateShapes.push_back(new ordinary_least_squares::Line(PointF(90, 5), PointF(100, 0), Color::White, 2.0f));
            plateShapes.push_back(new ordinary_least_squares::Line(PointF(90, -5), PointF(100, 0), Color::White, 2.0f));

            plateShapes.push_back(new ordinary_least_squares::Line(PointF(0, -100), PointF(0, 100), Color::White, 2.0f));
            plateShapes.push_back(new ordinary_least_squares::Line(PointF(5, 90), PointF(0, 100), Color::White, 2.0f));
            plateShapes.push_back(new ordinary_least_squares::Line(PointF(-5, 90), PointF(0, 100), Color::White, 2.0f));

            plateShapes.push_back(new ordinary_least_squares::Line(PointF(10, 10), PointF(20, 20), Color::White, 2.0f));

            return (INT_PTR)TRUE;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            plate->Render();
            plate->Draw(hdc, 10, 10);

            EndPaint(hWnd, &ps);
            break;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
                EndDialog(hWnd, LOWORD(wParam));
                return (INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDC_BUTTON_RESET_VIEW) {
                shift = PointF(0, 0);
                scaling = 2.0f;
                InvalidateRect(hWnd, NULL, false);
            }
            break;
        }
        case WM_MOUSEWHEEL: {
            short delta = GET_WHEEL_DELTA_WPARAM(wParam);
            float d = 0.1f;
            scaling *= (delta < 0 ? 1.0f - d : 1.0f + d);
            InvalidateRect(hWnd, NULL, false);
            break;
        }
        case WM_LBUTTONDOWN: {
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);
            if (xPos >= 10 && xPos <= 510 && yPos >= 10 && yPos <= 510) {
                mouseCoord.reset(new Point(xPos, yPos));
            }
            break;
        }
        case WM_MOUSEMOVE: {
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);
            if (mouseCoord && xPos >= 10 && xPos <= 510 && yPos >= 10 && yPos <= 510) {

                shift.X += (xPos - mouseCoord->X)/scaling;
                shift.Y += (yPos - mouseCoord->Y)/scaling;

                mouseCoord->X = xPos;
                mouseCoord->Y = yPos;
                InvalidateRect(hWnd, NULL, false);
            }
            break;
        }
        case WM_LBUTTONUP: {
            mouseCoord.reset();
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
void PaintPlate(Gdiplus::Graphics& graphics) {
    graphics.Clear(Gdiplus::Color::Blue);
    for (auto& plateShape : plateShapes) {
        plateShape->Paint(graphics, toScreenCoord);
    }
};