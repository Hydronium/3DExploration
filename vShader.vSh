#version 420

uniform vec2 screenDim;

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texIN;

//smooth out vec4 theColor;
out vec2 texCoord;

void main()
{
	mat4 OrthoMat;

	float left = 0.0;
	float bottom = 0.0;
	float zNear = -1.0;
	float zFar = 1.0;

	OrthoMat = mat4(
					vec4(2.0/(screenDim.s - left), 	0.0, 	0.0, 	0.0),
					vec4(0.0, 	2.0/(screenDim.t - bottom), 	0.0, 	0.0),
					vec4(0.0,	0.0,	-1 * (2.0/(zFar - zNear)), 	0.0),
					vec4(-1.0 * (screenDim.s + left)/(screenDim.s - left), -1.0 * (screenDim.t + bottom)/(screenDim.t - bottom), -1.0 * (zFar + zNear)/(zFar - zNear), 1.0)
	);

	texCoord = texIN;
    gl_Position = OrthoMat * vec4(position, 0.0, 1.0);
    //gl_Position = vec4(position, 0.0, 1.0);
    //theColor = color;
}
