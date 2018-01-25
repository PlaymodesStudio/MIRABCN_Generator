#version 410
#define M_PI 3.1415926535897932384626433832795

//uniform vec2 size;
uniform float phase;
uniform float time;

//Indexs
uniform samplerBuffer xIndexs;
uniform samplerBuffer yIndexs;

//Phase Offset
uniform samplerBuffer xPhaseOffset;
uniform samplerBuffer yPhaseOffset;

out vec4 out_color;

//varying sampler2DRect oldPhasor;
//varying sampler2D oldValue;


//float computeMultiplyMod(float value){
//    
//    
//    //random Add
//    if(randomAdd_Param)
//        value += randomAdd_Param*ofRandom(1);
//    
//    value = ofClamp(value, 0, 1);
//    
//    //SCALE
//    value *= scale_Param;
//    
//    //OFFSET
//    value += offset_Param;
//    
//    value = ofClamp(value, 0, 1);
//    
//    //pow
//    if(pow_Param != 0)
//        value = (pow_Param < 0) ? pow(value, 1/(float)(-pow_Param+1)) : pow(value, pow_Param+1);
//    
//    //bipow
//    value = ofMap(value, 0, 1, -1, 1);
//    if(biPow_Param != 0){
//        if(value < 0)
//            value = -((biPow_Param < 0) ? pow(abs(value), 1/(float)(-biPow_Param+1)) : pow(abs(value), biPow_Param+1));
//        else
//            value = (biPow_Param < 0) ? pow(value, 1/(float)(-biPow_Param+1)) : pow(value, biPow_Param+1);
//    }
//    value = ofMap(value, -1, 1, 0, 1, true);
//    
//    
//    value = ofClamp(value, 0, 1);
//    
//    //Quantization
//    if(quant_Param < 255)
//        value = (1/(float)quant_Param)*round(value*quant_Param);
//    
//    value = ofClamp(value, 0, 1);
//    
//    value *= amplitude_Param;
//    
//    float invertedValue = 1-value;
//    float nonInvertedValue = value;
//    value = (invert_Param) * invertedValue + (1-invert_Param) * nonInvertedValue;
//    
//    return value;
//}

float random (in vec2 _st) {
    return fract(sin(dot(_st.xy,
                         vec2(12.9898,78.233)))*
                 43758.5453123);
}

void main(){
	//we grab the x and y and store them in an int
	int xVal = int(gl_FragCoord.x);
	int yVal = int(gl_FragCoord.y);
    
    //Compute parameters of current coord;
    float index = texelFetch(xIndexs, xVal).r + texelFetch(yIndexs, yVal).r;
    float phaseOffset = texelFetch(xPhaseOffset, xVal).r + texelFetch(yPhaseOffset, yVal).r;
    
    
    float val = 0;
    
    //sin
    //val = (sin(2 * 3.14 * phase) + 1 ) / 2;
    
    //cos
//    val = (cos(2 * M_PI * phase) + 1 ) / 2;
    
    //tri
//    val = 1-(abs((phase * (-2)) + 1));
    
    //Square
//    val = (phase > 0) ? 1 : 0;
    
    //Saw
    //val = 1-phase;
    
    //inverted saw
//    val = phase;

    //val = texelFetch(xIndexs, xVal).r;
    
    //val = random(vec2(xVal+time+phase, yVal+time/2 +  phase/2));
    
    
    
    //get phasor to be w (radial freq)
    float w = (phase * 2 * M_PI);
    
    float k = (index + phaseOffset) * 2 * M_PI;
    //
//    //    //invert it?
//    //    k *=  freq_Param * ((float)indexCount_Param/(float)indexQuant_Param); //Index Modifiers
//    
    w = w + k;
//    w = fmod(w, 2*PI);
//    
//    w = ofMap(w, (1-pulseWidth_Param)*2*PI, 2*PI, 0, 2*PI, true);
//    
//    float skewedW = w;
//    
//
//    if(skew_Param < 0){
//        if(w < PI+((abs(skew_Param))*PI))
//            skewedW = ofMap(w, 0, PI+((abs(skew_Param))*PI), 0, PI, true);
//        else
//            skewedW = ofMap(w, PI+((abs(skew_Param))*PI), 2*PI, PI, 2*PI, true);
//    }
//    else if(skew_Param > 0){
//        if(w > ((1-abs(skew_Param))*PI))
//            skewedW = ofMap(w, (1-abs(skew_Param))*PI, 2*PI, PI, 2*PI, true);
//        else
//            skewedW = ofMap(w, 0, ((1-abs(skew_Param))*PI), 0, PI, true);
//    }
//    
//    w = skewedW;
//    
//    
    float linPhase =  w / (2*M_PI);
//    float val = 0;
//    switch (static_cast<oscTypes>(waveSelect_Param.get()+1)){
//        case sinOsc:
//        {
              val = (sin(w) + 1 ) / 2;
//            break;
//
//        }
//        case cosOsc:
//        {
            //val = (cos(w) + 1 ) / 2;
//            break;
//        }
//        case triOsc:
//        {
//            val = 1-(abs((linPhase * (-2)) + 1));
//            break;
//        }
//        case squareOsc:
//        {
//            val = (linPhase > 0) ? 1 : 0;
//            break;
//        }
//        case sawOsc:
//        {
//            val = 1-linPhase;
//            break;
//        }
//        case sawInvOsc:
//        {
//            val = linPhase;
//            break;
//        }
//        case rand1Osc:
//        {
//            if(linPhase < oldPhasor[xVal][yVal])
//                val = random(vec2(xVal+time, yVal-time/2));
//            else
//                val = oldValue[xVal][yVal];
//            
//            break;
//        }
////        case rand2Osc:
////        {
////            if(linPhase < oldPhasor){
////                pastRandom = newRandom;
////                newRandom = ofRandom(1);
////                val = pastRandom;
////            }
////            else
////                val = pastRandom*(1-linPhase) + newRandom*linPhase;
////            
////            break;
////        }
    
    //// // possible rand3
//// //    float rand(vec2 co){
//// //    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);

////        default:
////            break;
//    }
//    
//        oldValue[xVal][yVal] = val;
//
//    //val = computeMultiplyMod(val);
//    
        // oldPhasor[xVal][yVal] = linPhase;
//
    
    out_color = vec4(val,val,val, 1.0);
}
