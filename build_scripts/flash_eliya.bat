@echo off
setlocal
"C:\Espressif\python_env\idf5.5_py3.11_env\Scripts\python.exe" -m esptool --chip esp32s3 -p COM8 -b 460800 --before default_reset --after hard_reset write_flash 0x0 C:\xiaozhi-build\build\merged-binary.bin > C:\xiaozhi-build\build\flash_eliya.log 2>&1
echo FLASH_EXIT=%errorlevel%
endlocal