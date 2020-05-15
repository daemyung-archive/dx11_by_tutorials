#include "stdafx.h"
#include "DebugWindowClass.h"


DebugWindowClass::DebugWindowClass()
{
}


DebugWindowClass::DebugWindowClass(const DebugWindowClass& other)
{
}


DebugWindowClass::~DebugWindowClass()
{
}


bool DebugWindowClass::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int bitmapWidth, int bitmapHeight)
{
	// 화면 크기를 저장하십시오
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// 이 비트맵을 렌더링 할 픽셀의 크기를 저장합니다
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	// 이전 렌더링 위치를 음수로 초기화 합니다
	m_previousPosX = -1;
	m_previousPosY = -1;

	// 정점 및 인덱스 버퍼를 초기화합니다
	return InitializeBuffers(device);
}


void DebugWindowClass::Shutdown()
{
	// 버택스 및 인덱스 버퍼를 종료합니다
	ShutdownBuffers();
}


bool DebugWindowClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	// 화면의 다른 위치로 렌더링하기 위해 동적 정점 버퍼를 다시 빌드합니다
	if (!UpdateBuffers(deviceContext, positionX, positionY))
	{
		return false;
	}

	// 그리기를 준비하기 위해 그래픽 파이프 라인에 꼭지점과 인덱스 버퍼를 놓습니다
	RenderBuffers(deviceContext);

	return true;
}


int DebugWindowClass::GetIndexCount()
{
	return m_indexCount;
}


bool DebugWindowClass::InitializeBuffers(ID3D11Device* device)
{
	// 정점 배열의 정점 수를 설정합니다
	m_vertexCount = 6;

	// 인덱스 배열의 인덱스 수를 설정합니다
	m_indexCount = m_vertexCount;

	// 정점 배열을 만듭니다
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// 인덱스 배열을 만듭니다
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// 처음에는 정점 배열을 0으로 초기화 합니다
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// 데이터로 인덱스 배열을 로드합니다
	for (int i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

	// 정적 정점 버퍼의 설명을 설정한다
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 이제 정점 버퍼를 만듭니다
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}

	// 정점 인덱스 버퍼의 설명을 설정합니다
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 하위 리소스 구조에 인덱스 데이터에 대한 포인터를 제공합니다
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 만듭니다
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	// 이제 버텍스와 인덱스 버퍼가 생성되고 로드 된 배열을 해제하십시오
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}


void DebugWindowClass::ShutdownBuffers()
{
	// 인덱스 버퍼를 해제합니다
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// 버텍스 버퍼를 해제한다
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}


bool DebugWindowClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	// 이 비트맵을 렌더링 할 위치가 변경되지 않은 경우 정점 버퍼를 업데이트하지 마십시오
	// 현재 올바른 매개 변수가 있습니다
	if ((positionX == m_previousPosX) && (positionY == m_previousPosY))
	{
		return true;
	}

	// 변경된 경우 렌더링되는 위치를 업데이트 합니다
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	// 비트 맵 왼쪽의 화면 좌표를 계산합니다
	float left = (float)((m_screenWidth / 2) * -1) + (float)positionX;

	// 비트 맵 오른쪽의 화면 좌표를 계산합니다
	float right = left + (float)m_bitmapWidth;

	// 비트 맵 상단의 화면 좌표를 계산합니다
	float top = (float)(m_screenHeight / 2) - (float)positionY;

	// 비태 맵 아래쪽의 화면 좌표를 계산합니다
	float bottom = top - (float)m_bitmapHeight;

	// 정점 배열을 만듭니다
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// 정점 배열에 데이터를 로드합니다
	// 첫번째 삼각형
	vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	// 두번째 삼각형
	vertices[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[4].position = XMFLOAT3(right, top, 0.0f);  // Top right.
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	// 버텍스 버퍼를 쓸 수 있도록 잠급니다
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// 정점 버퍼의 데이터를 가리키는 포인터를 얻는다
	VertexType* verticesPtr = (VertexType*)mappedResource.pData;

	// 데이터를 정점 버퍼에 복사합니다
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// 정점 버퍼의 잠금을 해제합니다
	deviceContext->Unmap(m_vertexBuffer, 0);

	// 더 이상 필요하지 않은 꼭지점 배열을 해제합니다
	delete[] vertices;
	vertices = 0;

	return true;
}


void DebugWindowClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// 정점 버퍼 보폭 및 오프셋을 설정합니다
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 이 꼭지점 버퍼에서 렌더링 되어야하는 프리미티브 유형을 설정합니다
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}