#version 430 core
layout(local_size_x = 1, local_size_y = 1) in;

layout(binding = 0, r8) readonly uniform image2D heightmap; // heightmap (R8 normalized)
layout(std430, binding = 7) buffer HitBuf {
    vec4 hit;
};

// uniforms fournis par le CPU
uniform vec3 rayOrigin;
uniform vec3 rayDir;      // doit être normalisé
uniform vec2 terrainSize; // (sizeX, sizeZ) en unités monde, ex (10.0, 10.0)
uniform ivec2 resolution; // (width, height) de la heightmap
uniform float heightScale; // échelle à appliquer à la valeur R (0..1 -> monde)
uniform float tMax;       // distance max à tester
uniform float step;       // pas initial pour ray march
uniform int   refineIters; // iters de bsearch

// remap uv -> texel coords safely
ivec2 uvToTexel(vec2 uv) {
    int tx = int(clamp(uv.x * float(resolution.x), 0.0, float(resolution.x - 1)));
    int ty = int(clamp(uv.y * float(resolution.y), 0.0, float(resolution.y - 1)));
    return ivec2(tx, ty);
}

void main() {
    // init result: no hit
    hit = vec4(0.0, 0.0, 0.0, 0.0);

    // cast ray with coarse marching
    float t = 0.0;
    float prev_t = 0.0;
    bool found = false;

    for (; t <= tMax; t += step) {
        vec3 pos = rayOrigin + t * rayDir;

        // map world pos to uv in [0,1]
        vec2 uv = vec2( (pos.x / terrainSize.x) + 0.5, (pos.z / terrainSize.y) + 0.5 );
        if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {
            prev_t = t;
            continue;
        }

        ivec2 tex = uvToTexel(uv);
        float h = imageLoad(heightmap, tex).r * heightScale;

        if (pos.y <= h) { // entered terrain
            found = true;
            break;
        }
        prev_t = t;
    }

    if (!found) {
        // no intersection within tMax
        hit.w = 0.0;
        return;
    }

    // refine between prev_t and t with binary search/refinement
    float a = max(0.0, prev_t);
    float b = t;
    for (int i = 0; i < refineIters; ++i) {
        float mid = 0.5 * (a + b);
        vec3 pos = rayOrigin + mid * rayDir;
        vec2 uv = vec2( (pos.x / terrainSize.x) + 0.5, (pos.z / terrainSize.y) + 0.5 );

        if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {
            // if out of bounds, move the interval
            a = mid;
            continue;
        }

        ivec2 tex = uvToTexel(uv);
        float h = imageLoad(heightmap, tex).r * heightScale;

        if (pos.y <= h) {
            b = mid;
        } else {
            a = mid;
        }
    }

    float tHit = 0.5 * (a + b);
    vec3 pHit = rayOrigin + tHit * rayDir;

    hit = vec4(pHit, 1.0);
}
