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
	// 폰트 데이터를 포함한 텍스트 파일에로드한다.
	if (!LoadFontData(fontFilename))
	{
		return false;
	}

	// 폰트 문자가 있는 텍스처를 로드합니다.
	return LoadTexture(device, textureFilename);
}


void FontClass::Shutdown()
{
	// 폰트 텍스처를 해제합니다.
	ReleaseTexture();

	// 폰트 데이터를 해제합니다.
	ReleaseFontData();
}


bool FontClass::LoadFontData(const char* filename)
{
	ifstream fin;
	int i;
	char temp;


	// 글꼴 간격 버퍼를 만듭니다.
	m_Font = new FontType[95];
	if (!m_Font)
	{
		return false;
	}

	// 문자 사이의 글꼴 크기와 간격을 읽습니다.
	fin.open(filename);
	if (fin.fail())
	{
		return false;
	}

	// 텍스트에 사용 된 ASCII 문자 95 개를 읽습니다.
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

	// 파일을 닫습니다.
	fin.close();

	return true;
}


void FontClass::ReleaseFontData()
{
	// 폰트 데이터 배열을 해제한다.
	if (m_Font)
	{
		delete[] m_Font;
		m_Font = 0;
	}
}


bool FontClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
	// 텍스처 오브젝트를 생성한다.
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	// 텍스처 오브젝트를 초기화한다.
	return m_Texture->Initialize(device, filename);
}


void FontClass::ReleaseTexture()
{
	// 텍스처 오브젝트를 릴리즈한다.
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
	// 입력 정점을 VertexType 구조로 강제 변환합니다.
	VertexType* vertexPtr = (VertexType*)vertices;

	// 문장의 글자 수를 구하십시오.
	int numLetters = (int)strlen(sentence);

	// 정점 배열에 대한 인덱스를 초기화합니다.
	int index = 0;
	int letter = 0;

	// 각 문자를 쿼드 위에 그립니다.
	for (int i = 0; i<numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32;

		// 문자가 공백이면 3 픽셀 위로 이동합니다.
		if (letter == 0)
		{
			drawX = drawX + 3.0f;
		}
		else
		{
			// 쿼드의 첫 번째 삼각형.
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // 왼쪽 위
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);  // 오른쪽 아래
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX, (drawY - 16), 0.0f);  // 왼쪽 아래
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 1.0f);
			index++;

			// 쿼드의 두 번째 삼각형.
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // 왼쪽 위
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX + m_Font[letter].size, drawY, 0.0f);  // 오른쪽 위
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);  // 오른쪽 아래
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
			index++;

			// 문자의 크기와 한 픽셀 씩 그리기를 위해 x 위치를 업데이트합니다.
			drawX = drawX + m_Font[letter].size + 1.0f;
		}
	}
}