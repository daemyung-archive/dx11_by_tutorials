#pragma once

/////////////
// LINKING //
/////////////
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "DDSTextureLoader.h" // DDS 파일 처리
using namespace DirectX;

//////////////////////////
// warning C4316 처리용 //
/////////////////////////
#include "AlignedAllocationPolicy.h"