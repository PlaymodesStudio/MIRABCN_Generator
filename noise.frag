#version 120

uniform vec2 size;
uniform float phase;

void main(){
	//we grab the x and y and store them in an int
	float xVal = gl_FragCoord.x;
	float yVal = gl_FragCoord.y;
    
    
    float brightness = sin(2*3.14*phase);
    
    gl_FragColor = vec4(brightness,brightness,brightness, 1.0);
}
