#version 410
#define M_PI 3.1415926535897932384626433832795

//uniform vec2 size;
uniform float phase;
uniform float time;
uniform int width;

//Indexs
uniform samplerBuffer xIndexs;
uniform samplerBuffer yIndexs;

//Phase Offset
uniform samplerBuffer phaseOffset;
uniform samplerBuffer randomAddition;
uniform samplerBuffer scale;
uniform samplerBuffer offset;
uniform samplerBuffer pow_;
uniform samplerBuffer bipow;
uniform usamplerBuffer quantization;
uniform samplerBuffer pulseWidth;
uniform samplerBuffer skew;
uniform samplerBuffer fader;
uniform samplerBuffer invert_;
uniform samplerBuffer waveform;


out vec4 out_color;

//varying sampler2DRect oldPhasor;
//varying sampler2D oldValue;


float random (in vec2 _st) {
    return fract(sin(dot(_st.xy,
                         vec2(12.9898,78.233)))*
                 43758.5453123);
}

float map(float value, float istart, float istop, float ostart, float ostop) {
    return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
}

float computeMultiplyMod(float value, int xVal, int yVal){
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
        value = value + randomAdditionParam*random(vec2(xVal+time+phase, yVal+time/2 +  phase/2));
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
    
    return value;
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
//    if(wavformParam >= 6 && waveform < 7){
//        if(linPhase < oldPhasor[xVal][yVal])
//            val1 = random(vec2(xVal+time, yVal-time/2));
//        else
//            val1 = oldValue[xVal][yVal];
//    }
//        case rand2Osc:
//        {
//            if(linPhase < oldPhasor){
//                pastRandom = newRandom;
//                newRandom = ofRandom(1);
//                val = pastRandom;
//            }
//            else
//                val = pastRandom*(1-linPhase) + newRandom*linPhase;
//
//            break;
//        }
    
    float val = 0;
    float waveInterp = waveformParam - int(waveformParam);
    if(int(waveformParam)%2 == 0){
        val = val1 * (1-waveInterp) + val2 * waveInterp;
    }
    else{
        val = val1 * waveInterp + val2 * (1-waveInterp);
    }
    
//    
//        oldValue[xVal][yVal] = val;
//
    val = computeMultiplyMod(val, xVal, yVal);
//
        // oldPhasor[xVal][yVal] = linPhase;
//
    
    out_color = vec4(val,val,val, 1.0);
}
