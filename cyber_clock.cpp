// Cyberpunk-style 24-hour desktop clock for Windows.
// Build (MinGW): g++ -std=c++17 -municode -mwindows cyber_clock.cpp -o cyber_clock.exe -lgdi32

#define UNICODE
#define _UNICODE
#include <windows.h>

#include <ctime>
#include <iterator>

namespace {
constexpr int kWidth = 380;
constexpr int kHeight = 96;
constexpr UINT_PTR kClockTimer = 1;

COLORREF ContrastColor(COLORREF color) {
    // The RGB complement is the requested contrasting color for the sampled backdrop.
    return RGB(255 - GetRValue(color), 255 - GetGValue(color), 255 - GetBValue(color));
}

void PaintClock(HWND window) {
    PAINTSTRUCT ps{};
    HDC screen = BeginPaint(window, &ps);
    HDC buffer = CreateCompatibleDC(screen);
    HBITMAP bitmap = CreateCompatibleBitmap(screen, kWidth, kHeight);
    HGDIOBJ oldBitmap = SelectObject(buffer, bitmap);

    // Black is a color key (configured below), so every non-digit pixel is transparent.
    PatBlt(buffer, 0, 0, kWidth, kHeight, BLACKNESS);

    std::time_t raw = std::time(nullptr);
    std::tm local{};
    localtime_s(&local, &raw);
    wchar_t timeText[16];
    wcsftime(timeText, std::size(timeText), L"%H:%M:%S", &local);

    HFONT timeFont = CreateFontW(68, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FIXED_PITCH, L"Consolas");
    HGDIOBJ oldFont = SelectObject(buffer, timeFont);
    SetBkMode(buffer, TRANSPARENT);
    SIZE characterSize{};
    GetTextExtentPoint32W(buffer, L"0", 1, &characterSize);
    const int startX = (kWidth - characterSize.cx * 8) / 2;
    RECT windowRect{};
    GetWindowRect(window, &windowRect);
    for (int index = 0; index < 8; ++index) {
        // The sample is directly below each character, where this color-key window is transparent.
        const COLORREF behindDigit = GetPixel(screen, windowRect.left + startX + characterSize.cx * index
            + characterSize.cx / 2, windowRect.top + kHeight - 2);
        SetTextColor(buffer, ContrastColor(behindDigit == CLR_INVALID ? RGB(0, 0, 0) : behindDigit));
        TextOutW(buffer, startX + characterSize.cx * index, 8, &timeText[index], 1);
    }
    SelectObject(buffer, oldFont);
    DeleteObject(timeFont);

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
        case WM_ERASEBKGND: return 1;
        case WM_NCHITTEST: return HTCAPTION; // Drag the clock from any point.
        case WM_RBUTTONUP: DestroyWindow(window); return 0;
        case WM_DESTROY: PostQuitMessage(0); return 0;
        default: return DefWindowProcW(window, message, wParam, lParam);
        }
    };
    RegisterClassW(&wc);

    const int left = (GetSystemMetrics(SM_CXSCREEN) - kWidth) / 2;
    HWND window = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED, className, L"Cyber Clock",
        WS_POPUP, left, 0, kWidth, kHeight, nullptr, nullptr, instance, nullptr);
    if (!window) return 1;
    SetLayeredWindowAttributes(window, RGB(0, 0, 0), 0, LWA_COLORKEY);
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
