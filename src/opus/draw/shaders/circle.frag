#version 430 core

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_tex_coord;
layout(location = 2) in vec3 a_color;

struct ShaderData
{
    mat4 model;
    vec4 color;
    float thickness;
};

layout (std140, binding = 0) uniform Global
{
    mat4 g_projection;
    mat4 g_view;
    vec4 g_time;
};

layout (std140, binding = 1) uniform Texture
{
    vec2 texture_size;
    float texture_layer_count;
};

layout (std140, binding = 2) buffer Custom
{
    ShaderData data[];
};

/* Vertex Data */
in layout(location = 3) vec2 v_tex_coord;
in layout(location = 4) vec4 v_color;
in layout(location = 5) float v_thickness;

out vec4 color;

void main() 
{
    vec2 uv = v_tex_coord;
    uv = uv * 2 - 1;
    float thickness = v_thickness; 
    
    float d = distance(uv, vec2(0.0, 0.0));
    float a = 1 - smoothstep(1.0 - 0.05, 1.0, d);
    a -= 1 - smoothstep(1.0 - thickness - 0.05, 1 - thickness, d);
    color = vec4(v_color.rgb, a * v_color.a);
}
