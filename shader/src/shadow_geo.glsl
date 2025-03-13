#version 330 core

layout (triangles) in;

layout (triangle_strip, max_vertices = 18) out;

out vec4 g_FragPos;

uniform mat4 i_ShadowViewMats[6];
uniform mat4 i_ProjMat;

void
main()
{
	for (gl_Layer = 0; gl_Layer < 6; ++gl_Layer)
	{
		for (int i = 0; i < 3; ++i)
		{
			g_FragPos = gl_in[i].gl_Position;
			gl_Position = i_ProjMat * i_ShadowViewMats[gl_Layer] * g_FragPos;
			EmitVertex();
		}

		EndPrimitive();
	}
}
