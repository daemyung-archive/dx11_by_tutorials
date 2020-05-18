#include "stdafx.h"
#include "d3dclass.h"
#include "cameraclass.h"
#include "textclass.h"
#include "ModelClass.h"
#include "LightClass.h"
#include "LightShaderClass.h"
#include "ModelListClass.h"
#include "FrustumClass.h"
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	// Direct3D °´Ã¼ »ý¼º
	m_Direct3D = new D3DClass;
	if(!m_Direct3D)
	{
		return false;
	}

	// Direct3D °´Ã¼ ÃÊ±âÈ­
	if(!m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// m_Camera °´Ã¼ »ý¼º
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// Ä«¸Þ¶ó Æ÷Áö¼Ç ¼³Á¤
	XMMATRIX baseViewMatrix;
	m_Camera->SetPosition(0.0f, 0.0f, -1.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	// m_Text °´Ã¼ »ý¼º
	m_Text = new TextClass;
	if(!m_Text)
	{
		return false;
	}

	// m_Text °´Ã¼ ÃÊ±âÈ­
	if (!m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix))
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}

	// 모델 객체 생성
	m_Model = new ModelClass();
	if (!m_Model)
	{
		return false;
	}

	// 모델 객체 초기화
	if (!m_Model->Initialize(m_Direct3D->GetDevice(), L"../tutorial_16/data/seafloor.dds", "../tutorial_16/data/sphere.txt"))
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// 조명 쉐이더 객체 생성
	m_LightShader = new LightShaderClass();
	if (!m_LightShader)
	{
		return false;
	}

	// 조명 쉐이더 객체 초기화
	if (!m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// 조명 객체 생성
	m_Light = new LightClass();
	if (!m_Light)
	{
		return false;
	}

	// 조명 객체 초기화
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	// 모델 목록 객체 생성
	m_ModelList = new ModelListClass();
	if (!m_ModelList)
	{
		return false;
	}

	// 모델 목록 객체 초기화
	if (!m_ModelList->Initialize(25))
	{
		MessageBox(hwnd, L"Could not initialize the model list object.", L"Error", MB_OK);
		return false;
	}

	// 프로스텀 객체 생성
	m_Frustum = new FrustumClass();
	if (!m_Frustum)
	{
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	// 프로스텀 객체 반환
	if (m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = 0;
	}

	// 모델 목록 객체 반환
	if (m_ModelList)
	{
		m_ModelList->Shutdown();
		delete m_ModelList;
		m_ModelList = 0;
	}

	// 조면 객체 반환
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// 조명 쉐이더 객체 반환
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// 모델 객체 반환
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// m_Text °´Ã¼ ¹ÝÈ¯
	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	// m_Camera °´Ã¼ ¹ÝÈ¯
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Direct3D °´Ã¼ ¹ÝÈ¯
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
}


bool GraphicsClass::Frame(float rotationY)
{
	// 카메라 위치 설정
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	// 카메라의 회전을 설정합니다
	m_Camera->SetRotation(0.0f, rotationY, 0.0f);

	return true;
}


bool GraphicsClass::Render()
{
	float positionX = 0;
	float positionY = 0;
	float positionZ = 0;
	float radius = 1.0f; // 구의 반지름을 1.0f로 설정
	XMFLOAT4 color;

	// 씬을 그리기 위해 버퍼를 지웁니다
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다
	XMMATRIX worldMatrix, viewMatrix, proejctionMatrix, orthoMatrix;
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(proejctionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// 절두체를 만듭니다
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, proejctionMatrix, viewMatrix);

	// 렌더링 될 모델의 수를 얻는다
	int modelCount = m_ModelList->GetModelCount();

	// 렌더링 된 모델의 개수를 초기화 합니다
	int renderCount = 0;

	// 모든 모델을 살펴보고 카메라 뷰에서 볼 수 있는 경우만 렌더링 합니다
	for (int index = 0; index < modelCount; index++)
	{
		// 이 인덱스에서 구형 모델의 위치와 색상을 가져옵니다
		m_ModelList->GetData(index, positionX, positionY, positionZ, color);

		// 구형 모델이 뷰 frustum에 있는지 확인합니다
		if (m_Frustum->CheckSphere(positionX, positionY, positionZ, radius))
		{
			// 모델을 렌더링 할 위치로 이동합니다
			worldMatrix = XMMatrixTranslation(positionX, positionY, positionZ);

			// 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다
			m_Model->Render(m_Direct3D->GetDeviceContext());

			// 라이트 쉐이더를 사용하여 모델을 렌더링합니다
			m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, proejctionMatrix, m_Model->GetTexture(), m_Light->GetDirection(), color);

			// 원래의 월드 매트릭스로 리셋
			m_Direct3D->GetWorldMatrix(worldMatrix);

			// 이 모델이 렌더링 되었기 때문에 카운트 수를 늘립니다.
			renderCount++;
		}
	}

	// 이 프레임에서 실제로 렌더링 된 모델의 수를 설정합니다
	if (!m_Text->SetRenderCount(renderCount, m_Direct3D->GetDeviceContext()))
	{
		return false;
	}

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다
	m_Direct3D->TurnZBufferOff();

	// 텍스트를 렌더링하기 전에 알파 블렌딩을 켭니다
	m_Direct3D->TurnOnAlphaBlending();

	// 텍스트 문자열을 렌더링 합니다
	if (!m_Text->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix))
	{
		return false;
	}

	// 텍스트 렌더링 한 후 알파 블렌딩을 해제합니다
	m_Direct3D->TurnOffAlphaBlending();

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오
	m_Direct3D->TurnZBufferOn();

	// 버퍼의 내용을 화면에 출력합니다
	m_Direct3D->EndScene();

	return true;
}