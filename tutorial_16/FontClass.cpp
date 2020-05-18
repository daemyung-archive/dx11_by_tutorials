#include "stdafx.h"
#include "TextureClass.h"
#include "FontClass.h"

#include <fstream>
using namespace std;


FontClass::FontClass()
{
}

FontClass::FontClass(const FontClass& other)
{
}

FontClass::~FontClass()
{
}

bool FontClass::Initialize(ID3D11Device* device, const char* fontFilename, const WCHAR* textureFilename)
{
	// ��Ʈ �����͸� ������ �ؽ�Ʈ ���Ͽ��ε��Ѵ�.
	if (!LoadFontData(fontFilename))
	{
		return false;
	}

	// ��Ʈ ���ڰ� �ִ� �ؽ�ó�� �ε��մϴ�.
	return LoadTexture(device, textureFilename);
}


void FontClass::Shutdown()
{
	// ��Ʈ �ؽ�ó�� �����մϴ�.
	ReleaseTexture();

	// ��Ʈ �����͸� �����մϴ�.
	ReleaseFontData();
}


bool FontClass::LoadFontData(const char* filename)
{
	ifstream fin;
	int i;
	char temp;


	// �۲� ���� ���۸� ����ϴ�.
	m_Font = new FontType[95];
	if (!m_Font)
	{
		return false;
	}

	// ���� ������ �۲� ũ��� ������ �н��ϴ�.
	fin.open(filename);
	if (fin.fail())
	{
		return false;
	}

	// �ؽ�Ʈ�� ��� �� ASCII ���� 95 ���� �н��ϴ�.
	for (i = 0; i<95; i++)
	{
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}

		fin >> m_Font[i].left;
		fin >> m_Font[i].right;
		fin >> m_Font[i].size;
	}

	// ������ �ݽ��ϴ�.
	fin.close();

	return true;
}


void FontClass::ReleaseFontData()
{
	// ��Ʈ ������ �迭�� �����Ѵ�.
	if (m_Font)
	{
		delete[] m_Font;
		m_Font = 0;
	}
}


bool FontClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
	// �ؽ�ó ������Ʈ�� �����Ѵ�.
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	// �ؽ�ó ������Ʈ�� �ʱ�ȭ�Ѵ�.
	return m_Texture->Initialize(device, filename);
}


void FontClass::ReleaseTexture()
{
	// �ؽ�ó ������Ʈ�� �������Ѵ�.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}
}


ID3D11ShaderResourceView* FontClass::GetTexture()
{
	return m_Texture->GetTexture();
}


void FontClass::BuildVertexArray(void* vertices, const char* sentence, float drawX, float drawY)
{
	// �Է� ������ VertexType ������ ���� ��ȯ�մϴ�.
	VertexType* vertexPtr = (VertexType*)vertices;

	// ������ ���� ���� ���Ͻʽÿ�.
	int numLetters = (int)strlen(sentence);

	// ���� �迭�� ���� �ε����� �ʱ�ȭ�մϴ�.
	int index = 0;
	int letter = 0;

	// �� ���ڸ� ���� ���� �׸��ϴ�.
	for (int i = 0; i<numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32;

		// ���ڰ� �����̸� 3 �ȼ� ���� �̵��մϴ�.
		if (letter == 0)
		{
			drawX = drawX + 3.0f;
		}
		else
		{
			// ������ ù ��° �ﰢ��.
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // ���� ��
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);  // ������ �Ʒ�
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX, (drawY - 16), 0.0f);  // ���� �Ʒ�
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 1.0f);
			index++;

			// ������ �� ��° �ﰢ��.
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // ���� ��
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX + m_Font[letter].size, drawY, 0.0f);  // ������ ��
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);  // ������ �Ʒ�
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
			index++;

			// ������ ũ��� �� �ȼ� �� �׸��⸦ ���� x ��ġ�� ������Ʈ�մϴ�.
			drawX = drawX + m_Font[letter].size + 1.0f;
		}
	}
}