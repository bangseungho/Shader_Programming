#version 330

out vec4 FragColor;

in vec2 v_Texcoord;
uniform float u_Time;

void test()
{
	if(v_Texcoord.x >= 0.5)
	{
		FragColor = vec4(v_Texcoord, 0.0, 1.0);
	}
	else 
	{
		FragColor = vec4(0.0);
	}
}

void circle()
{
	vec2 temp = v_Texcoord - vec2(0.5, 0.5);
	float d = length(temp);
	float value = sin(d * 30);

	FragColor = vec4(value);

}

void main()
{
	circle();
}
