from random import randint

f = open("chunk", "w")
cz = 32


def color(x, y, z):
    x = x >= cz / 3 and x <= 2 * cz / 3
    y = y >= cz / 3 and y <= 2 * cz / 3
    z = z >= cz / 3 and z <= 2 * cz / 3
    if (
        (x and y and not z)
        or (x and not y and z)
        or (not x and y and z)
        or (x and y and z)
    ):
        return 1
    return 2

    # if (2 * randint(0, 100) / 100 > y / cz):
    #     return 1
    # return 0

    # s = (x + y + z)//2
    # if s <= 20:
    #     return 1
    # elif s <= 40:
    #     return 2
    # elif s <= 60:
    #     return 3
    # else:
    #     return 1


for x in range(cz):
    for y in range(cz):
        for z in range(cz - 1):
            f.write(chr(ord("0") + color(x, y, z)) + " ")
        f.write(chr(ord("0") + color(x, y, cz - 1)))
        f.write("\n")
f.close()
