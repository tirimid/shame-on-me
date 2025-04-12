#version 430 core

#define MAXTILEBATCH $O_MAXTILEBATCH

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 itexcoord;

uniform mat4 modelmats[MAXTILEBATCH];
uniform mat4 projmat;

out vec2 texcoord;

void
main()
{
	gl_Position = projmat * modelmats[gl_InstanceID] * vec4(pos, 1.0);
	texcoord = itexcoord;
}
