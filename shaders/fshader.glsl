#version 330 core
in vec3 v_position;
in vec3 v_normal;
in vec2 v_uv;
in float v_height;
in mat4 v_mvp;

out vec4 fragColor;

uniform vec3 light_position;
uniform sampler2D albedo;
uniform sampler2D current_hm;
uniform sampler2D heightmapSand;
uniform sampler2D heightmapWater;
uniform sampler2D heightmapLava;
uniform int hm_index;

void main() {


   // if(texture(current_hm, v_uv).r < texture(heightmapSand, v_uv).r ||
   //    texture(current_hm, v_uv).r < texture(heightmapWater, v_uv).r ||
   //    texture(current_hm, v_uv).r < texture(heightmapLava, v_uv).r)
   // {
   //    discard;
   // }

   // vec3 light_pos = (v_mvp * vec4(light_position, 1)).rgb;
   // vec3 v_pos = (v_mvp * vec4(v_position, 1)).rgb;
   vec3 L = normalize(light_position - v_position);
   float NL = max(dot(normalize(v_normal), L), 0.0);
   vec3 color = texture(albedo, v_uv).rgb;
   float alpha = 1.0;
   if(hm_index == 1){
      float intensity = max(texture(current_hm, v_uv).r - texture(heightmapSand, v_uv).r, texture(current_hm, v_uv).r - texture(heightmapLava, v_uv).r);
      vec3 deg_blue = vec3(intensity, intensity, 1.0);
      alpha = mix(1.0, 0.0, intensity);
      alpha = clamp(alpha, 0.6, 1.0);
      color = 0.4*color + 0.6*deg_blue;
   }
   vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);
   fragColor = vec4(v_height, v_height, v_height, 1.0);
   fragColor = vec4(col, alpha);
   // float ndcDepth = (2.0 * gl_FragCoord.z - gl_DepthRange.near - gl_DepthRange.far) / (gl_DepthRange.far - gl_DepthRange.near);
   // float clipDepth = ndcDepth / gl_FragCoord.w;
   // vec4 c = vec4((clipDepth * 0.5) + 0.5);
   // fragColor = vec4(c.x, c.y, c.z, 1.0);
}
