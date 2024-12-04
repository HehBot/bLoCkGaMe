#ifndef BLOCK_H
#define BLOCK_H

#include <cstdint>
#include <iosfwd>

enum class BlockType : uint8_t {
    AIR = 0,
    DIRT = 1,
    GRASS = 2,
};

float block_tex_x(BlockType bt, int face, int in_tex_x);
float block_tex_y(BlockType bt, int face, int in_tex_y);

std::istream& operator>>(std::istream&, BlockType&);

#endif // BLOCK_H
