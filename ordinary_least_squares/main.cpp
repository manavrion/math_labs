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
#include "ordinary_least_squares.h"
#include "paintable_point.h"
#include "xoy.h"
#include "func_shape.h"

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

std::vector<double> Parsedoubles(std::string line) {

    line.push_back(' ');

    std::vector<double> ret;

    std::string buf;

    for (auto ch : line) {

        if (isdigit(ch) || ch == '-') {
            buf.push_back(ch);
        } else if (ch == '.' || ch == ',') {
            buf.push_back('.');
        } else if (ch == ' ') {
            if (!buf.empty()) {
                double num = atof(buf.c_str());
                ret.push_back(num);
                buf.clear();
            }
        } else {

            std::string error = "Error: Unexpected symbol \'";
            error += ch;
            error += "\'";
            throw error;
        }
    }
    return  ret;
}


static GdiPlate* plate;
static std::vector<IPaintable*> plateShapes;
static std::vector<IPaintable*> dynamicPlateShapes;
static std::function<PointF(PointF)> toScreenCoord;
static std::function<PointF(PointF)> toRealCoord;

static const Gdiplus::Size size(500, 500);
static const PointF center(250, 250);

static PointF shift(0, 0);
static std::unique_ptr<Point> mouseCoord = nullptr;
static double scaling = 16.0f;

std::string GetText(HWND hWindow) {
    int bufSize = SendMessage(hWindow, WM_GETTEXTLENGTH, 0, 0) + 1;
    char* buf = new char[bufSize];
    SendMessage(hWindow, WM_GETTEXT, bufSize, (LPARAM)buf);
    std::string s(buf);
    delete buf;
    return s;
}

std::string BuildGraph(std::vector<double> xs, std::vector<double> ys, int order) {

    std::vector<double> as = RunOrdinaryLeastSquares(xs, ys, order);

    for (auto ob : dynamicPlateShapes) {
        delete ob;
    }
    dynamicPlateShapes.clear();


    dynamicPlateShapes.push_back(new FuncShape(as, Color::White, 4.0f));

    for (size_t i = 0; i < xs.size(); i++) {
        dynamicPlateShapes.push_back(new PaintablePoint(PointF(xs[i], ys[i]), Color::Red, 6.0f));
    }

    



    std::string ret;
    for (auto num : as) {
        ret += std::to_string(num);
        ret += "   ";
    }
    return ret;
}

// Message handler for about box.
INT_PTR CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
        case WM_INITDIALOG: {
            plate = new GdiPlate(PaintPlate, size);

            SendMessage(GetDlgItem(hWnd, IDC_EDIT_XS), WM_SETTEXT, 0, (LPARAM)"-5 -3.5 -2  1.5 3.25 5");
            SendMessage(GetDlgItem(hWnd, IDC_EDIT_YS), WM_SETTEXT, 0, (LPARAM)"0.5 1.2 1.4 1.6 1.7  1.5");
            SendMessage(GetDlgItem(hWnd, IDC_EDIT_ORDER), WM_SETTEXT, 0, (LPARAM)"3");

            toScreenCoord = [&](PointF point) -> PointF {
                point.X += shift.X;
                point.Y -= shift.Y;

                point.X *= scaling;
                point.Y *= -scaling;

                point.X += center.X;
                point.Y += center.Y;
                return point;
            };

            toRealCoord = [&](PointF point) -> PointF {

                point.X -= center.X;
                point.Y -= center.Y;

                point.X /= scaling;
                point.Y /= -scaling;

                point.X -= shift.X;
                point.Y += shift.Y;
                             
                return point;
            };

            plate->GetGraphics()->SetSmoothingMode(SmoothingMode::SmoothingModeAntiAlias);

            plateShapes.push_back(new XOY(Color::White, 2.0f));

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
            if (LOWORD(wParam) == IDC_BUTTON_BUILD) {
                std::string xs = GetText(GetDlgItem(hWnd, IDC_EDIT_XS));
                std::string ys = GetText(GetDlgItem(hWnd, IDC_EDIT_YS));
                std::string ord = GetText(GetDlgItem(hWnd, IDC_EDIT_ORDER));
                std::vector<double> vxs;
                std::vector<double> vys;
                std::vector<double> vord;
                try {
                    vxs = Parsedoubles(xs);
                    vys = Parsedoubles(ys);
                    vord = Parsedoubles(ord);
                } catch (const std::string& error) {
                    SendMessage(GetDlgItem(hWnd, IDC_STATIC_STATUS), WM_SETTEXT, 0, (LPARAM)error.c_str());
                    break;
                }
                if (vxs.size() != vys.size()) {
                    SendMessage(GetDlgItem(hWnd, IDC_STATIC_STATUS), WM_SETTEXT, 0, (LPARAM)"Error: Different number of arguments");
                    break;
                }
                if (vxs.size() == vys.size() && vys.size() == 0) {
                    SendMessage(GetDlgItem(hWnd, IDC_STATIC_STATUS), WM_SETTEXT, 0, (LPARAM)"Error: Empty input");
                    break;
                }

                int order = int(vord[0] + 0.01);

                if (order == 0) {
                    SendMessage(GetDlgItem(hWnd, IDC_STATIC_STATUS), WM_SETTEXT, 0, (LPARAM)"Error: Zero order is not supported");
                    break;
                }

                std::string result = BuildGraph(vxs, vys, order);

                SendMessage(GetDlgItem(hWnd, IDC_STATIC_RESULT), WM_SETTEXT, 0, (LPARAM)result.c_str());

                SendMessage(GetDlgItem(hWnd, IDC_STATIC_STATUS), WM_SETTEXT, 0, (LPARAM)"Success!");
                InvalidateRect(hWnd, NULL, false);
            }
            break;
        }
        case WM_MOUSEWHEEL: {
            short delta = GET_WHEEL_DELTA_WPARAM(wParam);
            double d = 0.1f;
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
        plateShape->Paint(graphics, toScreenCoord, toRealCoord);
    }
    for (auto& plateShape : dynamicPlateShapes) {
        plateShape->Paint(graphics, toScreenCoord, toRealCoord);
    }
};