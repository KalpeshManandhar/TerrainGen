#include "noise.h"

#include <stdlib.h>

static unsigned int random(unsigned int index){
    index = (index<<13)^index;
    return((index * (index *index *15731 +789221)+1376312589)&0x7fffffff);
}


// fade function used by ken perlin
double fade(double t) {
    return t * t * t * (t * (t * 6 - 15) + 10);         // 6t^5 - 15t^4 + 10t^3
}



uint32_t *getPermutationTable(uint32_t n){
    uint32_t *permutationTable = (uint32_t*)malloc(n * sizeof(*permutationTable));
    for (int i=0; i<n; ++i){
        permutationTable[i] = i;
    }
    for (int i=0; i<n; ++i){
        // swap places of i and j
        int j = random(random(i))%n;
        uint32_t temp = permutationTable[i];
        permutationTable[i] = permutationTable[j];
        permutationTable[j] = temp;
    }
    return(permutationTable);
}

float getConstantVector1D(uint32_t v){
    uint32_t h = v%2;
    if (h == 0) return(-1);
    else        return(1);
}

Vec2f getConstantVector2D(uint32_t v){
    uint32_t h = v%8;
    if(h == 0)       return(Vec2f{1,1});
    else if(h == 1)  return(Vec2f{-1,1});
    else if(h == 2)  return(Vec2f{-1,-1});
    else if(h == 3)  return(Vec2f{1,-1});
    else if(h == 4)  return(Vec2f{1.4142,0});
    else if(h == 5)  return(Vec2f{0,1.4142});
    else if(h == 6)  return(Vec2f{-1.4142,0});
    else             return(Vec2f{0,-1.4142});

}

Vec3f getConstantVector3D(uint32_t v){
    uint32_t h = v%12;
    if(h == 0)       return(Vec3f{1,1,0});
    else if(h == 1)  return(Vec3f{-1,1,0});
    else if(h == 2)  return(Vec3f{1,-1,0});
    else if(h == 3)  return(Vec3f{-1,-1,0});
    else if(h == 4)  return(Vec3f{1,0,1});
    else if(h == 5)  return(Vec3f{-1,0,1});
    else if(h == 6)  return(Vec3f{1,0,-1});
    else if(h == 7)  return(Vec3f{-1,0,-1});
    else if(h == 8)  return(Vec3f{0,1,1});
    else if(h == 9)  return(Vec3f{0,-1,1});
    else if(h == 10) return(Vec3f{0,1,-1});
    else             return(Vec3f{0,-1,-1});
}

float perlinNoise1D(float p, uint32_t *ptable, uint32_t n){
    int x = (int)p;
    float xf = p - x;
    
    // values at the ends of the line
    float value_start = ptable[x%n];
    float value_end = ptable[(x+1)%n];

    // 1D vectors from start/end to given point
    float start = xf;
    float end = xf -1;

    // interpolating between the start and end values
    float value_point = lerp(start * getConstantVector1D(value_start), end *getConstantVector1D(value_end), fade(xf));
    return((value_point+1)*0.5);
}


float perlinNoise2D(Vec2f p, uint32_t *permutationTable, uint32_t n){
    // cell no (x,y)
    int x = (int)p.x;
    int y = (int)p.y;
    float xf = p.x - x;
    float yf = p.y - y;

    // values at 4 corners
    float value_lt = permutationTable[(permutationTable[x%n]+y+1)%n];
    float value_lb = permutationTable[(permutationTable[x%n]+y)%n];
    float value_rt = permutationTable[(permutationTable[(x+1)%n]+y+1)%n];
    float value_rb = permutationTable[(permutationTable[(x+1)%n]+y)%n];

    // vectors from corners to point
    Vec2f lt = {xf, yf-1};
    Vec2f lb = {xf, yf};
    Vec2f rt = {xf-1, yf-1};
    Vec2f rb = {xf-1, yf};

    // dot product of the const vectors and above vectors
    float dot_lt = dotProduct(lt, getConstantVector2D(value_lt));
    float dot_lb = dotProduct(lb, getConstantVector2D(value_lb));
    float dot_rt = dotProduct(rt, getConstantVector2D(value_rt));
    float dot_rb = dotProduct(rb, getConstantVector2D(value_rb));

    // interpolate between values
    float value_left  = lerp(dot_lb, dot_lt, fade(yf));
    float value_right = lerp(dot_rb, dot_rt, fade(yf));

    float value_point = lerp(value_left, value_right, fade(xf));
    return ((value_point+1)*0.5);
}

