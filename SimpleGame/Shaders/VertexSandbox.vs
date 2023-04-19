#version 330

in vec3 a_Position;
uniform float u_Time;

const float PI = 3.141592;
void main()
{
	float value = PI * (a_Position.x + 1.0);
	float d = a_Position.x + 1.f;
	float newY = d * sin(value - u_Time);
	vec4 newPosition = vec4(a_Position, 1.0);
	newPosition.y = newY;
	gl_Position = newPosition;
}
