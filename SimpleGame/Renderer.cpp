#include "stdafx.h"
#include "Renderer.h"
#include <random>
#include <windows.h>

using namespace std;

default_random_engine dre;
uniform_real_distribution urdColor{0.f, 1.0f};
uniform_real_distribution urdPos{-1.f, 1.f};
uniform_real_distribution urdVelX{-1.f, 1.f};
uniform_real_distribution urdVelY{0.f, 2.f};
uniform_real_distribution urdEmitTime{0.f, 10.f};
uniform_real_distribution urdLifeTime{0.f, 3.f};
uniform_real_distribution urdPeriod{0.f, 1.f};
uniform_real_distribution urdAmp{-1.f, 1.f};
uniform_real_distribution urdValue{0.f, 1.f};

#pragma region DOIT

Renderer::Renderer(int windowSizeX, int windowSizeY)
{
	Initialize(windowSizeX, windowSizeY);
}

Renderer::~Renderer()
{
	delete m_Timer;
}

void Renderer::Initialize(int windowSizeX, int windowSizeY)
{
	//Set window size
	m_WindowSizeX = windowSizeX;
	m_WindowSizeY = windowSizeY;

	//Load shaders
	m_SolidRectShader = CompileShaders("./Shaders/SolidRect.vs", "./Shaders/SolidRect.fs");
	m_ParticleShader = CompileShaders("./Shaders/Particle.vs", "./Shaders/Particle.fs");
	m_FragmentSandboxShader = CompileShaders("./Shaders/FragmentSandbox.vs", "./Shaders/FragmentSandbox.fs");

	//Create VBOs
	CreateVertexBufferObjects();

	CreateParticle(20000);

	m_Timer = new Timer();

	if (m_SolidRectShader > 0 && m_VBORect > 0)
	{
		m_Initialized = true;
	}
}

bool Renderer::IsInitialized()
{
	return m_Initialized;
}

void Renderer::CreateVertexBufferObjects()
{
	float rect[]
		=
	{
		-1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f, -1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, 1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, //Triangle1
		-1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f,  1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, 1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f, //Triangle2
	};

	glGenBuffers(1, &m_VBORect);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);
}

void Renderer::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	//쉐이더 오브젝트 생성
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	//쉐이더 코드를 쉐이더 오브젝트에 할당
	glShaderSource(ShaderObj, 1, p, Lengths);

	//할당된 쉐이더 코드를 컴파일
	glCompileShader(ShaderObj);

	GLint success;
	// ShaderObj 가 성공적으로 컴파일 되었는지 확인
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];

		//OpenGL 의 shader log 데이터를 가져옴
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		printf("%s \n", pShaderText);
	}

	// ShaderProgram 에 attach!!
	glAttachShader(ShaderProgram, ShaderObj);
}

bool Renderer::ReadFile(char* filename, std::string *target)
{
	std::ifstream file(filename);
	if (file.fail())
	{
		std::cout << filename << " file loading failed.. \n";
		file.close();
		return false;
	}
	std::string line;
	while (getline(file, line)) {
		target->append(line.c_str());
		target->append("\n");
	}
	return true;
}

GLuint Renderer::CompileShaders(char* filenameVS, char* filenameFS)
{
	GLuint ShaderProgram = glCreateProgram(); //빈 쉐이더 프로그램 생성

	if (ShaderProgram == 0) { //쉐이더 프로그램이 만들어졌는지 확인
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs;

	//shader.vs 가 vs 안으로 로딩됨
	if (!ReadFile(filenameVS, &vs)) {
		printf("Error compiling vertex shader\n");
		return -1;
	};

	//shader.fs 가 fs 안으로 로딩됨
	if (!ReadFile(filenameFS, &fs)) {
		printf("Error compiling fragment shader\n");
		return -1;
	};

	// ShaderProgram 에 vs.c_str() 버텍스 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	// ShaderProgram 에 fs.c_str() 프레그먼트 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	//Attach 완료된 shaderProgram 을 링킹함
	glLinkProgram(ShaderProgram);

	//링크가 성공했는지 확인
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		// shader program 로그를 받아옴
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error linking shader program\n" << ErrorLog;
		return -1;
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error validating shader program\n" << ErrorLog;
		return -1;
	}

	glUseProgram(ShaderProgram);
	std::cout << filenameVS << ", " << filenameFS << " Shader compiling is done.";

	return ShaderProgram;
}

