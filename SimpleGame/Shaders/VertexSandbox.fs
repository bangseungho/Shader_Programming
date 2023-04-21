#version 330

layout(location = 0) out vec4 FragColor;
in float v_Alpha;

void main()
{
	float newLines = sin(2.0 * (1.0 - v_Alpha));

	FragColor = vec4(1, 1, 1, newLines);
}
