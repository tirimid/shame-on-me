#version 430 core

#define MAX_PLANE_BATCH $O_MAX_PLANE_BATCH

layout (location = 0) in vec3 i_Pos;

uniform mat4 i_ModelMats[MAX_PLANE_BATCH];

void
main()
{
	gl_Position = i_ModelMats[gl_InstanceID] * vec4(i_Pos, 1.0);
}
