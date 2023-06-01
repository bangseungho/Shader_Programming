#version 330

out vec4 FragColor;
varying vec4 v_Color;
in vec2 v_Texcoord;
uniform float u_Period;
uniform sampler2D u_TexSampler;

void circle()
{
	vec2 temp = v_Texcoord - vec2(0.5, 0.5);
	float d = length(temp);
	float value = sin(d * u_Period);
	if(d < 0.5)
	{
		FragColor = texture(u_TexSampler, v_Texcoord);
	}
	else 
	{
		FragColor = vec4(0.0);
	}
}

void main()
{
	circle();
}
