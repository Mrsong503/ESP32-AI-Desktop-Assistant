@echo off
setlocal
call "C:\Espressif\frameworks\esp-idf-v5.5.5\export.bat" >nul 2>&1
cd /d C:\xiaozhi-build
set IDF_PYTHON_ENV_PATH=C:\Espressif\python_env\idf5.5_py3.11_env
"C:\Espressif\python_env\idf5.5_py3.11_env\Scripts\python.exe" scripts\build.py bread-compact-wifi --name bread-compact-wifi > %~dp0build_eliya.log 2>&1
echo BUILD_EXIT=%errorlevel%
endlocal