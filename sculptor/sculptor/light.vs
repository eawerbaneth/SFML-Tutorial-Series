#version 150

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;

layout (location=0)in vec4 in_Position;
layout (location=1)in vec4 in_Color;

out vec4 pass_Color;

void main(void)
{
   vec4 w = vec4(in_Position[0], in_Position[1], in_Position[2], 1.0);
   gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * w;
   pass_Color = in_Color;
}