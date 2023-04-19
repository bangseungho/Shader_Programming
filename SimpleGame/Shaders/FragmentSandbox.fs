#version 330

out vec4 FragColor;

in vec2 v_Texcoord;

uniform vec2 u_Point;
uniform vec2 u_Points[3];
uniform float u_Time;
const float c_PI = 3.141592;

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
	vec2 temp = v_Texcoord - u_Points[0];
	float d = length(temp);	
	vec2 temp1 = v_Texcoord - u_Points[1];
	float d1 = length(temp1);

	if(d < 0.1 || d1 < 0.1){
		FragColor = vec4(1);
	}
	else {
		FragColor = vec4(0.f);
	}
}

void Rader()
{
	vec2 temp = v_Texcoord - vec2(0.5, 0.5);
	float d = length(temp);	
	float value = 0.2 * (pow(sin(c_PI * 2 * d - u_Time * 4), 20) - 0.5);
	float temp1 = ceil(value);

	vec4 result = vec4(0);
	for(int i = 0; i<3; i++){
		vec2 temp = v_Texcoord - u_Point[i];
		float d = length(temp);

		if(d < 0.03){
			result += 1.0 * temp1;
		}
	}

	FragColor = vec4(result + 10 * value);
}

void main()
{
	//circle();
	Rader();
}
