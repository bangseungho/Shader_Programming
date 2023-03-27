#version 330

in vec3 a_Position;
in vec3 a_Velocity;
in float a_EmitTime;
in float a_LifeTime;

uniform vec4 u_Trans;
uniform float u_Time;

const vec3 c_Gravity = vec3(0.0, -0.8, 0.0);

in vec3 a_Color;
out vec3 v_Color;
out float c_T;

void main()
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
					+ a_Velocity * newT
					+ c_Gravity * 0.5 * newT * newT;
		newPosition.w= 1;
	}

	v_Color = a_Color;
	gl_Position = newPosition;
}
