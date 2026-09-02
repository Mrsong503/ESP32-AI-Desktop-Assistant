#!/usr/bin/env python3
"""增量编译：只拷贝修改过的文件到 C:/xiaozhi-build，然后运行 ninja"""
import os
import shutil
import subprocess
import sys

REAL_PROJECT = os.path.normpath(
    'c:/Users/13455/Desktop/Eliya\u2018s mind/reference/xiaozhi-esp32'
)
BUILD_ROOT = 'C:/xiaozhi-build'
VENV_PY = 'C:/Espressif/python_env/idf5.5_py3.11_env/Scripts/python.exe'
IDF_PATH = 'C:/Espressif/frameworks/esp-idf-v5.5.5'

# 需要增量更新的文件（从 Unicode 路径拷贝到 ASCII 路径）
FILES_TO_COPY = [
    'main/boards/bread-compact-wifi/compact_wifi_board.cc',
    'main/boards/bread-compact-wifi/aht20_sensor.h',
    'main/boards/bread-compact-wifi/aht20_sensor.cc',
    'main/boards/bread-compact-wifi/config.h',
]

print('=== 拷贝修改的文件到 C:/xiaozhi-build ===')
for rel in FILES_TO_COPY:
    src = os.path.join(REAL_PROJECT, rel)
    dst = os.path.join(BUILD_ROOT, rel)
    if os.path.exists(src):
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        shutil.copy2(src, dst)
        print(f'  拷贝: {rel}')
    else:
        print(f'  跳过(不存在): {rel}')

# 激活 ESP-IDF 环境
print('\n=== 激活 ESP-IDF 5.5 环境 ===')
env = dict(os.environ)
env['IDF_PATH'] = IDF_PATH

# 设置 PATH（从 activate 输出推断）
idf_tools_path = 'C:/Espressif/tools'
esp_tool = 'C:/Espressif/frameworks/esp-idf-v5.5.5'
extra_paths = [
    esp_tool + '/tools',
    'C:/Program Files/Git/cmd',
    idf_tools_path + '/xtensa-esp-elf/esp-14.2.0_20260121/xtensa-esp-elf/bin',
    idf_tools_path + '/cmake/3.24.0/bin',
    idf_tools_path + '/ninja/1.12.1',
    idf_tools_path + '/python/3.11.2/esp32_py_dfu_tool_py_wrapper.sh/src',
]
env['PATH'] = ';'.join(extra_paths) + ';' + env.get('PATH', '')

os.chdir(BUILD_ROOT)

# 运行 ninja 增量编译
print('\n=== 运行增量编译 (ninja) ===')
result = subprocess.run(
    [VENV_PY, os.path.join(IDF_PATH, 'tools', 'idf.py'), 'build'],
    env=env
)
code = result.returncode

if code == 0:
    print('\n=== 编译成功，拷贝回产物 ===')
    dst_build = os.path.join(REAL_PROJECT, 'build')
    if os.path.exists(dst_build):
        shutil.rmtree(dst_build, ignore_errors=True)
    shutil.copytree('C:/xiaozhi-build/build', dst_build)
    print('build 目录已拷贝回')

    # merge-bin
    print('\n=== 生成合并固件 ===')
    r2 = subprocess.run(
        [VENV_PY, os.path.join(IDF_PATH, 'tools', 'idf.py'), 'merge-bin'],
        env=env
    )
    if r2.returncode == 0:
        merged = 'C:/xiaozhi-build/build/merged-binary.bin'
        if os.path.exists(merged):
            shutil.copy2(merged, os.path.join(REAL_PROJECT, 'build/merged-binary.bin'))
            print('merged-binary.bin 已拷贝回')
    print('\n[OK] 编译成功！')
else:
    print(f'\n[FAIL] 编译失败 (exit {code})')
sys.exit(code)
