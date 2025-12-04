#version 430 core
in vec4 vertex;
in vec3 normal;
in vec2 uv;

out vec3 v_world_pos;
out vec3 v_world_normal;
out vec2 v_uv;
out float v_height;

uniform mat4 model_matrix;      // <- nouveau
uniform mat4 mvp_matrix;
uniform mat3 normal_matrix;     // tu fournis déjà ceci depuis C++
uniform float height_scale;
uniform sampler2D current_hm;

void main()
{
    float eps = 0.01;

    vec3 pos = vertex.xyz;
    v_height = texture(current_hm, uv).r;
    pos.y = -v_height * height_scale;

    // calc normales depuis heightmap (tu as déjà ça)
    float hx1 = texture(current_hm, uv + vec2(eps, 0.0)).r;
    float hy1 = texture(current_hm, uv + vec2(0.0, eps)).r;
    float hx2 = texture(current_hm, uv - vec2(eps, 0.0)).r;
    float hy2 = texture(current_hm, uv - vec2(0.0, eps)).r;

    vec3 dx = vec3(2.0*eps, hx1 - hx2, 0.0);
    vec3 dy = vec3(0.0, hy1 - hy2, 2.0*eps);

    vec3 N = normalize(cross(dy, dx));

    // transform to WORLD
    v_world_pos    = (model_matrix * vec4(pos, 1.0)).xyz;
    v_world_normal = normalize(normal_matrix * N);   // normal_matrix = mat3(inverse(transpose(model_matrix)))

    v_uv = uv;
    gl_Position = mvp_matrix * vec4(pos, 1.0);
}
