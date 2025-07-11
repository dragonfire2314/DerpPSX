#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 uv_in;

out vec2 UV;
// Values that stay constant for the whole mesh.

void main(){

	gl_Position.xyz = vertexPosition_modelspace;
  	gl_Position.w = 1.0;

	UV = uv_in;
}

