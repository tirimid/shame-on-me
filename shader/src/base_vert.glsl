#version 330 core

#define MAXTILEBATCH $O_MAXTILEBATCH

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 itexcoord;
layout (location = 2) in vec3 inormal;

uniform mat4 modelmats[MAXTILEBATCH];
uniform mat3 normalmats[MAXTILEBATCH];
uniform mat4 viewmat;
uniform mat4 projmat;

out vec2 texcoord;
out vec3 normal;
out vec3 fragpos;

void
main()
{
	gl_Position = projmat * viewmat * modelmats[gl_InstanceID] * vec4(pos, 1.0);
	texcoord = itexcoord;
	normal = -normalize(normalmats[gl_InstanceID] * inormal);
	fragpos = vec3(modelmats[gl_InstanceID] * vec4(pos, 1.0));
}
