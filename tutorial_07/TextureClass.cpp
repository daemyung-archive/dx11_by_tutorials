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


bool TextureClass::Initialize(ID3D11Device* device, const wchar_t* filename)
{
	// DDS 이미지 데이터로부터 텍스쳐로 생성합니다
	if (FAILED(CreateDDSTextureFromFile(device, filename, nullptr, &m_texture)))
	{
		return false;
	}

	return true;
}


void TextureClass::Shutdown()
{
	// 텍스처 뷰 리소스를 해제한다
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}
}


ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_texture;
}
