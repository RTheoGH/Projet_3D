#version 330 core
in vec3 v_position;
in vec3 v_normal;
in vec2 v_uv;

out vec4 fragColor;

uniform vec3 light_position;
uniform sampler2D albedo;

void main() {
   vec3 L = normalize(light_position - v_position);
   float NL = max(dot(normalize(v_normal), L), 0.0);
   vec3 color = texture(albedo, v_uv).rgb;
   vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);
   fragColor = vec4(col, 1.0);
}
