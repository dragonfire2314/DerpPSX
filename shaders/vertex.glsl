#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec2 texUV;
layout(location = 3) in int page;
layout(location = 4) in int palette;


out vec3 fragmentColor;
// Values that stay constant for the whole mesh.

void main(){

	gl_Position.xyz = vertexPosition_modelspace;
  	gl_Position.w = 1.0;

	fragmentColor = vertexColor;
}

