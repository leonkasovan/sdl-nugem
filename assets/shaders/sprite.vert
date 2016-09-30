#version 330 core
// layout (location = 0)
in vec2 position;
in vec2 v_texCoords;
out vec2 f_texCoords;

uniform mat4 mvp;

void main()
{
    f_texCoords = v_texCoords;
    gl_Position = mvp * vec4(position, 0.0, 1.0);
}
