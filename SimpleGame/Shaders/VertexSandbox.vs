#version 330

in vec3 a_Position;
uniform float u_Time;

const float PI = 3.141592;
out float v_Alpha;
void main()
{
	float value = 2 *PI * (a_Position.x + 1.0);
	float d = a_Position.x + 1.0;
	float newY = d * sin(value + u_Time);
	vec4 newPosition = vec4(a_Position, 1.0);
	newPosition.y = newY / 5.0;
	gl_Position = newPosition;

	v_Alpha = 1.0 - (a_Position.x + 1.0) / 2.0;
}
