@echo off
echo === Checking build status ===
if exist C:\xiaozhi-build\build\firmware.bin (
    echo firmware.bin EXISTS
) else (
    echo firmware.bin MISSING
)
echo === Done ===
