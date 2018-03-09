#version 410
#define M_PI 3.1415926535897932384626433832795

uniform int width;
uniform float displacement;
uniform int useImage;

//uniform vec2 size;
uniform vec3 color1;
uniform vec3 color2;
uniform sampler2D inputTexture;
uniform sampler2D inputImage;

//ModulationInfo
uniform samplerBuffer modulationInfo;
uniform samplerBuffer displacementInfo;


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
    
    vec3 xDisplacementValue = texelFetch(displacementInfo, xVal).rgb;
    vec3 yDisplacementValue = texelFetch(displacementInfo, yVal + width).rgb;
    vec3 displacementValue = (xDisplacementValue + yDisplacementValue)/2;
    
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
    
    vec3 firstColor = vec3(0,0,0);
    if(useImage == 1){
        firstColor = texture(inputImage, vec2(xTex, yTex)).rgb;
    }
    else{
        firstColor = color1;
    }
    vec3 finalColor = (((color2 * modulationValue) + (firstColor * (1-modulationValue))) + displacementValue)  * r_info.r;
        
    out_color = vec4(finalColor, 1.0);
}
