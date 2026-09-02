@echo off
del /q build_logs\inc_done.txt 2>nul
C:\Espressif\python_env\idf5.5_py3.11_env\Scripts\python.exe incremental_build.py > build_logs\inc.txt 2>&1
echo %errorlevel% > build_logs\inc_done.txt
