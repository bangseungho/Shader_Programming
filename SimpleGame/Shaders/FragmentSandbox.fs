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
	/*vec4 result = vec4(0);
	 for(int i = 0; i<3; ++i){
		vec2 temp = v_Texcoord - u_Points[i];
		float d = length(temp);

		if(d < 0.1){
			result += 1.0;
		}
	} */
	vec2 temp = v_Texcoord - u_Points[0];
	float d = length(temp);	
	float value = 0.3 * pow(sin(c_PI * 2 * d - 5 * u_Time ), 20);
	float temp0 = ceil(value)

	if(d < 0.1){
		result += 1.0;
	}

	FragColor = vec4(result + 10 * value);
}

void main()
{
	//circle();
	Rader();
}
