import subprocess, os, sys

IDF_PATH = r"C:\Espressif\frameworks\esp-idf-v5.5.5"
VENV_PY = r"C:\Espressif\python_env\idf5.5_py3.11_env\Scripts\python.exe"
BUILD_DIR = r"C:\xiaozhi-build"
LOG_FILE = r"C:\Users\13455\Desktop\Eliya‘s mind\build_scripts\build_eliya.log"

env = os.environ.copy()
env["IDF_PATH"] = IDF_PATH
env["IDF_PYTHON_ENV_PATH"] = r"C:\Espressif\python_env\idf5.5_py3.11_env"
env["PATH"] = (
    r"C:\Espressif\tools\xtensa-esp-elf\esp-14.2.0_20260121\xtensa-esp-elf\bin;"
    r"C:\Espressif\tools\cmake\3.24.0\bin;"
    r"C:\Espressif\tools\ninja\1.12.1;"
    r"C:\Espressif\tools\python\3.11.2;"
    r"C:\Program Files\Git\cmd;"
    + env.get("PATH", "")
)

print("PATH prefix ok. IDF exists:", os.path.exists(IDF_PATH))
print("cmake on path:", any("cmake\\3.24.0\\bin" in p for p in env["PATH"].split(";")))

with open(LOG_FILE, "w") as log:
    r = subprocess.run(
        [VENV_PY, "scripts/build.py", "bread-compact-wifi", "--name", "bread-compact-wifi"],
        cwd=BUILD_DIR, env=env, stdout=log, stderr=subprocess.STDOUT
    )
    log.write(f"\nBUILD_EXIT={r.returncode}\n")

print(f"BUILD_EXIT={r.returncode}")
sys.exit(r.returncode)
