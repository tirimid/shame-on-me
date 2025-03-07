#version 330 core

layout (location = 0) in vec3 i_Pos;
layout (location = 1) in vec2 i_Texcoord;
layout (location = 2) in vec2 i_Normal;

uniform mat4 i_ModelMat;
uniform mat4 i_ViewMat;
uniform mat4 i_ProjMat;

out vec2 v_Texcoord;

void
main()
{
	gl_Position = i_ProjMat * i_ViewMat * i_ModelMat * vec4(i_Pos, 1.0);
	v_Texcoord = i_Texcoord;
}
