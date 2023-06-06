#version 330

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 FragColorHigh;

in vec2 v_TexPos;
varying vec4 v_Color;

uniform float u_Period;
uniform sampler2D u_TexSampler;

float brightStrength = 1.f;

void circle()
{
	vec2 temp = v_TexPos - vec2(0.5, 0.5);
	float d = length(temp);
	float value = sin(d * u_Period);
	if(d < 0.5)
	{
		FragColor = texture(u_TexSampler, v_TexPos);
	}
	else 
	{
		FragColor = vec4(0.0);
	}
}

void Textured()
{
	vec4 result = texture(u_TexSampler, v_TexPos) * v_Color;

	// RGB���� Grey scale�� �ٲ��ִ� ��
	// ������ RGB���� 1���� Ŭ ���� ��� ������ �κ��̴�.
	float brightness = dot(result.rgb, vec3(0.2126, 0.7152, 0.0722));
	FragColor = result;
	// ���� ���� ���ϰ� �ʹٸ� brightStrength�� ���� ����
	if(brightness > brightStrength)
		FragColorHigh = result;
	else
		FragColorHigh = vec4(0);
}

void main()
{
	Textured();
	//circle();
}
