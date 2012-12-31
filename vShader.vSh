#version 420

layout(location = 0) in vec3 position;
//layout(location = 1) in vec3 colour;

uniform mat4 perspectiveMatrix;
//flat out vec3 endcolour;

void main()
{
	//endcolour = colour;
    gl_Position = perspectiveMatrix * vec4(position, 1.0);
}
