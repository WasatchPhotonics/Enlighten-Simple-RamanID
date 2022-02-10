import sys

if len(sys.argv) < 2:
    print("Usage: just-intensity file.csv")
    sys.exit()

pathname = sys.argv[1]

values = []

with open(pathname) as f:
    state = "metadata"
    for line in f:
        line = line.strip()
        if state == "metadata":
            if line.startswith("Wavenumber"):
                state = "data"
        else:
            tok = line.split(',')
            if (len(tok) == 2):
                value = round(float(tok[1]))
                if value < 0:
                    value = 0
                elif value > 0xffff:
                    value = 0xffff
                values.append(str(value))

delimited = ", ".join(values)
print("static unsigned short spectrum[] = { %s };" % delimited)
