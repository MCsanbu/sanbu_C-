// Cyberpunk-style 24-hour desktop clock for Windows.
// Build (MinGW): g++ -std=c++17 -municode -mwindows cyber_clock.cpp -o cyber_clock.exe -lgdi32

#define UNICODE
#define _UNICODE
#include <windows.h>

#include <ctime>
#include <iterator>

namespace {
constexpr int kWidth = 540;
constexpr int kHeight = 116;
constexpr UINT_PTR kClockTimer = 1;

COLORREF Blend(COLORREF from, COLORREF to, int amount, int scale = 255) {
    return RGB(GetRValue(from) + (GetRValue(to) - GetRValue(from)) * amount / scale,
               GetGValue(from) + (GetGValue(to) - GetGValue(from)) * amount / scale,
               GetBValue(from) + (GetBValue(to) - GetBValue(from)) * amount / scale);
}

void FillVerticalGradient(HDC dc, const RECT& area, COLORREF top, COLORREF bottom) {
    for (int y = area.top; y < area.bottom; ++y) {
        const int amount = (y - area.top) * 255 / (area.bottom - area.top);
        HPEN pen = CreatePen(PS_SOLID, 1, Blend(top, bottom, amount));
        HGDIOBJ old = SelectObject(dc, pen);
        MoveToEx(dc, area.left, y, nullptr);
        LineTo(dc, area.right, y);
        SelectObject(dc, old);
        DeleteObject(pen);
    }
}

void DrawGlowText(HDC dc, const wchar_t* text, RECT rect, HFONT font) {
    HGDIOBJ oldFont = SelectObject(dc, font);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(255, 15, 171));
    for (int offset = 5; offset >= 2; --offset) {
        RECT glow = rect;
        OffsetRect(&glow, offset, 0);
        DrawTextW(dc, text, -1, &glow, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        OffsetRect(&glow, -2 * offset, 0);
        DrawTextW(dc, text, -1, &glow, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    SetTextColor(dc, RGB(82, 252, 255));
    DrawTextW(dc, text, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(dc, oldFont);
}

void PaintClock(HWND window) {
    PAINTSTRUCT ps{};
    HDC screen = BeginPaint(window, &ps);
    HDC buffer = CreateCompatibleDC(screen);
    HBITMAP bitmap = CreateCompatibleBitmap(screen, kWidth, kHeight);
    HGDIOBJ oldBitmap = SelectObject(buffer, bitmap);

    RECT client{0, 0, kWidth, kHeight};
    FillVerticalGradient(buffer, client, RGB(9, 7, 28), RGB(18, 5, 37));

    // Subtle scan lines and a cyan/magenta technical frame.
    for (int y = 8; y < kHeight; y += 8) {
        HPEN line = CreatePen(PS_SOLID, 1, RGB(29, 19, 57));
        HGDIOBJ old = SelectObject(buffer, line);
        MoveToEx(buffer, 0, y, nullptr); LineTo(buffer, kWidth, y);
        SelectObject(buffer, old); DeleteObject(line);
    }
    HPEN cyan = CreatePen(PS_SOLID, 2, RGB(54, 242, 255));
    HGDIOBJ oldPen = SelectObject(buffer, cyan);
    HBRUSH oldBrush = static_cast<HBRUSH>(SelectObject(buffer, GetStockObject(HOLLOW_BRUSH)));
    Rectangle(buffer, 5, 5, kWidth - 6, kHeight - 6);
    SelectObject(buffer, oldBrush); SelectObject(buffer, oldPen); DeleteObject(cyan);

    std::time_t raw = std::time(nullptr);
    std::tm local{};
    localtime_s(&local, &raw);
    wchar_t timeText[16];
    wcsftime(timeText, std::size(timeText), L"%H:%M:%S", &local);

    HFONT timeFont = CreateFontW(64, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FIXED_PITCH, L"Consolas");
    DrawGlowText(buffer, timeText, RECT{34, 19, kWidth - 34, 93}, timeFont);
    DeleteObject(timeFont);

    HFONT labelFont = CreateFontW(13, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    HGDIOBJ oldFont = SelectObject(buffer, labelFont);
    SetBkMode(buffer, TRANSPARENT); SetTextColor(buffer, RGB(255, 69, 191));
    TextOutW(buffer, 19, 11, L"SYSTEM TIME // 24H", 18);
    SelectObject(buffer, oldFont); DeleteObject(labelFont);

    BitBlt(screen, 0, 0, kWidth, kHeight, buffer, 0, 0, SRCCOPY);
    SelectObject(buffer, oldBitmap); DeleteObject(bitmap); DeleteDC(buffer);
    EndPaint(window, &ps);
}
}  // namespace

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
    const wchar_t* className = L"CyberClockWindow";
    WNDCLASSW wc{};
    wc.hInstance = instance;
    wc.lpszClassName = className;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpfnWndProc = [](HWND window, UINT message, WPARAM wParam, LPARAM lParam) -> LRESULT {
        switch (message) {
        case WM_PAINT: PaintClock(window); return 0;
        case WM_TIMER: InvalidateRect(window, nullptr, FALSE); return 0;
        case WM_NCHITTEST: return HTCAPTION; // Drag the clock from any point.
        case WM_RBUTTONUP: DestroyWindow(window); return 0;
        case WM_DESTROY: PostQuitMessage(0); return 0;
        default: return DefWindowProcW(window, message, wParam, lParam);
        }
    };
    RegisterClassW(&wc);

    const int left = (GetSystemMetrics(SM_CXSCREEN) - kWidth) / 2;
    HWND window = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, className, L"Cyber Clock",
        WS_POPUP, left, 0, kWidth, kHeight, nullptr, nullptr, instance, nullptr);
    if (!window) return 1;
    SetTimer(window, kClockTimer, 250, nullptr);
    ShowWindow(window, SW_SHOWNOACTIVATE);
    UpdateWindow(window);

    MSG message;
    while (GetMessageW(&message, nullptr, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    return static_cast<int>(message.wParam);
}
