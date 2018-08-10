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
   	fcolor = vec4(1,0,0,0);
	newposition=vposition;
	newtexcoord=vtexcoord;
	newtexcoord.y+=offset;
	if(newtexcoord.y>1.0){
	newtexcoord.y-=1.0;
	}
	newposition.y=texture(texUnit, newtexcoord).r*4.0;
  	gl_Position = projMatrix*cameraMatrix*newposition;
}
