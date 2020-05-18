#include "stdafx.h"
#include "TextureClass.h"
#include <cstdio>


TextureClass::TextureClass()
{
}


TextureClass::TextureClass(const TextureClass&)
{
}


TextureClass::~TextureClass()
{
}


bool TextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* filename)
{
	int width = 0;
	int height = 0;

	// targa 이미지 데이터를 메모리에 로드합니다
	if (!LoadTarga(filename, height, width))
	{
		return false;
	}

	// 텍스처의 구조체를 설정합니다
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// 빈 텍스처를 생성합니다
	HRESULT hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(hResult))
	{
		return false;
	}

	// targa 이미지 데이터의 너비 사이즈를 측정합니다
	UINT rowPitch = (width * 4) * sizeof(unsigned char);

	// targa 이미지 데이터를 텍스처에 복사합니다
	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

	// 셰이더 리소스 뷰 구조체를 설정합니다
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// 텍스처의 셰이더 리소스 뷰를 만듭니다
	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hResult))
	{
		return false;
	}

	// 이 텍스쳐에 대해 밉맵을 생성합니다
	deviceContext->GenerateMips(m_textureView);

	// 이미지 데이터가 텍스처에 로드 되었으므로 targa 이미지 데이터를 해제합니다
	delete[] m_targaData;
	m_targaData = 0;

	return true;
}


void TextureClass::Shutdown()
{
	// 텍스처 뷰 리소스를 해제한다
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = 0;
	}

	// 텍스쳐를 해제합니다
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	// targa 이미지 데이터를 해제합니다
	if (m_targaData)
	{
		delete[] m_targaData;
		m_targaData = 0;
	}
}


ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_textureView;
}


bool TextureClass::LoadTarga(const char* filename, int& height, int& width)
{
	// targa 파일을 바이너리 모드로 파일을 엽니다
	FILE* filePtr;
	if (fopen_s(&filePtr, filename, "rb") != 0)
	{
		return false;
	}

	// 파일 헤더를 읽어옵니다
	TargaHeader targaFileHeader;
	unsigned int count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// 파일헤더에서 중요 정보를 얻어옵니다
	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	int bpp = (int)targaFileHeader.bpp;

	// 파일이 32bit 인지 24bit인지 체크합니다
	if (bpp != 32)
	{
		return false;
	}

	// 32 비트 이미지 데이터의 크기를 계산합니다
	int imageSize = width * height * 4;

	// targa 이미지 데이터 용 메모리를 할당합니다
	unsigned char* targaImage = new unsigned char[imageSize];
	if (!targaImage)
	{
		return false;
	}

	// targa 이미지 데이터를 읽습니다
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// 파일을 닫습니다
	if (fclose(filePtr) != 0)
	{
		return false;
	}

	// targa 대상 데이터에 대한 메모리를 할당합니다
	m_targaData = new unsigned char[imageSize];
	if (!m_targaData)
	{
		return false;
	}

	// targa 대상 데이터 배열에 인덱스츷 로기화 합니다
	int index = 0;

	// targa 이미지 데이터에 인덱스를 초기화합니다
	int k = (width * height * 4) - (width * 4);

	// 이제 targa 형식이 거꾸로 저장되었으므로 올바른 순서로 targa 이미지 데이터를 targa 대상 배열에 복사합니다
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			m_targaData[index + 0] = targaImage[k + 2]; // 빨강
			m_targaData[index + 1] = targaImage[k + 1]; // 녹색
			m_targaData[index + 2] = targaImage[k + 0]; // 파랑
			m_targaData[index + 3] = targaImage[k + 3]; // 알파

			k += 4;                                     // 인덱스를 targa 데이터로 증가시킵니다
			index += 4;
		}

		// targa 이미지 데이터 인덱스를 연숙으로 읽은 후 열의 시작 부분에서 이전 행으로 다시 설정합니다
		k -= (width * 8);
	}

	// 대상 배열에 복사 된 targa 이미지 데이터를 해제합니다
	delete[] targaImage;
	targaImage = 0;

	return true;
}
