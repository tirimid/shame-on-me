#version 430 core

#define CAMCLIPFAR $O_CAMCLIPFAR

in vec4 fragpos;

uniform vec3 lightpos;

void
main()
{
	gl_FragDepth = length(fragpos.xyz - lightpos) / CAMCLIPFAR;
}
