#version 430 core

#define MAXLIGHTS $O_MAXLIGHTS
#define AMBIENTLIGHT $O_AMBIENTLIGHT
#define LIGHTSTEP $O_LIGHTSTEP
#define CAMCLIPFAR $O_CAMCLIPFAR
#define SHADOWBIAS $O_SHADOWBIAS

in vec2 texcoord;
in vec3 normal;
in vec3 fragpos;

uniform vec4 lights[MAXLIGHTS];
uniform samplerCube shadowmaps[MAXLIGHTS];
uniform sampler2D tex;
uniform float fadebrightness;

out vec4 col;

float
computelight(vec4 light, samplerCube shadowmap)
{
	vec3 ltof = fragpos - light.xyz;
	float depth = CAMCLIPFAR * texture(shadowmap, ltof).r;

	if (length(ltof) - SHADOWBIAS < depth)
	{
		vec3 dir = normalize(light.xyz - fragpos);
		float d = max(dot(dir, normal), 0.0);
		return sqrt(light.w * d);
	}

	return 0.0;
}

void
main()
{
	vec4 pix = texture(tex, texcoord);
	if (pix.a == 0.0)
	{
		discard;
	}

	float brightness = 0.0;

	// manually unrolled and refactored lighting computation loop.
	// platform GLSL compiler cannot be relied on.
	brightness += computelight(lights[0], shadowmaps[0]);
	brightness += computelight(lights[1], shadowmaps[1]);
	brightness += computelight(lights[2], shadowmaps[2]);
	brightness += computelight(lights[3], shadowmaps[3]);

	brightness -= brightness - LIGHTSTEP * floor(brightness / LIGHTSTEP);
	brightness += AMBIENTLIGHT;

	col = vec4(fadebrightness * brightness * pix.rgb, 1.0);
}
