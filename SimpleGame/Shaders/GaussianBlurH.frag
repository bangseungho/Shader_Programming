#version 330

layout(location = 0) out vec4 FragColor;

in vec2 v_TexPos;

uniform sampler2D u_TexSampler;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
	vec2 texOffset = 1.0 / textureSize(u_TexSampler, 0); // �ش� uv��ǥ�� �Ÿ�
	vec3 result = texture(u_TexSampler, v_TexPos).rgb * weight[0]; // �ؽ��� ��ǥ�� �ڽ��� ����ġ�� �����ش�.

	for(int i = 1; i<5; ++i){
		result += texture(u_TexSampler, v_TexPos + vec2(texOffset.x * i, 0.0)).rgb * weight[i];
		result += texture(u_TexSampler, v_TexPos - vec2(texOffset.x * i, 0.0)).rgb * weight[i];
	}
	// �翷 �ȼ��� �ڽ��� ����ġ ��ŭ �����ְ� �ڽ��� ����� �����ش�.

	FragColor = vec4(result, 1.0);
}
