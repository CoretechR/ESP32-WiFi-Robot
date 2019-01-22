with open("canvas_htm.h", "rt") as fin:
    lines = fin.read().splitlines()

#Edit line start and ending
lines[0] = lines[0][lines[0].find('"')+1:lines[0].find('\\n"\\')]
for i in range(1, len(lines)-1, 1):
    lines[i] = lines[i][1:lines[i].find('\\n"\\')]
lines[len(lines)-1] = lines[len(lines)-1][1:lines[len(lines)-1].find('\\n";')]

#Replace special characters
for i in range(0, len(lines), 1):
    lines[i] = lines[i].replace(r'\"', '"')
    lines[i] = lines[i].replace(r'\\n', r'\n')
    lines[i] = lines[i].replace(r'\\°', '\°')

#Export
with open("TouchESP32.html", 'wt') as file_handler:
    for item in lines:
        file_handler.write("{}\n".format(item))

#print(lines)