#version 330

in vec4 vposition;
//in vec4 vcolor;
in vec2 vtexcoord;

uniform mat4 projMatrix;
uniform mat4 cameraMatrix;
uniform sampler2D texUnit;
uniform float offset;
vec4 newposition;
vec2 newtexcoord;
out vec4 fcolor;


void main() {
	//access the texture at position for this vertex
	newposition=vposition;
	newtexcoord=vtexcoord;
	if(offset>=newtexcoord.y){
	newtexcoord.y=abs(newtexcoord.y-offset);
	}
	else{
	newtexcoord.y=1.0-abs(newtexcoord.y-offset);
	}
	newposition.y=texture(texUnit, newtexcoord).r;
  	fcolor = vec4(newposition.y,0,0,0);
	gl_Position = projMatrix*cameraMatrix*newposition;
}
