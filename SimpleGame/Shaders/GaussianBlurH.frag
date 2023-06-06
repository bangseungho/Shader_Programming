#version 330

layout(location = 0) out vec4 FragColor;

in vec2 v_TexPos;

uniform sampler2D u_TexSampler;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
	vec2 texOffset = 1.0 / textureSize(u_TexSampler, 0); // 해당 uv좌표의 거리
	vec3 result = texture(u_TexSampler, v_TexPos).rgb * weight[0]; // 텍스쳐 좌표에 자신의 가중치를 곱해준다.

	for(int i = 1; i<5; ++i){
		result += texture(u_TexSampler, v_TexPos + vec2(texOffset.x * i, 0.0)).rgb * weight[i];
		result += texture(u_TexSampler, v_TexPos - vec2(texOffset.x * i, 0.0)).rgb * weight[i];
	}
	// 양옆 픽셀을 자신의 가중치 만큼 곱해주고 자신의 결과에 더해준다.

	FragColor = vec4(result, 1.0);
}
