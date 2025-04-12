#version 430 core

layout (triangles) in;

layout (triangle_strip, max_vertices = 18) out;

out vec4 fragpos;

uniform mat4 shadowviewmats[6];
uniform mat4 projmat;

void
main()
{
	for (gl_Layer = 0; gl_Layer < 6; ++gl_Layer)
	{
		for (int i = 0; i < 3; ++i)
		{
			fragpos = gl_in[i].gl_Position;
			gl_Position = projmat * shadowviewmats[gl_Layer] * fragpos;
			EmitVertex();
		}

		EndPrimitive();
	}
}
