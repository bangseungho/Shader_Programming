#version 330

out vec4 FragColor;

in vec4 v_Color;

void main()
{
	
	FragColor = vec4(v_Color.r, v_Color.g, v_Color.b, v_Color.a);
}
