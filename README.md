# Cyber Clock

A small Windows desktop clock that stays centered at the top of the screen. It
uses a 24-hour digital display with cyan and magenta neon styling, scan lines,
and a technical frame.

## Build and run

Build it with MinGW-w64:

```bash
x86_64-w64-mingw32-g++ -std=c++17 -municode -mwindows cyber_clock.cpp -o cyber_clock.exe -lgdi32
```

Run `cyber_clock.exe`. Drag anywhere on the clock to move it; right-click it to
close it.