float perlinNoise3D(Vec3f p, uint32_t *permutationTable, uint32_t n){
    // cell no (x,y,z)
    int x = (int)p.x;
    int y = (int)p.y;
    int z = (int)p.z;
    float xf = p.x - x;
    float yf = p.y - y;
    float zf = p.z - z;

    // values at corners of cube
    float value_aaa = permutationTable[(permutationTable[(permutationTable[x%n]+y)%n]+z)%n];
    float value_aab = permutationTable[(permutationTable[(permutationTable[x%n]+y)%n]+z+1)%n];
    float value_aba = permutationTable[(permutationTable[(permutationTable[x%n]+y+1)%n]+z)%n];
    float value_abb = permutationTable[(permutationTable[(permutationTable[x%n]+y+1)%n]+z+1)%n];
    float value_baa = permutationTable[(permutationTable[(permutationTable[(x+1)%n]+y)%n]+z)%n];
    float value_bab = permutationTable[(permutationTable[(permutationTable[(x+1)%n]+y)%n]+z+1)%n];
    float value_bba = permutationTable[(permutationTable[(permutationTable[(x+1)%n]+y+1)%n]+z)%n];
    float value_bbb = permutationTable[(permutationTable[(permutationTable[(x+1)%n]+y+1)%n]+z+1)%n];

    // vectors from corners to point
    Vec3f aaa = {p.x, p.y, p.z};
    Vec3f aab = {p.x, p.y, p.z-1};
    Vec3f aba = {p.x, p.y-1, p.z};
    Vec3f abb = {p.x, p.y-1, p.z-1};
    Vec3f baa = {p.x-1, p.y, p.z};
    Vec3f bab = {p.x-1, p.y, p.z-1};
    Vec3f bba = {p.x-1, p.y-1, p.z};
    Vec3f bbb = {p.x-1, p.y-1, p.z-1};

    // dot product of the const vectors and above vectors
    float dot_aaa = dotProduct(aaa, getConstantVector3D(value_aaa));
    float dot_aab = dotProduct(aab, getConstantVector3D(value_aab));
    float dot_aba = dotProduct(aba, getConstantVector3D(value_aba));
    float dot_abb = dotProduct(abb, getConstantVector3D(value_abb));
    float dot_baa = dotProduct(baa, getConstantVector3D(value_baa));
    float dot_bab = dotProduct(bab, getConstantVector3D(value_bab));
    float dot_bba = dotProduct(bba, getConstantVector3D(value_bba));
    float dot_bbb = dotProduct(bbb, getConstantVector3D(value_bbb));


    // interpolate between values
    float u = fade(xf);
    float v = fade(yf);
    float w = fade(zf);

    float value_aaz = lerp(value_aaa, value_aab, w);
    float value_abz = lerp(value_aba, value_abb, w);
    float value_baz = lerp(value_baa, value_bab, w);
    float value_bbz = lerp(value_bba, value_bbb, w);

    float value_ayz = lerp(value_aaz, value_abz, v);
    float value_byz = lerp(value_baz, value_bbz, v);

    float value_xyz = lerp(value_ayz, value_byz, u);
    return ((value_xyz+1)*0.5);
}


float fractionalBrownianMotion2D(Vec2f p, uint32_t *permutationTable, uint32_t ptableSize, uint32_t layers){
    float value = 0;
    float freq = 1;
    for (int i=0; i<layers; i++){
        // double the frequency half the effect
        value += perlinNoise2D(Vec2f{p.x * freq, p.y * freq}, permutationTable, ptableSize)/freq;
        freq *= 2;
    }
    return(Clamp(0,value,1));
}
