import serial, serial.tools.list_ports, time

for p in serial.tools.list_ports.comports():
    print('PORT:', p.device, p.description)

s = serial.Serial('COM8', 115200, timeout=1)
# 通过 DTR/RTS 触发 ESP32-S3 USB-Serial/JTAG 软复位，抓完整启动日志
s.dtr = False
s.rts = True
time.sleep(0.1)
s.rts = False
time.sleep(0.1)
s.dtr = True

buf = b''
start = time.time()
while time.time() - start < 25:
    n = s.in_waiting
    if n:
        buf += s.read(n)
s.close()

log = buf.decode('utf-8', 'replace')
print('=== LOG ===')
print(log)
# 关键行高亮
for key in ['AHT20', 'MCP', 'mcp', 'CompactWifiBoard', 'i2c', 'E (', 'W (AHT']:
    for line in log.splitlines():
        if key in line:
            print('[KEY]', line)
