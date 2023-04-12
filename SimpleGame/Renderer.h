#pragma once

#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <memory>
#include "Timer.h"

#include "Dependencies\glew.h"

class Renderer
{
public:
	Renderer(int windowSizeX, int windowSizeY);
	~Renderer();

	bool IsInitialized();
	void DrawSolidRect(float x, float y, float z, float size, float r, float g, float b, float a);
	void DrawParticleEffect();
	void DrawFragmentSandbox();

private:
	void CreateParticle(int numParticle);
	

	GLuint m_ParticleShader = -1;
	int SetAttribVBO(int shaderProgram, const GLchar* attribName, GLuint VBO, GLint size);
	void Renderer::SetAttribute(int attribLoc, int size, int stride, int offset);

	GLuint m_ParticlePositionVBO = -1;
	GLuint m_ParticleColorVBO = -1;
	GLuint m_ParticleVelocityVBO = -1;
	GLuint m_ParticleEmitTimeVBO = -1;
	GLuint m_ParticleLifeTimeVBO = -1;	
	GLuint m_ParticlePeriodVBO = -1;
	GLuint m_ParticleAmpVBO = -1;
	GLuint m_ParticleValueVBO = -1;
	GLuint m_ParticlePosColorVelVBO = -1;

	GLuint m_FragmentSandboxShader = -1;
	GLuint m_FragmentSandboxVBO = -1;

private:
	float m_VerticesCount = 0;
	float m_ParticleSize = 0;
	int m_ParticleVerticesCount = 0;

public:
	Timer* m_Timer;

private:
	void Initialize(int windowSizeX, int windowSizeY);
	bool ReadFile(char* filename, std::string *target);
	void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
	GLuint CompileShaders(char* filenameVS, char* filenameFS);
	void CreateVertexBufferObjects();
	void GetGLPosition(float x, float y, float *newX, float *newY);

	bool m_Initialized = false;
	
	unsigned int m_WindowSizeX = 0;
	unsigned int m_WindowSizeY = 0;

	GLuint m_VBORect = 0;
	GLuint m_SolidRectShader = 0;
};

