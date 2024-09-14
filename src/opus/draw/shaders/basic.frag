#version 430 core

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_tex_coord;
layout(location = 2) in vec4 a_color;

layout (std140, binding = 0) uniform Global
{
    vec4 g_time;
};

layout (std140, binding = 1) uniform Texture
{
    vec2 texture_size;
    float texture_layer_count;
};

uniform mat4 u_model;

/* Vertex Data */
in layout(location = 3) vec2 v_tex_coord;
in layout(location = 4) vec4 v_color;

out vec4 color;

void main() {
    color = v_color;
}
