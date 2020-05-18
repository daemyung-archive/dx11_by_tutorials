#include "stdafx.h"
#include "TextureArrayClass.h"
#include <cstdio>


TextureArrayClass::TextureArrayClass()
{
}


TextureArrayClass::TextureArrayClass(const TextureArrayClass&)
{
}


TextureArrayClass::~TextureArrayClass()
{
}


bool TextureArrayClass::Initialize(ID3D11Device* device, const wchar_t* filename1, const wchar_t* filename2)
{
	// 첫번째 텍스처를 파일로부터 읽어온다
	if (FAILED(CreateDDSTextureFromFile(device, filename1, nullptr, &m_textures[0])))
	{
		return false;
	}

	// 두번째 텍스처를 파일로부터 읽어온다
	if (FAILED(CreateDDSTextureFromFile(device, filename2, nullptr, &m_textures[1])))
	{
		return false;
	}

	return true;
}


void TextureArrayClass::Shutdown()
{
	// 텍스처 뷰 리소스를 해제한다
	if (m_textures[0])
	{
		m_textures[0]->Release();
		m_textures[0] = 0;
	}

	if (m_textures[1])
	{
		m_textures[1]->Release();
		m_textures[1] = 0;
	}
}


ID3D11ShaderResourceView** TextureArrayClass::GetTextureArray()
{
	return m_textures;
}

