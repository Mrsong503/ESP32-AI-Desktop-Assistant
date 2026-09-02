import subprocess, sys, os, glob

REAL = glob.glob('c:/Users/13455/Desktop/Eliya*/reference/xiaozhi-esp32')[0]
os.makedirs('c:/aht_logs', exist_ok=True)
LOG = 'c:/aht_logs/once.txt'
SCRIPT = os.path.join(REAL, 'scripts', 'build_aht20.py')
PY = r'C:\Espressif\python_env\idf5.5_py3.11_env\Scripts\python.exe'

out = open(LOG, 'w', encoding='utf-8', errors='replace')
r = subprocess.run([PY, SCRIPT], capture_output=True, text=True,
                   encoding='utf-8', errors='replace',
                   cwd=os.path.dirname(os.path.abspath(__file__)))
out.write('RC=' + str(r.returncode) + '\n')
out.write('---STDOUT---\n' + r.stdout[-10000:])
out.write('---STDERR---\n' + r.stderr[-6000:])
out.close()
sys.exit(r.returncode)