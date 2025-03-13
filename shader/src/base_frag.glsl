#version 330 core

#define MAX_LIGHTS 4
#define AMBIENT_LIGHT 0.05
#define LIGHT_STEP 0.1
#define CAM_CLIP_FAR 500.0
#define SHADOW_BIAS 0.4

in vec2 v_Texcoord;
in vec3 v_Normal;
in vec3 v_FragPos;

uniform vec4 i_Lights[MAX_LIGHTS];
uniform samplerCube i_ShadowMaps[MAX_LIGHTS];
uniform sampler2D i_Tex;

out vec4 f_Col;

void
main()
{
	vec4 Pix = texture(i_Tex, v_Texcoord);
	if (Pix.a == 0.0)
		discard;

	float Brightness = 0.0;
	for (int i = 0; i < MAX_LIGHTS; ++i)
	{
		vec3 LightToFrag = v_FragPos - i_Lights[i].xyz;
		float Depth = CAM_CLIP_FAR * texture(i_ShadowMaps[i], LightToFrag).r;

		if (length(LightToFrag) - SHADOW_BIAS < Depth)
		{
			vec3 FragToLight = normalize(i_Lights[i].xyz - v_FragPos);
			float Diff = max(dot(FragToLight, v_Normal), 0.0);
			Brightness += sqrt(i_Lights[i].w * Diff);
		}
	}
	Brightness -= Brightness - LIGHT_STEP * floor(Brightness / LIGHT_STEP);
	Brightness += AMBIENT_LIGHT;

	f_Col = vec4(Brightness * Pix.rgb, 1.0);
}
