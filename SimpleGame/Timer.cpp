#include "stdafx.h"
#include "Timer.h"

Timer::Timer()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&periodFrequency);
	// 1�� ���� CPU�� �������� ��ȯ(������)

	QueryPerformanceCounter((LARGE_INTEGER*)&lastTime);
	// �����ڴ� Update�Լ����� ������ ����Ǳ� ������ lastTime�� ����

	timeScale = 1.0 / static_cast<double>(periodFrequency);
	// �̸� (1 / �ʴ�������)�� ���·� ����� ����
}

void Timer::Update()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&curTime); 
	// ���� ������ ������ ����

	deltaTime = static_cast<double>(curTime - lastTime) * timeScale;
	// (���� ������ ������ - ���� ������ ������)

	lastTime = curTime;
	// ���� �������� ���� ���������� ����, ���� ��� �ݺ�
}
