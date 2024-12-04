#ifndef WORLD_H
#define WORLD_H

#include "block.h"

#include <iosfwd>
#include <map>
#include <vector>

#define RENDER_DISTANCE_CHUNKS 10
#define MAX_CHUNKS (4 * RENDER_DISTANCE_CHUNKS * RENDER_DISTANCE_CHUNKS)

class World {
private:
    struct ChunkPos {
        int x, y, z;
        bool operator==(ChunkPos const& c) const
        {
            return x == c.x && y == c.y && z == c.z;
        }
        bool operator<(ChunkPos const& c) const
        {
            return (x < c.x || (x == c.x && (y < c.y || (y == c.y && z < c.z))));
        }
    };

    struct ChunkManager;

    struct Chunk {
    public:
        constexpr static int size = 32;

        ChunkPos p;
        BlockType blocks[size][size][size];

        mutable std::vector<float> last_mesh;
        mutable bool last_mesh_valid;

    public:
        void load(ChunkPos p, std::istream& i);
        std::vector<float> mesh() const;
    };

    struct ChunkManager {
        uint32_t nr_chunks;

        std::fstream& file;
        size_t next_offset;
        uint32_t nr_chunk_offsets_loaded;
        std::map<ChunkPos, size_t> file_offsets;

        std::map<ChunkPos, size_t> index;
        Chunk chunks[MAX_CHUNKS];
        uint16_t valid_queue[MAX_CHUNKS];

        ChunkManager(std::fstream& file);

        Chunk const* get_chunk(ChunkPos p);
    };

    ChunkManager cm;

    ChunkPos last_pos;
    std::vector<float> last_mesh;
    bool last_mesh_valid;

public:
    World(std::fstream& file)
        : cm(file), last_mesh_valid(false)
    {
    }

    std::vector<float> mesh(int x, int y, int z);
    bool mesh_has_changed(int x, int y, int z);
};

#endif // WORLD_H
