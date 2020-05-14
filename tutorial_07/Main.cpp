// Main.cpp: 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "SystemClass.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	// System 객체 생성
	SystemClass* System = new SystemClass;
	if (!System)
	{
		return -1;
	}

	// System 객체 초기화 및 실행
	if (System->Initialize())
	{
		System->Run();
	}

	// System 객체 종료 및 메모리 반환
	System->Shutdown();
	delete System;
	System = nullptr;

	return 0;
}