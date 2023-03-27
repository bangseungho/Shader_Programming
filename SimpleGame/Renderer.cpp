#include "stdafx.h"
#include "Renderer.h"
#include <random>
#include <windows.h>

using namespace std;

default_random_engine dre;
uniform_real_distribution urdColor{0.f, 1.f};
uniform_real_distribution urdPos{-1.f, 1.f};
uniform_real_distribution urdVelX{-0.3f, 0.3f};
uniform_real_distribution urdVelY{0.9f, 1.6f};
uniform_real_distribution urdEmitTime{0.f, 10.f};
uniform_real_distribution urdLifeTime{0.f, 3.f};

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

	//Create VBOs
	CreateVertexBufferObjects();

	CreateParticle(2000);

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
	//���̴� ������Ʈ ����
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	//���̴� �ڵ带 ���̴� ������Ʈ�� �Ҵ�
	glShaderSource(ShaderObj, 1, p, Lengths);

	//�Ҵ�� ���̴� �ڵ带 ������
	glCompileShader(ShaderObj);

	GLint success;
	// ShaderObj �� ���������� ������ �Ǿ����� Ȯ��
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];

		//OpenGL �� shader log �����͸� ������
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		printf("%s \n", pShaderText);
	}

	// ShaderProgram �� attach!!
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
	GLuint ShaderProgram = glCreateProgram(); //�� ���̴� ���α׷� ����

	if (ShaderProgram == 0) { //���̴� ���α׷��� ����������� Ȯ��
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs;

	//shader.vs �� vs ������ �ε���
	if (!ReadFile(filenameVS, &vs)) {
		printf("Error compiling vertex shader\n");
		return -1;
	};

	//shader.fs �� fs ������ �ε���
	if (!ReadFile(filenameFS, &fs)) {
		printf("Error compiling fragment shader\n");
		return -1;
	};

	// ShaderProgram �� vs.c_str() ���ؽ� ���̴��� �������� ����� attach��
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	// ShaderProgram �� fs.c_str() �����׸�Ʈ ���̴��� �������� ����� attach��
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	//Attach �Ϸ�� shaderProgram �� ��ŷ��
	glLinkProgram(ShaderProgram);

	//��ũ�� �����ߴ��� Ȯ��
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		// shader program �α׸� �޾ƿ�
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

void Renderer::DrawParticleEffect()
{
	m_Timer->Update();
	//cout << m_Timer->GetDeltaTime() << endl;

	int shaderProgram = m_ParticleShader;
	glUseProgram(shaderProgram);

	int attribPosition = -1;
	attribPosition = glGetAttribLocation(shaderProgram, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticlePositionVBO);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	int attribColor = -1;
	attribColor = glGetAttribLocation(shaderProgram, "a_Color");
	glEnableVertexAttribArray(attribColor);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleColorVBO);
	glVertexAttribPointer(attribColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

	int attribVelocity = -1;
	attribVelocity = glGetAttribLocation(shaderProgram, "a_Velocity");
	glEnableVertexAttribArray(attribVelocity);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVelocityVBO);
	glVertexAttribPointer(attribVelocity, 3, GL_FLOAT, GL_FALSE, 0, 0);

	int attribEmitTime = -1;
	attribEmitTime = glGetAttribLocation(shaderProgram, "a_EmitTime");
	glEnableVertexAttribArray(attribEmitTime);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleEmitTimeVBO);
	glVertexAttribPointer(attribEmitTime, 1, GL_FLOAT, GL_FALSE, 0, 0);

	int attribLifeTime = -1;
	attribLifeTime = glGetAttribLocation(shaderProgram, "a_LifeTime");
	glEnableVertexAttribArray(attribLifeTime);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleLifeTimeVBO);
	glVertexAttribPointer(attribLifeTime, 1, GL_FLOAT, GL_FALSE, 0, 0);

	static float g_Time = 0.f;
	g_Time += m_Timer->GetDeltaTime();
	glUniform1f(glGetUniformLocation(m_ParticleShader, "u_Time"), g_Time);

	glDrawArrays(GL_TRIANGLES, 0, m_ParticleVerticesCount);
}

void Renderer::CreateParticle(int numParticle)
{
	m_ParticleSize = 0.005f;

	int particleCount = numParticle;
	m_ParticleVerticesCount = particleCount * 6;
	int floatCount = particleCount * 6 * 3;

	// Position
	std::vector<float> verticesPosition;

	for (int i = 0; i < particleCount; ++i) {
		float centerX = 0;
		float centerY = -0.8;

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
	std::vector<float> verticesColor;

	for (int i = 0; i < particleCount; ++i) {
		float colorR = urdColor(dre);
		float colorG = urdColor(dre);
		float colorB = urdColor(dre);

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
}

void Renderer::GetGLPosition(float x, float y, float *newX, float *newY)
{
	*newX = x * 2.f / m_WindowSizeX;
	*newY = y * 2.f / m_WindowSizeY;
}