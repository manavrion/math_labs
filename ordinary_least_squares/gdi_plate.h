#pragma once
#include <functional>
#include <chrono>
#include <string>

namespace Gdiplus {

    class GdiPlate {
    public:
        GdiPlate(std::function<void(Graphics&)> paint, Size bufferSize) : 
            paint(std::move(paint)), 
            bufferSize(bufferSize), 
            fps(false) {

            //init backBuffer
            backBuffer = new byte[bufferSize.Width * bufferSize.Height * 4];
            backBufferHBitmap = CreateBitmap(bufferSize.Width, bufferSize.Height, 1, 32, backBuffer);

            //init HDC
            backBufferHDC = CreateCompatibleDC(nullptr);
            SelectObject(backBufferHDC, backBufferHBitmap);

            //init graphics
            backBufferGraphics = new Graphics(backBufferHDC);
            backBufferGraphics->SetSmoothingMode(SmoothingMode::SmoothingModeAntiAlias);
        }

        void Render() {
            if (!fps) {
                paint(*backBufferGraphics);
            } else {
                typedef std::chrono::high_resolution_clock clock;
                auto start = clock::now();
                paint(*backBufferGraphics);
                auto end = clock::now();
                PaintTimeOfFrame(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
            }
        }

        void PaintTimeOfFrame(int time) {
            std::wstring str;
            if (time != 0) {
                str = std::to_wstring(time) + std::wstring(L" ms (") + std::to_wstring(1000 / time) + std::wstring(L" fps)");
            } else {
                str = L"really fast";
            }            
            backBufferGraphics->DrawString(str.c_str(), -1, &Gdiplus::Font(L"Arial", 7, FontStyleBold), PointF(1, 4), &Gdiplus::SolidBrush(Gdiplus::Color::White));
        }

        void Draw(HDC hdc, int x, int y) {
            BitBlt(hdc, x, y, bufferSize.Width, bufferSize.Height, backBufferHDC, 0, 0, SRCCOPY);
        }

        Size GetSize() {
            return bufferSize;
        }

        Graphics* GetGraphics() {
            return backBufferGraphics;
        }

        void VisibleBenchmark(bool fps) {
            this->fps = fps;
        }

        bool IsBenchmark() {
            return fps;
        }

    private:
        Size bufferSize;

        byte* backBuffer;
        HBITMAP backBufferHBitmap;
        HDC backBufferHDC;
        Graphics* backBufferGraphics;

        std::function<void(Graphics&)> paint;
        bool fps;

    public:
        ~GdiPlate() {
            delete[] backBuffer;
            DeleteObject(backBufferHBitmap);

            DeleteDC(backBufferHDC);
            delete backBufferGraphics;
        }
    };

}