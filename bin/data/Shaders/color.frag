#version 410
#define M_PI 3.1415926535897932384626433832795

uniform int width;

//uniform vec2 size;
uniform vec3 color1;
uniform vec3 color2;
uniform sampler2D inputTexture;

//ModulationInfo
uniform samplerBuffer modulationInfo;


out vec4 out_color;

void main(){
    
    //we grab the x and y and store them in an int
    int xVal = int(gl_FragCoord.x);
    int yVal = int(gl_FragCoord.y);

    float xTex = (float(xVal)+0.5) / float(textureSize(inputTexture, 0).x);
    float yTex = (float(yVal)+0.5) / float(textureSize(inputTexture, 0).y);
    
    vec4 r_info = texture(inputTexture, vec2(xTex, yTex));
    
    float xModulationValue = texelFetch(modulationInfo, xVal).r;
    float yModulationValue = texelFetch(modulationInfo, yVal + width).r;
    
    
    float modulationValue;
    
    if(xModulationValue == -1 && yModulationValue == -1){
        modulationValue = 0;
    }else if(xModulationValue == -1){
        modulationValue = yModulationValue;
    }else if(yModulationValue == -1){
        modulationValue = xModulationValue;
    }else{
        modulationValue = (xModulationValue + yModulationValue)/2;
    }
    
    vec3 finalColor = ((color2 * modulationValue) + (color1 * (1-modulationValue))) * r_info.r;
    
//    out_color = vec4(xModulationValue, modulationValue, modulationValue, 1.0);
    out_color = vec4(finalColor, 1.0);
}
