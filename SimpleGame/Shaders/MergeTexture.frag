#version 330

layout(location = 0) out vec4 FragColor;

in vec2 v_TexPos;

uniform sampler2D u_TexSamplerScene;
uniform sampler2D u_TexSamplerbloom;
uniform float u_Exposure = 2.2;

void main()
{
	const float gamma = 2.2;
	vec3 hdrColor = texture(u_TexSamplerScene, v_TexPos).rgb;
	vec3 bloomColor = texture(u_TexSamplerbloom, v_TexPos).rgb;
	hdrColor += bloomColor;

	vec3 result = vec3(1.0) - exp(-hdrColor * u_Exposure);

	result = pow(result, vec3(1.0 / gamma));
	FragColor = vec4(result, 1.0);
}
