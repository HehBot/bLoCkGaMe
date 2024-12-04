#version 460 core

out vec4 fragColor;

in vec2 texCoord;

uniform sampler2D tex_atlas;

void main()
{
    fragColor = texture(tex_atlas, texCoord);
}
