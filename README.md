# Cyber Clock

A small Windows desktop clock that stays centered at the top of the screen. Its
background is fully transparent. Each character samples the desktop directly
below it and uses that sampled color's RGB complement, keeping the display
contrasting with what is behind it.

## Build and run

Build it with MinGW-w64:

```bash
x86_64-w64-mingw32-g++ -std=c++17 -municode -mwindows cyber_clock.cpp -o cyber_clock.exe -lgdi32 -lshell32
```

Run `cyber_clock.exe`. The clock stays fixed at the top of the screen. Its
system-tray icon provides an **Exit Cyber Clock** menu item for closing it.