void Renderer::DrawSolidRect(float x, float y, float z, float size, float r, float g, float b, float a)
{
	float newX, newY;

	GetGLPosition(x, y, &newX, &newY);

	//Program select
	glUseProgram(m_SolidRectShader);

	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Trans"), newX, newY, 0, size);
	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Color"), r, g, b, a);

	int attribPosition = glGetAttribLocation(m_SolidRectShader, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(attribPosition);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int Renderer::SetAttribVBO(int shaderProgram, const GLchar* attribName, GLuint VBO, GLint size)
{
	int attribLoc = -1;
	attribLoc = glGetAttribLocation(shaderProgram, attribName);
	glEnableVertexAttribArray(attribLoc);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(attribLoc, size, GL_FLOAT, GL_FALSE, 0, 0);
	return attribLoc;
}

#pragma endregion

void Renderer::SetAttribute(int attribLoc, int size, int stride, int offset)
{
	glEnableVertexAttribArray(attribLoc);
	glVertexAttribPointer(attribLoc, size, GL_FLOAT, GL_FALSE, sizeof(float) * stride, (GLvoid*)(sizeof(float) * offset));
}

void Renderer::DrawParticleEffect()
{
	m_Timer->Update();

	int shaderProgram = m_ParticleShader;
	glUseProgram(shaderProgram);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//int posAttribLoc = SetAttribVBO(shaderProgram, "a_Position", m_ParticlePositionVBO, 3);
	//int colorAttribLoc = SetAttribVBO(shaderProgram, "a_Color", m_ParticleColorVBO, 3);
	//int velocityAttribLoc = SetAttribVBO(shaderProgram, "a_Vel", m_ParticleVelocityVBO, 3);
	int emitAttribLoc = SetAttribVBO(shaderProgram, "a_EmitTime", m_ParticleEmitTimeVBO, 1);
	int lifeAttribLoc = SetAttribVBO(shaderProgram, "a_LifeTime", m_ParticleLifeTimeVBO, 1);
	int	periodAttribLoc = SetAttribVBO(shaderProgram, "a_Period", m_ParticlePeriodVBO, 1);
	int ampAttribLoc = SetAttribVBO(shaderProgram, "a_Amp", m_ParticleAmpVBO, 1);
	int valueAttribLoc = SetAttribVBO(shaderProgram, "a_Value", m_ParticleValueVBO, 1);

	int posAttribLoc = -1;
	posAttribLoc = glGetAttribLocation(shaderProgram, "a_Position");
	int colorAttribLoc = -1;
	colorAttribLoc = glGetAttribLocation(shaderProgram, "a_Color");	
	int velocityAttribLoc = -1;
	velocityAttribLoc = glGetAttribLocation(shaderProgram, "a_Vel");

	glBindBuffer(GL_ARRAY_BUFFER, m_ParticlePosColorVelVBO);
	SetAttribute(posAttribLoc, 3, 10, 0);
	SetAttribute(colorAttribLoc, 4, 10, 3);
	SetAttribute(velocityAttribLoc, 3, 10, 7);

	static float g_Time = 0.f;
	g_Time += m_Timer->GetDeltaTime();
	glUniform1f(glGetUniformLocation(m_ParticleShader, "u_Time"), g_Time);

	glDrawArrays(GL_TRIANGLES, 0, m_ParticleVerticesCount);
	glDisable(GL_BLEND); // 다른 함수에서도 사용하기 때문에 꺼야함 나중에
}

void Renderer::DrawFragmentSandbox()
{
	GLuint shader = m_FragmentSandboxShader;
	glUseProgram(shader);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int posLoc = glGetAttribLocation(shader, "a_Position");
	int texLoc = glGetAttribLocation(shader, "a_Texcoord");
	glEnableVertexAttribArray(posLoc);
	glEnableVertexAttribArray(texLoc);

	glBindBuffer(GL_ARRAY_BUFFER, m_FragmentSandboxVBO);
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE,
		sizeof(float) * 5, 0);
	glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE,
		sizeof(float) * 5, (GLvoid*)(sizeof(float)*3));

	glDrawArrays(GL_TRIANGLES, 0, 6);
}


