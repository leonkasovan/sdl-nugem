#version 330 core
in vec2 f_texCoords;
layout(location = 0) out uvec4 color;

uniform usampler2D glSpriteTexture;

void main()
{    
    color = texture(glSpriteTexture, f_texCoords);
}  
