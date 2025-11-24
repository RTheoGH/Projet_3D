#version 330 core
in vec4 vertex;
in vec3 normal;
in vec2 uv;

out vec3 v_position;
out vec3 v_normal;
out vec2 v_uv;

uniform mat4 mvp_matrix;
uniform mat3 normal_matrix;
uniform sampler2D heightmap;

void main() {

    float eps = 0.01;

    v_position = vertex.xyz;
    v_position.y = -texture(heightmap, uv).r;

    float hx1 = texture(heightmap, uv + vec2(eps, 0.0)).r;
    float hy1 = texture(heightmap, uv + vec2(0.0, eps)).r;

    float hx2 = texture(heightmap, uv - vec2(eps, 0.0)).r;
    float hy2 = texture(heightmap, uv - vec2(0.0, eps)).r;

    vec3 dx = vec3(2.0*eps, hx1-hx2, 0.0);
    vec3 dy = vec3(0.0, hy1-hy2, 2.0*eps);

    vec3 new_normal = -normalize(cross(dy, dx));

    v_normal = normal_matrix * new_normal;
    v_uv = uv;

    // Calculate vertex position in screen space
    gl_Position = mvp_matrix * vec4(v_position, 1);
}
