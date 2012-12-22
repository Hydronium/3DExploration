#version 420

out vec4 outputColor;
flat in vec3 endcolour;

void main()
{
   	outputColor = vec4(endcolour, 1.0);
}
