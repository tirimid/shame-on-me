#version 430 core

in vec2 texcoord;

uniform sampler2D tex;

out vec4 col;

void
main()
{
	vec4 pix = texture(tex, texcoord);
	if (pix.a == 0.0)
	{
		discard;
	}
	col = pix;
}
