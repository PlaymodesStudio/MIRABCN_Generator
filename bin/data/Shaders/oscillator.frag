#version 410
#define M_PI 3.1415926535897932384626433832795

//uniform vec2 size;
uniform float phase;
uniform float time;
uniform int width;
uniform sampler2D randomInfo;

//Indexs
uniform samplerBuffer xIndexs;
uniform samplerBuffer yIndexs;

//Phase Offset
uniform samplerBuffer phaseOffset;
uniform samplerBuffer pulseWidth;
uniform samplerBuffer skew;
uniform samplerBuffer waveform;


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

float map(float value, float istart, float istop, float ostart, float ostop) {
    return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
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

void main(){
	//we grab the x and y and store them in an int
	int xVal = int(gl_FragCoord.x);
	int yVal = int(gl_FragCoord.y);
    
    
    
    //Compute parameters of current coord;
    float index = texelFetch(xIndexs, xVal).r + texelFetch(yIndexs, yVal).r;
    float phaseOffsetParam = texelFetch(phaseOffset, xVal).r + texelFetch(phaseOffset, yVal + width).r;
    float pulseWidthParam = texelFetch(pulseWidth, xVal).r * texelFetch(pulseWidth, yVal + width).r;
    float skewParam = texelFetch(skew, xVal).r + texelFetch(skew, yVal + width).r;
    
    //How to blend waveform Parameter???
    float waveformParam = max(texelFetch(waveform, xVal).r, texelFetch(waveform, yVal + width).r);
    
    
    //randon Info
    float xTex = (float(xVal)+0.5) / float(textureSize(randomInfo, 0).x);
    float yTex = (float(yVal)+0.5) / float(textureSize(randomInfo, 0).y);

    vec4 r_info = texture(randomInfo, vec2(xTex, yTex));
    float oldValue = r_info.r;
    float oldPhasor = r_info.g;
    float pastRandom = r_info.b;
    float newRandom = r_info.a;
    
    
    
    //get phasor to be w (radial freq)
    float w = (phase * 2 * M_PI);
    
    float k = (index + phaseOffsetParam) * 2 * M_PI;
    //
//    //    //invert it?
//    //    k *=  freq_Param * ((float)indexCount_Param/(float)indexQuant_Param); //Index Modifiers
//    
    w = w + k;
    w = mod(w, 2*M_PI);
//    
    w = map(w, (1-pulseWidthParam)*2*M_PI, 2*M_PI, 0, 2*M_PI);
    w = clamp(w, 0, 2*M_PI);

    float w_skewed = w;


    if(skewParam < 0){
        if(w < M_PI+((abs(skewParam))*M_PI)){
            w_skewed = map(w, 0, M_PI+((abs(skewParam))*M_PI), 0, M_PI);
        }
        else{
            w_skewed = map(w, M_PI+((abs(skewParam))*M_PI), 2*M_PI, M_PI, 2*M_PI);
        }
    }
    else if(skewParam > 0){
        if(w > ((1-abs(skewParam))*M_PI)){
            w_skewed = map(w, (1-abs(skewParam))*M_PI, 2*M_PI, M_PI, 2*M_PI);
        }
        else{
            w_skewed = map(w, 0, ((1-abs(skewParam))*M_PI), 0, M_PI);
        }
    }

    w = clamp(w_skewed, 0, 2*M_PI);    
    
    float linPhase =  w / (2*M_PI);
    float val1 = 0;
    float val2 = 0;
    if(waveformParam >= 0 && waveformParam < 1){ //SIN
        val1 = (sin(w) + 1 ) / 2;
    }
    if(waveformParam > 0 && waveformParam < 2){ //COS
        val2 = (cos(w) + 1 ) / 2;
    }
    if(waveformParam > 1 && waveformParam < 3){ //TRI
        val1 = 1-(abs((linPhase * (-2)) + 1));
    }
    if(waveformParam > 2 && waveformParam < 4){ //SQUARE
        val2 = (linPhase > 0) ? 1 : 0;
    }
    if(waveformParam > 3 && waveformParam < 5){ //SAW
        val1 = 1-linPhase;
    }
    if(waveformParam > 4 && waveformParam < 6){ //INVERTED SAW
        val2 = linPhase;
    }
    if(waveformParam > 5 && waveformParam < 7){ //Random
        if(linPhase < oldPhasor){
            val1 = rrrand(vec2((xVal) + time, (yVal) + time));
        }
        else{
            val1 = oldValue;
        }
    }
    if(waveformParam > 6 && waveformParam <= 7){
        if(linPhase < oldPhasor){
            pastRandom = newRandom;
            newRandom = snoise(vec2((xVal) + time, (yVal) + time));
            val2 = pastRandom;
        }
        else{
            val2 = pastRandom*(1-linPhase) + newRandom*linPhase;
        }
    }
    
    float val = 0;
    float waveInterp = waveformParam - int(waveformParam);
    if(int(waveformParam)%2 == 0){
        val = val1 * (1-waveInterp) + val2 * waveInterp;
    }
    else{
        val = val1 * waveInterp + val2 * (1-waveInterp);
    }
    
//    
//
//
        // oldPhasor[xVal][yVal] = linPhase;
//
    out_color = vec4(val, linPhase, pastRandom, newRandom);
}
