#version 420

//smooth in vec4 theColor;
uniform sampler2D textureS2D;

in vec2 texCoord;
out vec4 outputColor;


void main()
{
   	outputColor = texture(textureS2D, texCoord);
}
