from random import randint
import pickle

f = open("world", "wb")
chunk_size = 32
file_area_size = chunk_size**3
max_entries_in_single_table = file_area_size // 20


def color(x, y, z):
    r = randint(0, 100) / 100
    if r > y / chunk_size:
        return 1
    elif 2 * r > y / chunk_size:
        return 2
    return 0


offsets = dict()

next_free_file_area = 1

for x in range(-3, 3):
    for y in range(0, 2):
        for z in range(-3, 3):
            offsets[(x, y, z)] = next_free_file_area * file_area_size
            next_free_file_area += 1

for chunk, offset in offsets.items():
    cx, cy, cz = chunk
    c = []
    for x in range(chunk_size):
        for y in range(chunk_size):
            for z in range(chunk_size):
                c.append(color(x, y, z))
    f.seek(offset)
    f.write(bytearray(c))

s = b""
n = 0

first_table = None
last_table = None


def emit_table():
    global first_table
    global last_table
    global s
    global n
    global next_free_file_area
    global file_area_size
    global f
    this_table = next_free_file_area * file_area_size
    next_free_file_area += 1

    if first_table == None:
        first_table = this_table
    if last_table != None:
        f.seek(last_table + file_area_size - 8)
        f.write(this_table.to_bytes(8, byteorder="little"))

    print("Emitting table of size", len(s) / 5, "to", this_table)

    f.seek(this_table)
    f.write(s)
    s = b""
    n = 0


for chunk, offset in offsets.items():
    cx, cy, cz = chunk
    s += (
        cx.to_bytes(4, "little", signed=True)
        + cy.to_bytes(4, "little", signed=True)
        + cz.to_bytes(4, "little", signed=True)
        + offset.to_bytes(8, "little")
    )
    n += 1
    if n == max_entries_in_single_table:
        emit_table()
emit_table()

f.seek(0)
f.write((len(offsets)).to_bytes(4, "little"))
f.write(first_table.to_bytes(8, byteorder="little"))

f.close()

f = open("chunkmap", "wb")
pickle.dump(offsets, f)
f.close()
