import os

COMMENT = """<PVS comment should be here>"""

c_files = filter(lambda x: True if x[-2:] == ".c" else False, os.listdir("."))

for file in c_files:
    cont = None
    with open(file, "r") as f:
        cont = f.read()
    if COMMENT not in cont:
        with open(file, "w") as f:
            f.write(COMMENT + cont)
