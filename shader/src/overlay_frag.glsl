#version 330 core

#define MAX_LIGHTS 64

in vec2 v_Texcoord;

uniform vec4 i_Lights[MAX_LIGHTS];
uniform sampler2D i_Tex;

out vec4 f_Col;

void
main()
{
	vec4 Pix = texture(i_Tex, v_Texcoord);
	if (Pix.a == 0.0)
		discard;
	f_Col = Pix;
}
