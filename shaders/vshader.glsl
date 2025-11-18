#version 330 core
in vec4 vertex;
in vec3 normal;
in vec2 uv;

out vec3 v_position;
out vec3 v_normal;

uniform mat4 mvp_matrix;
uniform mat3 normal_matrix;
uniform sampler2D heightmap;

void main() {
    v_position = vertex.xyz;
    v_position.y = texture(heightmap, uv).r;
    v_normal = normal_matrix * normal;

    // Calculate vertex position in screen space
    gl_Position = mvp_matrix * vertex;
}
