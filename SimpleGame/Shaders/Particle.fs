#version 330

layout(location=0) out vec4 FragColor;

uniform vec4 u_Color;

in vec4 v_Color;
in float c_T;

void main()
{
	// FragColor = vec4(0.73f, 0.16f, 0.73f, u_Color.a);
	// FragColor.xy += c_T;

	FragColor = vec4(v_Color.x, v_Color.y, v_Color.z, u_Color.a);
}
