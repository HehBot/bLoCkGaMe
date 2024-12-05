#include "world.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

bool World::mesh_has_changed(int x, int y, int z)
{
    ChunkPos curr_pos = { x / Chunk::size, y / Chunk::size, z / Chunk::size };
    return !(last_mesh_valid && curr_pos == last_pos);
}

std::ostream& operator<<(std::ostream& o, World::ChunkPos p)
{
    return o << "Chunk(" << p.x << ',' << p.y << ',' << p.z << ')';
}

std::vector<float> World::mesh(int x, int y, int z)
{
    ChunkPos curr_pos = { x / Chunk::size, y / Chunk::size, z / Chunk::size };
    if (last_mesh_valid && curr_pos == last_pos)
        return last_mesh;

    last_pos = curr_pos;
    last_mesh.clear();

    for (int i = -RENDER_DISTANCE_CHUNKS; i < RENDER_DISTANCE_CHUNKS; ++i) {
        for (int j = -RENDER_DISTANCE_CHUNKS; j < RENDER_DISTANCE_CHUNKS; ++j) {
            for (int k = -RENDER_DISTANCE_CHUNKS; k < RENDER_DISTANCE_CHUNKS; ++k) {
                Chunk const* c = cm.get_chunk(ChunkPos { last_pos.x + i, last_pos.y + j, last_pos.z + k });
                if (c == nullptr)
                    continue;
                std::vector<float> m = c->mesh();
                last_mesh.insert(last_mesh.end(), m.begin(), m.end());
            }
        }
    }

    last_mesh_valid = true;
    return last_mesh;
}

World::ChunkManager::ChunkManager(std::fstream& file)
    : file(file)
{
    struct Entry {
        int32_t x, y, z;
        uint64_t offset;
    } __attribute__((packed));
    constexpr size_t size_of_chunk_in_file = Chunk::size * Chunk::size * Chunk::size;
    constexpr size_t max_nr_entries_in_table = (size_of_chunk_in_file - sizeof(uint64_t)) / sizeof(Entry);

    for (int i = 0; i < CHUNK_CACHE_SIZE; ++i)
        lru[i] = std::numeric_limits<uint16_t>::max();

    static_assert(sizeof(nr_chunks) == 4);
    file.read((char*)&nr_chunks, 4);

    uint64_t table_offset;
    static_assert(sizeof(table_offset) == 8);
    file.read((char*)&table_offset, 8);

    /*
     * TODO process free bitmap
     */

    file.seekg(table_offset);

    uint32_t nr_chunks_remaining = nr_chunks;
    while (true) {
        size_t nr_remaining_in_this_table = max_nr_entries_in_table;
        if (nr_remaining_in_this_table > nr_chunks_remaining)
            nr_remaining_in_this_table = nr_chunks_remaining;
        Entry table[max_nr_entries_in_table];

        file.read((char*)&table[0], nr_remaining_in_this_table * sizeof(Entry));

        for (size_t i = 0; i < nr_remaining_in_this_table; ++i)
            file_offsets[ChunkPos { table[i].x, table[i].y, table[i].z }] = table[i].offset;

        nr_chunks_remaining -= nr_remaining_in_this_table;

        if (nr_chunks_remaining > 0) {
            file.read((char*)&table_offset, sizeof(table_offset));
            file.seekg(table_offset);
        } else
            break;
    }
    nr_chunk_offsets_loaded = nr_chunks - nr_chunks_remaining;
}

