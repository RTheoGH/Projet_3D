#version 430 core
in vec4 vertex;
in vec3 normal;
in vec2 uv;

out vec3 v_position;
out vec3 v_normal;
out vec2 v_uv;
out float v_height;
out mat4 v_mvp;

uniform mat4 mvp_matrix;
uniform mat3 normal_matrix;
uniform sampler2D current_hm;
uniform sampler2D heightmapSand;
uniform sampler2D heightmapWater;
uniform sampler2D heightmapLava;
uniform int hm_index;
uniform float height_scale;


void main() {

    float eps = 0.01;

    v_position = vertex.xyz;
    v_height = textureLod(current_hm, v_uv, 0.0).r;
    v_position.y = -v_height * height_scale;

    float hx1 = texture(current_hm, uv + vec2(eps, 0.0)).r;
    float hy1 = texture(current_hm, uv + vec2(0.0, eps)).r;

    float hx2 = texture(current_hm, uv - vec2(eps, 0.0)).r;
    float hy2 = texture(current_hm, uv - vec2(0.0, eps)).r;

    vec3 dx = vec3(2.0*eps, hx1-hx2, 0.0);
    vec3 dy = vec3(0.0, hy1-hy2, 2.0*eps);

    vec3 new_normal = -normalize(cross(dy, dx));

    v_normal = normal_matrix * new_normal;
    v_uv = uv;
    v_mvp = mvp_matrix;

    gl_Position = mvp_matrix * vec4(v_position, 1);
}
