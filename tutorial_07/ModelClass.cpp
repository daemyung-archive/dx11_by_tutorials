#include "stdafx.h"
#include "ModelClass.h"
#include "TextureClass.h"
#include <fstream>

using namespace std;


ModelClass::ModelClass()
{
}


ModelClass::ModelClass(const ModelClass&)
{
}


ModelClass::~ModelClass()
{
}


bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* modelFilename, const wchar_t* textureFilename)
{
	// 모델 데이터를 로드합니다
	if (!LoadModel(modelFilename))
	{
		return false;
	}

	// 정점 및 인덱스 버퍼를 초기화합니다
	if (!InitializeBuffers(device))
	{
		return false;
	}

	// 이 모델의 텍스처를 로드합니다
	return LoadTexture(device, deviceContext, textureFilename);
}


void ModelClass::Shutdown()
{
	// 모델 텍스쳐를 반환합니다
	ReleaseTexture();

	// 버텍스 및 인덱스 버퍼를 종료합니다
	ShutdownBuffers();

	// 모델 데이터 반환
	ReleaseModel();
}


void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// 그리기를 준비하기 위해 그래픽 파이프 라인에 꼭지점과 인덱스 버퍼를 놓습니다
	RenderBuffers(deviceContext);
}


int ModelClass::GetIndexCount()
{
	return m_indexCount;
}


ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}


bool ModelClass::InitializeBuffers(ID3D11Device* device)
{

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

	// 정점 배열과 인덱스 배열을 데이터로 읽어옵니다
	for (int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		indices[i] = i;
	}

	// 정적 정점 버퍼의 구조체를 설정합니다
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
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

	// 정적 인덱스 버퍼의 구조체를 설정합니다
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 인덱스 데이터를 가리키는 보조 리소스 구조체를 작성합니다
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 생성합니다
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	// 생성되고 값이 할당된 정점 버퍼와 인덱스 버퍼를 해제합니다
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}


void ModelClass::ShutdownBuffers()
{
	// 인덱스 버퍼를 해제합니다
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// 정점 버퍼를 해제합니다
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}


void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// 정점 버퍼의 단위와 오프셋을 설정합니다
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 정점 버퍼로 그릴 기본형을 설정합니다 여기서는 삼각형으로 설정합니다
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


bool ModelClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* filename)
{
	// 텍스처 오브젝트를 생성한다
	m_Texture = new TextureClass();
	if (!m_Texture)
	{
		return false;
	}

	// 텍스처 오브젝트를 초기화한다
	return m_Texture->Initialize(device, filename);
}


void ModelClass::ReleaseTexture()
{
	// 텍스처 오브젝트를 릴리즈한다
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}
}


bool ModelClass::LoadModel(const char* filename)
{
	// 모델 파일을 엽니다
	ifstream fin;
	fin.open(filename);

	// 파일을 열 수 없으면 종료합니다
	if (fin.fail())
	{
		return false;
	}

	// 버텍스 카운트의 값까지 읽는다
	char input = 0;
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// 버텍스 카운트를 읽는다
	fin >> m_vertexCount;

	// 인덱스의 수를 정점 수와 같게 설정합니다
	m_indexCount = m_vertexCount;

	// 읽어 들인 정점 개수를 사용하여 모델을 만듭니다
	m_model = new ModelType[m_vertexCount];
	if (!m_model)
	{
		return false;
	}

	// 데이터의 시작 부분까지 읽는다
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// 버텍스 데이터를 읽습니다
	for (int i = 0; i < m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	// 모델 파일을 닫는다
	fin.close();

	return true;
}


void ModelClass::ReleaseModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}
}
