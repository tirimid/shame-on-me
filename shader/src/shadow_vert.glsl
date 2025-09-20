#version 330 core

#define MAXTILEBATCH $O_MAXTILEBATCH

layout (location = 0) in vec3 pos;

uniform mat4 modelmats[MAXTILEBATCH];

void
main()
{
	gl_Position = modelmats[gl_InstanceID] * vec4(pos, 1.0);
}
