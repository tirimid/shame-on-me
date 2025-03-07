#version 330 core

in vec2 v_Texcoord;

uniform sampler2D i_Tex;

out vec4 f_Col;

void
main()
{
	f_Col = texture(i_Tex, v_Texcoord);
}
