@echo off
setlocal
set IDF_PATH=C:\Espressif\frameworks\esp-idf-v5.5.5
set VENV_PY=C:\Espressif\python_env\idf5.5_py3.11_env\Scripts\python.exe
set LOGDIR=c:\Users\13455\Desktop\Eliya‘s mind\build_logs

echo === call export.bat ===
call "%IDF_PATH%\export.bat" > "%LOGDIR%\b_export.txt" 2>&1

echo === copy modified file (belt and braces) ===
copy /Y "c:\Users\13455\Desktop\Eliya‘s mind\reference\xiaozhi-esp32\main\boards\bread-compact-wifi\compact_wifi_board.cc" "C:\xiaozhi-build\main\boards\bread-compact-wifi\compact_wifi_board.cc"
copy /Y "c:\Users\13455\Desktop\Eliya‘s mind\reference\xiaozhi-esp32\scripts\build_aht20.py" "C:\xiaozhi-build\scripts\build_aht20.py" >nul 2>&1

echo === build ===
cd /d C:\xiaozhi-build
"%VENV_PY%" "%IDF_PATH%\tools\idf.py" build > "%LOGDIR%\b_build.txt" 2>&1
set CODE=%ERRORLEVEL%
echo BUILD_EXIT=%CODE% > "%LOGDIR%\b_exit.txt"

echo === copy back ===
if exist "c:\Users\13455\Desktop\Eliya‘s mind\reference\xiaozhi-esp32\build" rmdir /s /q "c:\Users\13455\Desktop\Eliya‘s mind\reference\xiaozhi-esp32\build"
xcopy /e /i /y "C:\xiaozhi-build\build" "c:\Users\13455\Desktop\Eliya‘s mind\reference\xiaozhi-esp32\build" >nul 2>&1
echo DONE > "%LOGDIR%\b_done.txt"
endlocal