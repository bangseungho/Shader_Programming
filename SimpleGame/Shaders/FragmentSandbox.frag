#version 330

layout(location = 1) out vec4 FragColor;
layout(location = 2) out vec4 FragColor2;
layout(location = 3) out vec4 FragColor3;
layout(location = 4) out vec4 FragColor4;

in vec2 v_TexPos;

uniform vec2 u_Point;
uniform vec2 u_Points[3];
uniform float u_Time;
const float c_PI = 3.141592;
uniform sampler2D u_TexSampler;

void test()
{
	float newValueX = v_TexPos.x * 100.0 * c_PI;
	float outColorGreyVertical = sin(newValueX);
	
	float newValueY = v_TexPos.y * 100.0 * c_PI;
	float outColorGreyHorizon = sin(newValueY);
	float max = max(outColorGreyVertical, outColorGreyHorizon);
	
	FragColor = vec4(max );
}

void circle()
{
	vec2 temp = v_TexPos - u_Points[0];
	float d = length(temp);	
	vec2 temp1 = v_TexPos - u_Points[1];
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
	vec2 temp = v_TexPos - vec2(0.5, 0.5);
	float d = length(temp);
	float value = 0.2 * (pow(sin(d * 2 * c_PI - 5 * u_Time), 20) - 0.5);
	float temp2 = ceil(value);
	
	vec4 result = vec4(0);
	for(int i = 0; i<3; ++i){
		vec2 temp = v_TexPos - u_Points[i];
		float d = length(temp);

		if(d < 0.03){
			result += 1.0 * temp2;
		}
	}
	FragColor4 = vec4(0, result.g + 10 * value, 0, result.a + 10 * value);
}

void Flag()
{
	float newColor2 = sin(v_TexPos.x * c_PI * 2.0 - 10 * (u_Time + 0.2)) * v_TexPos.x;
	float sinValue = sin(v_TexPos.x * c_PI * 2.0 * 50);
	float width = 0.01;

	float finalColor = 0;
	for(int i = 0; i<6; ++i){
		float newTime = u_Time / 10 + i * 0.1;
		float newColor = sin(v_TexPos.x * c_PI * 2.0 - 10 * newTime) * v_TexPos.x;
		float width = 0.01 * v_TexPos.x * 10;
		if( (2.0 * (v_TexPos.y - 0.5)) > newColor && (2.0 * (v_TexPos.y - 0.5)) < newColor + width)
		{
			finalColor += 1 * sinValue * (1 - v_TexPos.x);
		}
	}
	FragColor3 = vec4(finalColor);
}

void Ttest()
{
	float finalColor = 0;
	for(int i=0; i<10; i++)
	{
		float newTime = u_Time * 0.1 + i*0.2;
		float newColor = v_TexPos.x*0.5*
				sin(v_TexPos.x*c_PI*2 - 10*newTime);
		float sinValue = sin(v_TexPos.x*c_PI*2*10 - 500*newTime);
		float width = 0.01*v_TexPos.x*5+0.001;
		if(2.0*(v_TexPos.y-0.5) > newColor && 
		   2.0*(v_TexPos.y-0.5) < newColor + width)
		{
			finalColor += 1*sinValue*(1.0-v_TexPos.x);
		}
		else
		{
		}
	}
	FragColor2 = vec4(finalColor);
}

void RealFlag()
{
	float period = (v_TexPos.x + 1.0) * 1.0;
	float x = v_TexPos.x * 2.0 * c_PI * period;
	float y = ((1 - v_TexPos.y) - 0.5) * 2;
	float sinValue = 0.25 * sin(x - u_Time * 10.0);

	float reverseX = 1 - v_TexPos.x;

	if(sinValue  * v_TexPos.x + 0.75 * (1 - v_TexPos.x * 0.75) > y && sinValue * v_TexPos.x - 0.75 * (1 - v_TexPos.x* 0.75) < y){
		float vX = v_TexPos.x;
		float yW = 1.5;
		float yD = y - (sinValue * v_TexPos.x - 0.75);
		float vY = 1 - yD / yW;

		FragColor = texture(u_TexSampler, vec2(vX, vY));
	}
	else {
		FragColor = vec4(0.f);
	}
}

void Draw()
{
	FragColor = vec4(v_TexPos, 0.f, 1.f);
}

void main()
{
	RealFlag();
	Ttest();
	Rader();
	Flag();
}
