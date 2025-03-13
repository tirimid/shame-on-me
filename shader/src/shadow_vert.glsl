#version 330 core

layout (location = 0) in vec3 i_Pos;

uniform mat4 i_ModelMat;

void
main()
{
	gl_Position = i_ModelMat * vec4(i_Pos, 1.0);
}
