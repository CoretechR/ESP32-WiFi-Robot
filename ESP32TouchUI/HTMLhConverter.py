with open("TouchESP32.html", "rt") as fin:
    lines = fin.read().splitlines()

#Replace special characters
for i in range(0, len(lines), 1):
    lines[i] = lines[i].replace('"', r'\"')
    lines[i] = lines[i].replace(r'\n', r'\\n')
    lines[i] = lines[i].replace('\°', r'\\°')

#Edit line start and ending
lines[0] = 'static const char canvas_htm[] PROGMEM = "' + lines[0] + '\\n"\\'
for i in range(1, len(lines)-1, 1):
    lines[i] = '"' + lines[i] + '\\n"\\'
lines[len(lines)-1] = '"' + lines[len(lines)-1] + '\\n";'

#Export
with open("canvas_htm.h", 'wt') as file_handler:
    for item in lines:
        file_handler.write("{}\n".format(item))

#print(lines)