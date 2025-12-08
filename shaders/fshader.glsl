#version 430 core
in vec3 v_world_pos;
in vec3 v_world_normal;
in vec2 v_uv;
in float v_height;

out vec4 fragColor;

uniform vec3 light_position;
uniform sampler2D current_hm;
uniform sampler2D heightmapSand;
uniform sampler2D heightmapWater;
uniform sampler2D heightmapLava;
layout(binding = 4) uniform sampler2D albedo;
uniform sampler2D grassTexture;
uniform int hm_index;

// sampler2D get_heightmap(int index){
//    switch(hm_index){
//       case 0:
//          return heightmapSand;
//          break;
//       case 1:
//          return heightmapWater;
//          break;
//       case 2:
//          return heightmapLava;
//          break;
//       default:
//          return heightmapSand;
//          break;
//    }
// }

void main() {
   vec3 light_pos = light_position;
   light_pos.y -= 1.0;
   vec3 L = normalize(light_pos - v_world_pos);
   float NL = max(dot(normalize(v_world_normal), L), 0.0);
   vec3 color = texture(albedo, v_uv).rgb;
   // float height_world = v_world_pos.y;
   // vec3 color = vec3(v_height);

   float alpha = 1.0;
   if(hm_index == 1){
      float intensity = max(texture(current_hm, v_uv).r - texture(heightmapSand, v_uv).r, texture(current_hm, v_uv).r - texture(heightmapLava, v_uv).r);
      vec3 deg_blue = vec3(intensity, intensity, 1.0);
      alpha = mix(1.0, 0.0, intensity);
      alpha = clamp(alpha, 0.6, 1.0);
      color = 0.4*color + 0.6*deg_blue;
   }

   if(hm_index == 2){
      float waterHeight = texture(heightmapWater, v_uv).r;
      float dirtHeight = texture(current_hm, v_uv).r;

      float heightDiff = dirtHeight - waterHeight;

      float grassFactor = 0.0;
      if (heightDiff < 0.50) {
         grassFactor = smoothstep(0.50, 0.0, heightDiff);
      }
      if (dirtHeight < waterHeight) {
          grassFactor = 0.0;
      }

      vec3 dirtColor = texture(albedo, v_uv).rgb;
      vec3 grassColor = texture(grassTexture, v_uv).rgb;

      color = mix(dirtColor, grassColor, grassFactor);
   }

   vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);
   fragColor = vec4(col, alpha);
   // fragColor = vec4(col, 1.0) * 0.0 + vec4(v_uv.x, v_uv.y, 0.0, 1.0) * 1.0;
}
