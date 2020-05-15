#include "stdafx.h"
#include "D3DClass.h"
#include "CameraClass.h"
#include "ModelClass.h"
#include "LightShaderClass.h"
#include "LightClass.h"
#include "RenderTextureClass.h"
#include "DebugWindowClass.h"
#include "TextureShaderClass.h"
#include "GraphicsClass.h"


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
	// Direct3D 객체 생성
	m_Direct3D = (D3DClass*)_aligned_malloc(sizeof(D3DClass), 16);
	if (!m_Direct3D)
	{
		return false;
	}

	// Direct3D 객체 초기화
	if (!m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// m_Camera 객체 생성
	m_Camera = new CameraClass();
	if (!m_Camera)
	{
		return false;
	}

	// 카메라 포지션 설정
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	// m_Model 객체 생성
	m_Model = new ModelClass();
	if (!m_Model)
	{
		return false;
	}

	// m_Model 객체 초기화
	if (!m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../tutorial_07/data/cube.txt", L"../tutorial_07/data/seafloor.dds"))
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// m_LightShader 객체 생성
	m_LightShader = new LightShaderClass();
	if (!m_LightShader)
	{
		return false;
	}

	// m_LightShader 객체 초기화
	if (!m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	// m_Light 객체 생성
	m_Light = new LightClass();
	if (!m_Light)
	{
		return false;
	}

	// m_Light 객체 초기화
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	// 렌더링 텍스쳐 객체를 생성한다
	m_RenderTexture = new RenderTextureClass();
	if (!m_RenderTexture)
	{
		return false;
	}

	// 렌더링 텍스처 객체를 초기화한다
	if (!m_RenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight))
	{
		return false;
	}

	// 디버그 창 객체를 만듭니다
	m_DebugWindow = new DebugWindowClass();
	if (!m_DebugWindow)
	{
		return false;
	}

	// 디버그 창 객체를 초기화 합니다
	if (!m_DebugWindow->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, 100, 100))
	{
		MessageBox(hwnd, L"Could not initialize the debug window object.", L"Error", MB_OK);
		return false;
	}

	// 텍스처 쉐이더 객체를 생성한다
	m_TextureShader = new TextureShaderClass();
	if (!m_TextureShader)
	{
		return false;
	}

	// 텍스처 쉐이더 객체를 초기화한다
	if (!m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	// 텍스처 쉐이더 객체를 해제한다
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// 디버그 창 객체를 해제한다
	if (m_DebugWindow)
	{
		m_DebugWindow->Shutdown();
		delete m_DebugWindow;
		m_DebugWindow = 0;
	}

	// 렌더 텍스쳐 객체를 해제한다
	if (m_RenderTexture)
	{
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	// m_Light 객체 반환
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// m_LightShader 객체 반환
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// m_Model 객체 반환
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// m_Camera 객체 반환
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Direct3D 객체 반환
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		_aligned_free(m_Direct3D);
		m_Direct3D = 0;
	}
}


bool GraphicsClass::Frame()
{
	// 카메라 위치 설정
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	return true;
}


bool GraphicsClass::Render()
{
	// 전체 장면을 먼제 텍스쳐로 렌더링 합니다
	if (!RenderToTexture())
	{
		return false;
	}

	// 씬을 그리기 위해 버퍼를 지웁니다
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 백 버퍼의 장면을 정상적으로 렌더링합니다
	if (!RenderScene())
	{
		return false;
	}

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다
	m_Direct3D->TurnZBufferOff();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;

	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// 디버그 윈도우 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 그리기 준비를 합니다
	if (!m_DebugWindow->Render(m_Direct3D->GetDeviceContext(), 50, 50))
	{
		return false;
	}

	// 텍스처 셰이더를 사용해 디버그 윈도우를 렌더링한다
	if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_DebugWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_RenderTexture->GetShaderResourceView()))
	{
		return false;
	}

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오
	m_Direct3D->TurnZBufferOn();

	// 버퍼의 내용을 화면에 출력합니다
	m_Direct3D->EndScene();

	return true;
}


bool GraphicsClass::RenderToTexture()
{
	// 렌더링 대상을 렌더링에 맞게 설정합니다
	m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView());

	// 렌더링을 텍스처에 지웁니다
	m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView(), 0.0f, 0.0f, 1.0f, 1.0f);

	// 이제 장면을 렌더링하면 백 버퍼 대신 텍스처로 렌더링됩니다
	if (!RenderScene())
	{
		return true;
	}

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다
	m_Direct3D->SetBackBufferRenderTarget();

	return true;
}


bool GraphicsClass::RenderScene()
{
	// 카메라의 위치에 따라 뷰 행렬을 생성합니다
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// 각 프레임의 rotation 변수를 업데이트합니다
	static float rotation = 0.0f;
	rotation += (float)XM_PI * 0.0025f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// 회전 값으로 월드 행렬을 회전합니다
	worldMatrix = XMMatrixRotationY(rotation);

	// 모델 버텍스와 인덱스 버퍼를 그래픽 파이프라인에 배치하여 렌더링 합니다
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// 라이트 쉐이더를 사용하여 모델을 렌더링합니다
	return m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor());
}