World::Chunk const* World::ChunkManager::get_chunk(ChunkPos p)
{
    if (index.count(p) > 0)
        return &chunks[index[p]];

    if (file_offsets.count(p) == 0)
        return nullptr;

    uint16_t max = std::numeric_limits<uint16_t>::min();
    size_t new_index = -1;
    for (size_t i = 0; i < CHUNK_CACHE_SIZE; ++i) {
        if (lru[i] > max) {
            new_index = i;
            max = lru[i];
        }
    }

    Chunk* c = &chunks[new_index];

    /*
     * update index map
     */
    if (lru[new_index] != std::numeric_limits<uint16_t>::max()) {
        std::cout << "Unloaded " << c->p << '\n';
        index.erase(c->p);
    }
    index[p] = new_index;

    /*
     * update lru queue
     */
    for (size_t i = 0; i < CHUNK_CACHE_SIZE; ++i)
        if (lru[i] < lru[new_index])
            lru[i]++;
    lru[new_index] = 0;

    file.seekg(file_offsets[p]);
    c->load(p, file);

    std::cout << "Loaded " << p << '\n';

    return c;
}

void World::Chunk::load(ChunkPos p, std::istream& i)
{
    this->p = p;
    i.read((char*)&blocks[0][0][0], sizeof(blocks));
    last_mesh_valid = false;
}

static int constexpr cube[6][6][5] = {
    { // +x
      // coord   tex
      { 1, 1, 1, 0, 1 },
      { 1, 0, 0, 1, 0 },
      { 1, 1, 0, 1, 1 },
      { 1, 0, 0, 1, 0 },
      { 1, 1, 1, 0, 1 },
      { 1, 0, 1, 0, 0 } },
    { // -x
      { 0, 1, 1, 1, 1 },
      { 0, 1, 0, 0, 1 },
      { 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0 },
      { 0, 0, 1, 1, 0 },
      { 0, 1, 1, 1, 1 } },
    { // +y
      { 0, 1, 0, 0, 1 },
      { 1, 1, 1, 1, 0 },
      { 1, 1, 0, 1, 1 },
      { 1, 1, 1, 1, 0 },
      { 0, 1, 0, 0, 1 },
      { 0, 1, 1, 0, 0 } },
    { // -y
      { 0, 0, 0, 1, 1 },
      { 1, 0, 0, 0, 1 },
      { 1, 0, 1, 0, 0 },
      { 1, 0, 1, 0, 0 },
      { 0, 0, 1, 1, 0 },
      { 0, 0, 0, 1, 1 } },
    { // +z
      { 0, 0, 1, 0, 0 },
      { 1, 0, 1, 1, 0 },
      { 1, 1, 1, 1, 1 },
      { 1, 1, 1, 1, 1 },
      { 0, 1, 1, 0, 1 },
      { 0, 0, 1, 0, 0 } },
    { // -z
      { 0, 0, 0, 1, 0 },
      { 1, 1, 0, 0, 1 },
      { 1, 0, 0, 0, 0 },
      { 1, 1, 0, 0, 1 },
      { 0, 0, 0, 1, 0 },
      { 0, 1, 0, 1, 1 } },
};

std::vector<float> World::Chunk::mesh() const
{
    if (!last_mesh_valid) {
        size_t n = 0;
        last_mesh.clear();
        last_mesh.reserve(size * size * size * 6 * 6 * 5);
        for (int x = 0; x < size; ++x)
            for (int y = 0; y < size; ++y)
                for (int z = 0; z < size; ++z) {
                    BlockType c = blocks[x][y][z];
                    if (c != BlockType::AIR) {
                        n++;
                        for (size_t f = 0; f < 6; ++f)
                            for (size_t v = 0; v < 6; ++v) {
                                last_mesh.push_back(p.x * size + x + cube[f][v][0]);
                                last_mesh.push_back(p.y * size + y + cube[f][v][1]);
                                last_mesh.push_back(p.z * size + z + cube[f][v][2]);
                                last_mesh.push_back(block_tex_x(c, f, cube[f][v][3]));
                                last_mesh.push_back(block_tex_y(c, f, cube[f][v][4]));
                            }
                    }
                }
        last_mesh_valid = true;
        std::cout << '[' << n << " blocks in " << p << "\n";
    }
    return last_mesh;
}
