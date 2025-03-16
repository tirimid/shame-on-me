#version 330 core

layout (location = 0) in vec3 i_Pos;
layout (location = 1) in vec2 i_Texcoord;
layout (location = 2) in vec3 i_Normal;

uniform mat4 i_ModelMat;
uniform mat4 i_ViewMat;
uniform mat4 i_ProjMat;

out vec2 v_Texcoord;
out vec3 v_Normal;
out vec3 v_FragPos;

void
main()
{
	gl_Position = i_ProjMat * i_ViewMat * i_ModelMat * vec4(i_Pos, 1.0);
	v_Texcoord = i_Texcoord;
	// TODO: precompute normal matrix.
	v_Normal = -normalize(mat3(transpose(inverse(i_ModelMat))) * i_Normal);
	v_FragPos = vec3(i_ModelMat * vec4(i_Pos, 1.0));
}
