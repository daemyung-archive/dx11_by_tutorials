#include "stdafx.h"
#include "ModelListClass.h"
#include <ctime>


ModelListClass::ModelListClass()
{
}


ModelListClass::ModelListClass(const ModelListClass&)
{
}


ModelListClass::~ModelListClass()
{
}


bool ModelListClass::Initialize(int numModels)
{
	float red = 0.0f;
	float green = 0.0f;
	float blue = 0.0f;

	// 모델 수를 저장합니다
	m_modelCount = numModels;

	// 모델 정보의 리스트 배열을 만든다
	m_ModelInfoList = new ModelInfoType[m_modelCount];
	if (!m_ModelInfoList)
	{
		return false;
	}

	// 랜던 제네레이터에 현재 시간을 시드합니다
	srand((unsigned int)time(NULL));

	// 모든 모델을 거쳐 무작위로 모델 색상과 위치를 생성합니다
	for (int i = 0; i < m_modelCount; i++)
	{
		// 모델에 임의의 색상을 생성합니다
		red = (float)rand() / RAND_MAX;
		green = (float)rand() / RAND_MAX;
		blue = (float)rand() / RAND_MAX;

		m_ModelInfoList[i].color = XMFLOAT4(red, green, blue, 1.0f);

		// 모드에 대한 뷰어 앞에 임의의 위치를 생성합니다.
		m_ModelInfoList[i].positionX = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		m_ModelInfoList[i].positionY = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		m_ModelInfoList[i].positionZ = ((((float)rand() - (float)rand()) / RAND_MAX) * 10.0f) + 5.0f;
	}

	return true;
}


void ModelListClass::Shutdown()
{
	// 모델 정보 목록을 해제합니다
	if (m_ModelInfoList)
	{
		delete[] m_ModelInfoList;
		m_ModelInfoList = 0;
	}
}


int ModelListClass::GetModelCount()
{
	return m_modelCount;
}


void ModelListClass::GetData(int index, float& positionX, float& positionY, float& positionZ, XMFLOAT4& color)
{
	positionX = m_ModelInfoList[index].positionX;
	positionY = m_ModelInfoList[index].positionY;
	positionZ = m_ModelInfoList[index].positionZ;

	color = m_ModelInfoList[index].color;
}
