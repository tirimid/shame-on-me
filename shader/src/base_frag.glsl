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

float
ComputeLight(vec4 Light, samplerCube ShadowMap)
{
	vec3 LightToFrag = v_FragPos - Light.xyz;
	float Depth = CAM_CLIP_FAR * texture(ShadowMap, LightToFrag).r;

	if (length(LightToFrag) - SHADOW_BIAS < Depth)
	{
		vec3 Dir = normalize(Light.xyz - v_FragPos);
		float Diff = max(dot(Dir, v_Normal), 0.0);
		return sqrt(Light.w * Diff);
	}

	return 0.0;
}

void
main()
{
	vec4 Pix = texture(i_Tex, v_Texcoord);
	if (Pix.a == 0.0)
		discard;

	float Brightness = 0.0;

	// manually unrolled and refactored lighting computation loop.
	// platform GLSL compiler cannot be relied on.
	Brightness += ComputeLight(i_Lights[0], i_ShadowMaps[0]);
	Brightness += ComputeLight(i_Lights[1], i_ShadowMaps[1]);
	Brightness += ComputeLight(i_Lights[2], i_ShadowMaps[2]);
	Brightness += ComputeLight(i_Lights[3], i_ShadowMaps[3]);

	Brightness -= Brightness - LIGHT_STEP * floor(Brightness / LIGHT_STEP);
	Brightness += AMBIENT_LIGHT;

	f_Col = vec4(Brightness * Pix.rgb, 1.0);
}
