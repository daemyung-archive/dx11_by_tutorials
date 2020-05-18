#include "stdafx.h"
#include "TextureClass.h"


TextureClass::TextureClass()
{
}


TextureClass::TextureClass(const TextureClass& other)
{
}


TextureClass::~TextureClass()
{
}

bool TextureClass::Initialize(ID3D11Device* device, const WCHAR* filename)
{
	// �ؽ�ó�� ���Ϸκ��� �о�´�
	if(FAILED(CreateDDSTextureFromFile(device, filename, nullptr, &m_texture)))
	{
		return false;
	}

	return true;
}


void TextureClass::Shutdown()
{
	//�ؽ�ó �� ���ҽ��� �����Ѵ�.
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