#version 330 core

#define CAM_CLIP_FAR $O_CAM_CLIP_FAR

in vec4 g_FragPos;

uniform vec3 i_LightPos;

void
main()
{
	gl_FragDepth = length(g_FragPos.xyz - i_LightPos) / CAM_CLIP_FAR;
}
