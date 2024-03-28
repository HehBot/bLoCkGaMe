f = open("chunk", "w")
cz = 32


def color(x, y, z):
    s = x + y + z
    if s <= 20:
        return 1
    elif s <= 40:
        return 2
    elif s <= 60:
        return 3
    else:
        return 0


for x in range(cz):
    for y in range(cz):
        for z in range(cz - 1):
            f.write(chr(ord("0") + color(x, y, z)) + " ")
        f.write(chr(ord("0") + color(x, y, cz - 1)))
        f.write("\n")
f.close()
