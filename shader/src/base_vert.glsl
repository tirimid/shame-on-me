#version 430 core

#define MAX_PLANE_BATCH $O_MAX_PLANE_BATCH

layout (location = 0) in vec3 i_Pos;
layout (location = 1) in vec2 i_Texcoord;
layout (location = 2) in vec3 i_Normal;

uniform mat4 i_ModelMats[MAX_PLANE_BATCH];
uniform mat3 i_NormalMats[MAX_PLANE_BATCH];
uniform mat4 i_ViewMat;
uniform mat4 i_ProjMat;

out vec2 v_Texcoord;
out vec3 v_Normal;
out vec3 v_FragPos;

void
main()
{
	gl_Position = i_ProjMat * i_ViewMat * i_ModelMats[gl_InstanceID] * vec4(i_Pos, 1.0);
	v_Texcoord = i_Texcoord;
	v_Normal = -normalize(i_NormalMats[gl_InstanceID] * i_Normal);
	v_FragPos = vec3(i_ModelMats[gl_InstanceID] * vec4(i_Pos, 1.0));
}
