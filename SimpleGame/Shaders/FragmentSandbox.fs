#version 330

out vec4 FragColor;

in vec2 v_Texcoord;

uniform vec2 u_Point;
uniform vec2 u_Points[3];
uniform float u_Time;
const float c_PI = 3.141592;

void test()
{
	float newValueX = v_Texcoord.x * 100.0 * c_PI;
	float outColorGreyVertical = sin(newValueX);

	float newValueY = v_Texcoord.y * 100.0 * c_PI;
	float outColorGreyHorizon = sin(newValueY);
	float max = max(outColorGreyVertical, outColorGreyHorizon);

	FragColor = vec4(max );
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
	float value = 0.2 * (pow(sin(d * 2 * c_PI - 5 * u_Time), 20) - 0.5);
	float temp2 = ceil(value);
	
	vec4 result = vec4(0);
	for(int i = 0; i<3; ++i){
		vec2 temp = v_Texcoord - u_Points[i];
		float d = length(temp);

		if(d < 0.03){
			result += 1.0 * temp2;
		}
	}
	FragColor = vec4(0, result.gba + 10 * value);
}

void Flag()
{
	float newColor2 = sin(v_Texcoord.x * c_PI * 2.0 - 10 * (u_Time + 0.2)) * v_Texcoord.x;
	float sinValue = sin(v_Texcoord.x * c_PI * 2.0 * 50);
	float width = 0.01;

	float finalColor = 0;
	for(int i = 0; i<6; ++i){
		float newTime = u_Time / 10 + i * 0.1;
		float newColor = sin(v_Texcoord.x * c_PI * 2.0 - 10 * newTime) * v_Texcoord.x;
		float width = 0.01 * v_Texcoord.x * 10;
		if( (2.0 * (v_Texcoord.y - 0.5)) > newColor && (2.0 * (v_Texcoord.y - 0.5)) < newColor + width)
		{
			finalColor += 1 * sinValue * (1 - v_Texcoord.x);
		}
	}
	FragColor = vec4(finalColor);
}

void Ttest()
{
	float finalColor = 0;
	for(int i=0; i<10; i++)
	{
		float newTime = u_Time * 0.1 + i*0.2;
		float newColor = v_Texcoord.x*0.5*
				sin(v_Texcoord.x*c_PI*2 - 10*newTime);
		float sinValue = sin(v_Texcoord.x*c_PI*2*10 - 500*newTime);
		float width = 0.01*v_Texcoord.x*5+0.001;
		if(2.0*(v_Texcoord.y-0.5) > newColor && 
		   2.0*(v_Texcoord.y-0.5) < newColor + width)
		{
			finalColor += 1*sinValue*(1.0-v_Texcoord.x);
		}
		else
		{
		}
	}
	FragColor = vec4(finalColor);
}

void main()
{
	Flag();
	Ttest();
}