void Renderer::CreateParticle(int numParticle)
{
	m_ParticleSize = 0.004f;
	float centerX = 0.f;
	float centerY = 0.f;
	int particleCount = numParticle;
	m_ParticleVerticesCount = particleCount * 6;
	int floatCount = particleCount * 6 * 3;
	int floatCountPosColor = particleCount * 6 * (3 + 4 + 3);


	// Position
	// 총 파티클 버텍스의 x, y, z 값을 설정한다.
	std::vector<float> verticesPosition;

	for (int i = 0; i < particleCount; ++i) {
		centerX = 0.f;
		centerY = 0.f;

		// 1
		verticesPosition.push_back(centerX - m_ParticleSize);
		verticesPosition.push_back(centerY + m_ParticleSize);
		verticesPosition.push_back(0.f);
		
		// 2
		verticesPosition.push_back(centerX -m_ParticleSize);
		verticesPosition.push_back(centerY -m_ParticleSize);
		verticesPosition.push_back(0.f);

		// 3
		verticesPosition.push_back(centerX + m_ParticleSize);
		verticesPosition.push_back(centerY + m_ParticleSize);
		verticesPosition.push_back(0.f);

		// 4
		verticesPosition.push_back(centerX + m_ParticleSize);
		verticesPosition.push_back(centerY + m_ParticleSize);
		verticesPosition.push_back(0.f);

		// 5
		verticesPosition.push_back(centerX -m_ParticleSize);
		verticesPosition.push_back(centerY -m_ParticleSize);
		verticesPosition.push_back(0.f);

		// 6
		verticesPosition.push_back(centerX + m_ParticleSize);
		verticesPosition.push_back(centerY - m_ParticleSize);
		verticesPosition.push_back(0.f);
	}

	glGenBuffers(1, &m_ParticlePositionVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticlePositionVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * floatCount, &verticesPosition[0], GL_STATIC_DRAW);

	// Color
	// RGB 세 개의 값이 버텍스 하나에 들어가야 하기 때문에
	// attribPointer에서 size는 3임
	std::vector<float> verticesColor;

	for (int i = 0; i < particleCount; ++i) {
		float colorR = urdColor(dre);
		float colorG = urdColor(dre);
		float colorB = urdColor(dre);

		verticesColor.push_back(colorR);
		verticesColor.push_back(colorG);
		verticesColor.push_back(colorB);

		for (int i = 0; i < 6; ++i) {
			verticesColor.push_back(colorR);
			verticesColor.push_back(colorG);
			verticesColor.push_back(colorB);
		}
	}

	glGenBuffers(1, &m_ParticleColorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleColorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * floatCount, &verticesColor[0], GL_STATIC_DRAW);

	// Velocity
	std::vector<float> verticesVelocity;

	for (int i = 0; i < particleCount; ++i) {

		float velocityX = urdVelX(dre);
		float velocityY = urdVelY(dre);

		for (int i = 0; i < 6; ++i) {
			verticesVelocity.push_back(velocityX);
			verticesVelocity.push_back(velocityY);
			verticesVelocity.push_back(0.f);
		}
	}

	glGenBuffers(1, &m_ParticleVelocityVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVelocityVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * floatCount, &verticesVelocity[0], GL_STATIC_DRAW);

	// EmitTime
	std::vector<float> verticesEmitTime;
	int floatCountSingle = particleCount * 6;

	for (int i = 0; i < particleCount; ++i) {
		float emitTime = urdEmitTime(dre);

		verticesEmitTime.push_back(emitTime);
		verticesEmitTime.push_back(emitTime);
		verticesEmitTime.push_back(emitTime);

		verticesEmitTime.push_back(emitTime);
		verticesEmitTime.push_back(emitTime);
		verticesEmitTime.push_back(emitTime);
	}

	glGenBuffers(1, &m_ParticleEmitTimeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleEmitTimeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * floatCountSingle, &verticesEmitTime[0], GL_STATIC_DRAW);

	// LifeTime
	std::vector<float> verticesLifeTime;

	for (int i = 0; i < particleCount; ++i) {
		float lifeTime = urdLifeTime(dre);

		verticesLifeTime.push_back(lifeTime);
		verticesLifeTime.push_back(lifeTime);
		verticesLifeTime.push_back(lifeTime);
		
		verticesLifeTime.push_back(lifeTime);
		verticesLifeTime.push_back(lifeTime);
		verticesLifeTime.push_back(lifeTime);
	}

	glGenBuffers(1, &m_ParticleLifeTimeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleLifeTimeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * floatCountSingle, &verticesLifeTime[0], GL_STATIC_DRAW);

	// Period
	std::vector<float> verticesPeriod;

	for (int i = 0; i < particleCount; ++i) {
		float randPeriod = urdPeriod(dre);

		verticesPeriod.push_back(randPeriod);
		verticesPeriod.push_back(randPeriod);
		verticesPeriod.push_back(randPeriod);

		verticesPeriod.push_back(randPeriod);
		verticesPeriod.push_back(randPeriod);
		verticesPeriod.push_back(randPeriod);
	}

	glGenBuffers(1, &m_ParticlePeriodVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticlePeriodVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * floatCountSingle, &verticesPeriod[0], GL_STATIC_DRAW);

	// Amp
	std::vector<float> verticesAmp;

	for (int i = 0; i < particleCount; ++i) {
		float randAmp = urdAmp(dre);

		verticesLifeTime.push_back(randAmp);
		verticesLifeTime.push_back(randAmp);
		verticesLifeTime.push_back(randAmp);

		verticesLifeTime.push_back(randAmp);
		verticesLifeTime.push_back(randAmp);
		verticesLifeTime.push_back(randAmp);
	}

	glGenBuffers(1, &m_ParticleAmpVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleAmpVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * floatCountSingle, &verticesLifeTime[0], GL_STATIC_DRAW);

	// Value
	std::vector<float> verticesValue;

	for (int i = 0; i < particleCount; ++i) {
		float randValue = urdAmp(dre);

		verticesValue.push_back(randValue);
		verticesValue.push_back(randValue);
		verticesValue.push_back(randValue);

		verticesValue.push_back(randValue);
		verticesValue.push_back(randValue);
		verticesValue.push_back(randValue);
	}

	glGenBuffers(1, &m_ParticleValueVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleValueVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * floatCountSingle, &verticesValue[0], GL_STATIC_DRAW);

	// PosColor
	std::vector<float> verticesPosColorVel;

	for (int i = 0; i < particleCount; ++i) {
		centerX = 0.f;
		centerY = 0.f;
		float colorR = urdColor(dre);
		float colorG = urdColor(dre);
		float colorB = urdColor(dre);
		float colorA = urdColor(dre);
		float velocityX = urdVelX(dre);
		float velocityY = urdVelY(dre);

		// 1
		verticesPosColorVel.push_back(centerX - m_ParticleSize);
		verticesPosColorVel.push_back(centerY + m_ParticleSize);
		verticesPosColorVel.push_back(0.f);
		verticesPosColorVel.push_back(colorR);
		verticesPosColorVel.push_back(colorG);
		verticesPosColorVel.push_back(colorB);
		verticesPosColorVel.push_back(colorA);
		verticesPosColorVel.push_back(velocityX);
		verticesPosColorVel.push_back(velocityY);
		verticesPosColorVel.push_back(0.f);
		// 2
		verticesPosColorVel.push_back(centerX - m_ParticleSize);
		verticesPosColorVel.push_back(centerY - m_ParticleSize);
		verticesPosColorVel.push_back(0.f);
		verticesPosColorVel.push_back(colorR);
		verticesPosColorVel.push_back(colorG);
		verticesPosColorVel.push_back(colorB);
		verticesPosColorVel.push_back(colorA);
		verticesPosColorVel.push_back(velocityX);
		verticesPosColorVel.push_back(velocityY);
		verticesPosColorVel.push_back(0.f);
		// 3
		verticesPosColorVel.push_back(centerX + m_ParticleSize);
		verticesPosColorVel.push_back(centerY + m_ParticleSize);
		verticesPosColorVel.push_back(0.f);
		verticesPosColorVel.push_back(colorR);
		verticesPosColorVel.push_back(colorG);
		verticesPosColorVel.push_back(colorB);
		verticesPosColorVel.push_back(colorA);
		verticesPosColorVel.push_back(velocityX);
		verticesPosColorVel.push_back(velocityY);
		verticesPosColorVel.push_back(0.f);
		// 4
		verticesPosColorVel.push_back(centerX + m_ParticleSize);
		verticesPosColorVel.push_back(centerY + m_ParticleSize);
		verticesPosColorVel.push_back(0.f);
		verticesPosColorVel.push_back(colorR);
		verticesPosColorVel.push_back(colorG);
		verticesPosColorVel.push_back(colorB);
		verticesPosColorVel.push_back(colorA);
		verticesPosColorVel.push_back(velocityX);
		verticesPosColorVel.push_back(velocityY);
		verticesPosColorVel.push_back(0.f);
		// 5
		verticesPosColorVel.push_back(centerX - m_ParticleSize);
		verticesPosColorVel.push_back(centerY - m_ParticleSize);
		verticesPosColorVel.push_back(0.f);
		verticesPosColorVel.push_back(colorR);
		verticesPosColorVel.push_back(colorG);
		verticesPosColorVel.push_back(colorB);
		verticesPosColorVel.push_back(colorA);
		verticesPosColorVel.push_back(velocityX);
		verticesPosColorVel.push_back(velocityY);
		verticesPosColorVel.push_back(0.f);
		// 6
		verticesPosColorVel.push_back(centerX + m_ParticleSize);
		verticesPosColorVel.push_back(centerY - m_ParticleSize);
		verticesPosColorVel.push_back(0.f);
		verticesPosColorVel.push_back(colorR);
		verticesPosColorVel.push_back(colorG);
		verticesPosColorVel.push_back(colorB);
		verticesPosColorVel.push_back(colorA);
		verticesPosColorVel.push_back(velocityX);
		verticesPosColorVel.push_back(velocityY);
		verticesPosColorVel.push_back(0.f);
	}

	glGenBuffers(1, &m_ParticlePosColorVelVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticlePosColorVelVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * floatCountPosColor, &verticesPosColorVel[0], GL_STATIC_DRAW);

	float rect1[] =
	{
		-1.f, -1.f, 0.f,	0.f, 1.f,
		-1.f, 1.f, 0.f,		0.f, 0.f,
		1.f, 1.f, 0.f,		1.f, 0.f,
		-1.f, -1.f, 0.f,	0.f, 1.f,
		1.f, 1.f, 0.f,		1.f, 0.f,
		1.f, -1.f, 0.f,		1.f, 1.f
	};

	glGenBuffers(1, &m_FragmentSandboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_FragmentSandboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect1), rect1, GL_STATIC_DRAW);
}

void Renderer::GetGLPosition(float x, float y, float *newX, float *newY)
{
	*newX = x * 2.f / m_WindowSizeX;
	*newY = y * 2.f / m_WindowSizeY;
}