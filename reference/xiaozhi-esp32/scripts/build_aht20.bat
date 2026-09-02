@echo off
REM 编译 bread-compact-wifi（含 AHT20 支持）
REM 用法: scripts\build_aht20.bat

set IDF_PATH=C:\Espressif\frameworks\esp-idf-v5.5.5
set IDF_PYTHON=C:\Espressif\python_env\idf5.5_py3.11_env\Scripts\python.exe
set PROJECT=%~dp0..

cd /d "%PROJECT%"

echo === 编译 bread-compact-wifi ===
"%IDF_PYTHON%" -m idf.py build -B build -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;boards/bread-compact-wifi/config.json" -DBOARD_TYPE_BREAD_COMPACT_WIFI=y -DOLED_SSD1306_128X32=y

echo === 合并二进制 ===
"%IDF_PYTHON%" -m idf.py merge-bin

echo === 完成 ===
pause