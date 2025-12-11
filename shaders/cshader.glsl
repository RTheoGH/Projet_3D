#version 430 core

layout(local_size_x = 16, local_size_y = 16) in;

layout(binding = 0, r8) readonly uniform image2D inputImage;
layout(binding = 1, r8) writeonly uniform image2D outputImage;
layout(binding = 2, r8) uniform image2D sand_hm;
layout(binding = 3, r8) readonly uniform image2D water_hm;
layout(binding = 4, r8) readonly uniform image2D lava_hm;
layout(binding = 5, rgba32f) readonly uniform image2D water_velo_in;
layout(binding = 6, rgba32f) writeonly uniform image2D water_velo_out;

uniform int hm_index;
uniform float moy_pix;
uniform int simulationEnabled;

void main() {

    ivec2 size = imageSize(inputImage);
    ivec2 p = ivec2(gl_GlobalInvocationID.xy);

    int x  = clamp(p.x, 0, size.x - 1);
    int y  = clamp(p.y, 0, size.y - 1);
    int xm = clamp(p.x - 1, 0, size.x - 1);
    int xp = clamp(p.x + 1, 0, size.x - 1);
    int ym = clamp(p.y - 1, 0, size.y - 1);
    int yp = clamp(p.y + 1, 0, size.y - 1);

    float c = imageLoad(inputImage, ivec2(x, y)).r;
    float h = imageLoad(inputImage, ivec2(x, ym)).r;
    float b = imageLoad(inputImage, ivec2(x, yp)).r;
    float g = imageLoad(inputImage, ivec2(xm, y)).r;
    float d = imageLoad(inputImage, ivec2(xp, y)).r;
    float hg = imageLoad(inputImage, ivec2(xm, ym)).r;
    float hd = imageLoad(inputImage, ivec2(xp, ym)).r;
    float bg = imageLoad(inputImage, ivec2(xm, yp)).r;
    float bd = imageLoad(inputImage, ivec2(xp, yp)).r;

    float new_val = c;

    if(hm_index == 0){

        float water_c = imageLoad(water_hm, ivec2(x, y)).r;
        // float water_h = imageLoad(water_hm, ivec2(x, ym)).r;
        // float water_b = imageLoad(water_hm, ivec2(x, yp)).r;
        // float water_g = imageLoad(water_hm, ivec2(xm, y)).r;
        // float water_d = imageLoad(water_hm, ivec2(xp, y)).r;
        // float water_hg = imageLoad(water_hm, ivec2(xm, ym)).r;
        // float water_hd = imageLoad(water_hm, ivec2(xp, ym)).r;
        // float water_bg = imageLoad(water_hm, ivec2(xm, yp)).r;
        // float water_bd = imageLoad(water_hm, ivec2(xp, yp)).r;

        float avg = (c + h + b + g + d + hg + hd + bg + bd) / 9.0;

        // new_val = min(c, avg);
        new_val = avg;

        if(water_c > c && simulationEnabled == 1){
//            new_val += 0.01 * moy_pix;
            new_val -= 1.0/255.0;
        }

    }

    if (hm_index == 1)
    {


        if(simulationEnabled == 1){

            // new_val = 0.7 * c + 0.3 * moy_pix;
            new_val = max(c, max(d, max(g, max(h, b))));

            bool is_c_max = (new_val == c);

            if(new_val < imageLoad(sand_hm, ivec2(x, y)).r || new_val < imageLoad(lava_hm, ivec2(x, y)).r){

                new_val = c;

            }

            float sand_c = imageLoad(sand_hm, ivec2(x,y)).r;

            bool water_coming = (c < sand_c) && (new_val >= sand_c);

            // érosion de vague
            if (water_coming) {

                float sand_g = imageLoad(sand_hm, ivec2(xm,y)).r;
                float sand_d = imageLoad(sand_hm, ivec2(xp,y)).r;
                float sand_b = imageLoad(sand_hm, ivec2(x,ym)).r;
                float sand_h = imageLoad(sand_hm, ivec2(x,yp)).r;

                float erosion = 3.0/255.0;

                imageStore(sand_hm, ivec2(x, y), vec4(max(0,sand_c-erosion),0,0,1));
                if(g < sand_g) imageStore(sand_hm, ivec2(xm, y), vec4(max(0,sand_g-erosion),0,0,1));
                if(d < sand_d) imageStore(sand_hm, ivec2(xp, y), vec4(max(0,sand_d-erosion),0,0,1));
                if(b < sand_b) imageStore(sand_hm, ivec2(x, ym), vec4(max(0,sand_b-erosion),0,0,1));
                if(h < sand_h) imageStore(sand_hm, ivec2(x, yp), vec4(max(0,sand_h-erosion),0,0,1));
            }

        }
        // new_val -= 0.01;
        // if(new_val == c){
        //     // new_val -= 0.01;
        // }
    }


    if(hm_index == 2){
        float avg = (c + h + b + g + d + hg + hd + bg + bd) / 9.0;
        new_val = 0.5 * c + 0.5 * min(c, avg);
    }

    imageStore(outputImage, p, vec4(new_val,0,0,1));
}
