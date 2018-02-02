#version 410
#define M_PI 3.1415926535897932384626433832795



//uniform vec2 size;
uniform float phase;
uniform float time;
uniform sampler2D randomInfo;


//Phase Offset
uniform samplerBuffer randomAddition;
uniform samplerBuffer scale;
uniform samplerBuffer offset;
uniform samplerBuffer pow_;
uniform samplerBuffer bipow;
uniform usamplerBuffer quantization;
uniform samplerBuffer fader;
uniform samplerBuffer invert_;

out vec4 out_color;

// ---------- A simplex noise helper method to please the eye:

// ---------
//
// Description : Array and textureless GLSL 2D simplex noise function.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : ijm
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise
//

vec3 mod289(vec3 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 x) {
    return mod289(((x*34.0)+1.0)*x);
}

float snoise(vec2 v)
{
    const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                        0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                        -0.577350269189626,  // -1.0 + 2.0 * C.x
                        0.024390243902439); // 1.0 / 41.0
    // First corner
    vec2 i  = floor(v + dot(v, C.yy) );
    vec2 x0 = v -   i + dot(i, C.xx);
    
    // Other corners
    vec2 i1;
    //i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
    //i1.y = 1.0 - i1.x;
    i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    // x0 = x0 - 0.0 + 0.0 * C.xx ;
    // x1 = x0 - i1 + 1.0 * C.xx ;
    // x2 = x0 - 1.0 + 2.0 * C.xx ;
    vec4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;
    
    // Permutations
    i = mod289(i); // Avoid truncation effects in permutation
    vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
                     + i.x + vec3(0.0, i1.x, 1.0 ));
    
    vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
    m = m*m ;
    m = m*m ;
    
    // Gradients: 41 points uniformly over a line, mapped onto a diamond.
    // The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)
    
    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;
    
    // Normalise gradients implicitly by scaling m
    // Approximation of: m *= inversesqrt( a0*a0 + h*h );
    m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
    
    // Compute final noise value at P
    vec3 g;
    g.x  = a0.x  * x0.x  + h.x  * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}

// --------- / end simplex noise

float random (in vec2 _st) {
    return fract(sin(dot(_st.xy,
                         vec2(12.9898,78.233)))*
                 43758.5453123);
}

highp float rrrand(vec2 co)
{
    highp float a = 12.9898;
    highp float b = 78.233;
    highp float c = 43758.5453;
    highp float dt= dot(co.xy ,vec2(a,b));
    highp float sn= mod(dt,3.14);
    return fract(sin(sn) * c);
}

float map(float value, float istart, float istop, float ostart, float ostop) {
    return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
}

void main(){
    
    //we grab the x and y and store them in an int
    int xVal = int(gl_FragCoord.x);
    int yVal = int(gl_FragCoord.y);
    int width = textureSize(randomInfo, 0).x;
    
    float xTex = (float(xVal)+0.5) / float(textureSize(randomInfo, 0).x);
    float yTex = (float(yVal)+0.5) / float(textureSize(randomInfo, 0).y);
    
    vec4 r_info = texture(randomInfo, vec2(xTex, yTex));
    float value = r_info.r;
    
    float randomAdditionParam = texelFetch(randomAddition, xVal).r + texelFetch(randomAddition, yVal + width).r;
    float scaleParam = texelFetch(scale, xVal).r * texelFetch(scale, yVal + width).r;
    float offsetParam = texelFetch(offset, xVal).r + texelFetch(offset, yVal + width).r;
    float powParam = texelFetch(pow_, xVal).r + texelFetch(pow_, yVal + width).r;
    float bipowParam = texelFetch(bipow, xVal).r + texelFetch(bipow, yVal + width).r;
    uint quantizationParam = min(texelFetch(quantization, xVal).r, texelFetch(quantization, yVal + width).r);
    float faderParam = texelFetch(fader, xVal).r * texelFetch(fader, yVal + width).r;
    float invertParam = max(texelFetch(invert_, xVal).r, texelFetch(invert_, yVal + width).r);
    
    //random Add
    if(randomAdditionParam != 0){
        value = value + randomAdditionParam*rrrand(vec2((xVal) + time, (yVal) + time));
    }
    
    value = clamp(value, 0.0, 1.0);
    
    //SCALE
    value *= scaleParam;

    //OFFSET
    value += offsetParam;
    
    value = clamp(value, 0.0, 1.0);

    //pow
    if(powParam != 0)
        value = (powParam < 0) ? pow(value, 1/float(-powParam+1)) : pow(value, powParam+1);
    
    //bipow
    value = map(value, 0, 1, -1, 1);
    if(bipowParam != 0){
        if(value < 0)
            value = -((bipowParam < 0) ? pow(abs(value), 1/float(-bipowParam+1)) : pow(abs(value), bipowParam+1));
        else
            value = (bipowParam < 0) ? pow(value, 1/float(-bipowParam+1)) : pow(value, bipowParam+1);
    }
    value = map(value, -1, 1, 0, 1);
    
    value = clamp(value, 0, 1);

    //Quantization
    if(quantizationParam < 255)
        value = (1/float(quantizationParam))*round(value*quantizationParam);
    
    value = clamp(value, 0, 1);

    value *= faderParam;
    
    float invertedValue = 1-value;
    float nonInvertedValue = value;
    value = (invertParam * invertedValue) + ((1-invertParam) * nonInvertedValue);
    
    out_color = vec4(value,value,value, 1.0);
}
