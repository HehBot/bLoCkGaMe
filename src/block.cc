#include "block.h"

#include <iostream>

static float constexpr tex_x_mult = 0.25f, tex_y_mult = 0.25f;
static float constexpr tex_coord[3][6][2] = {
    { { 0.0f, 0.75f }, { 0.0f, 0.75f }, { 0.0f, 0.75f }, { 0.0f, 0.75f }, { 0.0f, 0.75f }, { 0.0f, 0.75f } },
    { { 0.0f, 0.75f }, { 0.0f, 0.75f }, { 0.25f, 0.75f }, { 0.0f, 0.75f }, { 0.0f, 0.75f }, { 0.0f, 0.75f } },
    { { 0.5f, 0.0f }, { 0.5f, 0.0f }, { 0.5f, 0.0f }, { 0.5f, 0.0f }, { 0.5f, 0.0f }, { 0.5f, 0.0f } },
};

float block_tex_x(BlockType bt, int f, int in_tex_x)
{
    return tex_x_mult * in_tex_x + tex_coord[(int)bt - 1][f][0];
}
float block_tex_y(BlockType bt, int f, int in_tex_y)
{
    return tex_y_mult * in_tex_y + tex_coord[(int)bt - 1][f][1];
}

std::istream& operator>>(std::istream& i, BlockType& b)
{
    int t;
    i >> t;
    uint8_t* p = (uint8_t*)&b;
    *p = (unsigned char)t;
    return i;
}
