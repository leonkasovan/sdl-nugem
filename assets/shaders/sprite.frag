#version 330 core
in vec2 f_texCoords;
layout(location = 0) out vec4 color;

uniform usampler2D glSpriteTexture;
// uniform vec3 glSpriteColor;

void main()
{    
    color = /*vec4(glSpriteColor, 1.0) **/ texture(glSpriteTexture, f_texCoords);
}  
