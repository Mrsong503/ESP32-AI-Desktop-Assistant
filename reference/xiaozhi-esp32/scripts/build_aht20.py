#!/usr/bin/env python3
"""
编译 bread-compact-wifi（含 AHT20 支持）
Windows 专用：解决项目路径含 Unicode 引号导致 ESP-IDF kconfig 失败的问题。

方案：将项目复制到纯 ASCII 路径 C:\\xiaozhi-build 编译，产物回拷。

用法: python scripts/build_aht20.py
"""
import os
import re
import shutil
import subprocess
import sys

IDF_PATH = 'C:/Espressif/frameworks/esp-idf-v5.5.5'
VENV_PY = 'C:/Espressif/python_env/idf5.5_py3.11_env/Scripts/python.exe'
# 真实项目路径（含 Unicode 左单引号 U+2018）
REAL_PROJECT = os.path.normpath(
    'c:/Users/13455/Desktop/Eliya\u2018s mind/reference/xiaozhi-esp32'
)
# 纯 ASCII 编译目录
BUILD_ROOT = 'C:/xiaozhi-build'


def activate_env():
    """运行 ESP-IDF activate.py，解析输出的环境变量"""
    out = subprocess.run(
        [VENV_PY, os.path.join(IDF_PATH, 'tools', 'activate.py'), '--export', '-s', 'cmd.exe'],
        capture_output=True, text=True, encoding='utf-8', errors='replace'
    )
    m = re.search(r'call\s+(.+\.bat)', out.stdout + out.stderr)
    if not m:
        print('无法找到 activate bat 文件')
        print('STDOUT:', out.stdout)
        print('STDERR:', out.stderr)
        sys.exit(1)
    bat_path = m.group(1).strip()

    env = dict(os.environ)
    with open(bat_path, encoding='utf-8', errors='replace') as f:
        for line in f:
            line = line.strip()
            if line.startswith('set '):
                line = line[4:].strip()
                if '=' in line:
                    key, _, value = line.partition('=')
                    key = key.strip().strip('"')
                    value = value.strip().strip('"')
                    if key:
                        if key.upper() == 'PATH':
                            env['PATH'] = value + ';' + env.get('PATH', '')
                        else:
                            env[key] = value
    env['IDF_PATH'] = IDF_PATH
    return env


def copy_project():
    """将项目复制到纯 ASCII 路径（排除 build/.git/managed_components 等可再生成目录）"""
    if os.path.exists(BUILD_ROOT):
        shutil.rmtree(BUILD_ROOT, ignore_errors=True)

    exclude_dirs = {'build', '.git'}
    os.makedirs(BUILD_ROOT, exist_ok=True)

    for item in os.listdir(REAL_PROJECT):
        src = os.path.join(REAL_PROJECT, item)
        dst = os.path.join(BUILD_ROOT, item)
        if os.path.isdir(src):
            if item in exclude_dirs:
                print(f'跳过目录: {item}')
                continue
            shutil.copytree(src, dst, ignore=shutil.ignore_patterns('*.pyc', '__pycache__'))
        else:
            shutil.copy2(src, dst)
    print(f'项目已复制到 {BUILD_ROOT}')


def build():
    """在 ASCII 路径下编译"""
    os.chdir(BUILD_ROOT)
    result = subprocess.run(
        [VENV_PY, 'scripts/build.py', 'bread-compact-wifi', '--name', 'bread-compact-wifi'],
        env=dict(os.environ, **activate_env())
    )
    return result.returncode


def copy_back():
    """将编译产物复制回真实项目"""
    src_build = os.path.join(BUILD_ROOT, 'build')
    dst_build = os.path.join(REAL_PROJECT, 'build')
    if os.path.exists(dst_build):
        shutil.rmtree(dst_build, ignore_errors=True)
    if os.path.exists(src_build):
        shutil.copytree(src_build, dst_build)

    src_sdkconfig = os.path.join(BUILD_ROOT, 'sdkconfig')
    dst_sdkconfig = os.path.join(REAL_PROJECT, 'sdkconfig')
    if os.path.exists(src_sdkconfig):
        shutil.copy2(src_sdkconfig, dst_sdkconfig)
    print('编译产物已回拷')


def main():
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('--copy-only', action='store_true')
    args = parser.parse_args()

    if args.copy_only:
        print('=== 仅复制项目到 ASCII 路径 ===')
        copy_project()
        sys.exit(0)

    print('=== 1. 激活 ESP-IDF 5.5 环境 ===')
    env = activate_env()
    print('IDF_PATH:', env['IDF_PATH'])

    print('\n=== 2. 复制项目到 ASCII 路径 ===')
    copy_project()

    print('\n=== 3. 编译 bread-compact-wifi ===')
    code = build()

    if code == 0:
        print('\n=== 4. 回拷编译产物 ===')
        copy_back()
        print('\n[OK] 编译成功！')
    else:
        print(f'\n[FAIL] 编译失败 (exit {code})')
    sys.exit(code)


if __name__ == '__main__':
    main()