from random import randint

f = open("world", "wb")
cz = 32


def color(x, y, z):
    r = randint(0, 100) / 100
    if r > y / cz:
        return 1
    elif 2 * r > y / cz:
        return 2
    return 0


f.write((1).to_bytes(4, "little"))
f.write(("0" * 32764).encode("utf-8"))

f.write((0).to_bytes(4, "little"))
f.write((0).to_bytes(4, "little"))
f.write((0).to_bytes(4, "little"))
f.write((32768 * 2).to_bytes(8, "little"))
f.write(("0" * (32768 - 20)).encode("utf-8"))

for x in range(cz):
    for y in range(cz):
        for z in range(cz):
            f.write(color(x, y, z).to_bytes(1, "little"))
f.close()
