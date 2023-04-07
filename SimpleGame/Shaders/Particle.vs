#version 330

in vec3 a_Position;
in vec3 a_Vel;
in float a_EmitTime;
in float a_LifeTime;
in float a_Period;
in float a_Amp;
in float a_Value;

uniform vec4 u_Trans;
uniform float u_Time;

const vec3 c_Gravity = vec3(0.0, -0.8, 0.0);

in vec3 a_Color;
out vec3 v_Color;
out float c_T;

const float c_PI = 3.141592;

vec4 RocketFire()
{
	float t = u_Time - a_EmitTime;
	vec4 newPosition = vec4(0, 0, 0, 1);

	if(t < 0.0)
	{
	
	}
	else
	{
		float newT = a_LifeTime * fract(t/a_LifeTime);

		float nX = sin(a_Value * 2.0 * c_PI);
		float nY = cos(a_Value * 2.0 * c_PI);

		newPosition.x = a_Position.x + nX + newT*a_Vel.x;
		newPosition.y = a_Position.y + nY + newT*a_Vel.y;
		
		vec2 newDir = vec2(-a_Vel.y, a_Vel.x);
		newDir = normalize(newDir);

		newPosition.xy += newT*a_Amp*sin(a_Period * newT * 2.0 * c_PI) * 
							newDir;
	
	}

	return newPosition;
}

vec4 FireWork()
{
	float t = u_Time - a_EmitTime;
	
	vec4 newPosition = vec4(0, 0, 0, 1);


	if(t < 0.0)
	{

	}
	else
	{
		float newT = a_LifeTime * fract(t / a_LifeTime);
		c_T = newT;

		newPosition.xyz = a_Position.xyz 
					+ a_Vel * newT
					+ c_Gravity * 0.5 * newT * newT;
		newPosition.w= 1;
	}

	v_Color = a_Color;

	return newPosition;
}

void main()
{
	// gl_Position = FireWork();
	gl_Position = RocketFire();
}
