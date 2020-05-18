#include "stdafx.h"
#include "inputclass.h"
#include "graphicsclass.h"
#include "TimerClass.h"
#include "PositionClass.h"
#include "systemclass.h"


SystemClass::SystemClass()
{
}


SystemClass::SystemClass(const SystemClass& other)
{
}


SystemClass::~SystemClass()
{
}


bool SystemClass::Initialize()
{
	// À©µµ¿ì Ã¢ °¡·Î, ¼¼·Î ³ÐÀÌ º¯¼ö ÃÊ±âÈ­
	int screenWidth = 0;
	int screenHeight = 0;

	// À©µµ¿ì »ý¼º ÃÊ±âÈ­
	InitializeWindows(screenWidth, screenHeight);

	// m_Input °´Ã¼ »ý¼º. ÀÌ Å¬·¡½º´Â ÃßÈÄ »ç¿ëÀÚÀÇ Å°º¸µå ÀÔ·Â Ã³¸®¿¡ »ç¿ëµË´Ï´Ù.
	m_Input = new InputClass;
	if (!m_Input)
	{
		return false;
	}

	// m_Input °´Ã¼ ÃÊ±âÈ­
	if(!m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight))
	{
		MessageBox(m_hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// m_Graphics °´Ã¼ »ý¼º.  ±×·¡ÇÈ ·£´õ¸µÀ» Ã³¸®ÇÏ±â À§ÇÑ °´Ã¼ÀÔ´Ï´Ù.
	m_Graphics = new GraphicsClass;
	if (!m_Graphics)
	{
		return false;
	}

	// m_Graphics °´Ã¼ ÃÊ±âÈ­.
	if(!m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd))
	{
		return false;
	}

	// Å¸ÀÌ¸Ó °´Ã¼¸¦ ¸¸µì´Ï´Ù.
	m_Timer = new TimerClass;
	if(!m_Timer)
	{
		return false;
	}

	// Å¸ÀÌ¸Ó °´Ã¼¸¦ ÃÊ±âÈ­ÇÕ´Ï´Ù.
	if(!m_Timer->Initialize())
	{
		MessageBox(m_hwnd, L"Could not initialize the Timer object.", L"Error", MB_OK);
		return false;
	}

	// 위치 객체를 생성합니다
	m_Position = new PositionClass();
	if (!m_Position)
	{
		return false;
	}

	return true;
}


void SystemClass::Shutdown()
{
	// 위치 객체 반환
	if (m_Position)
	{
		delete m_Position;
		m_Position = 0;
	}

	// Å¸ÀÌ¸Ó °´Ã¼¸¦ ÇØÁ¦ÇÕ´Ï´Ù.
	if(m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	// m_Graphics °´Ã¼ ¹ÝÈ¯
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// m_Input °´Ã¼ ¹ÝÈ¯
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	// Window Á¾·á Ã³¸®
	ShutdownWindows();
}


void SystemClass::Run()
{
	// ¸Þ½ÃÁö ±¸Á¶Ã¼ »ý¼º ¹× ÃÊ±âÈ­
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	// »ç¿ëÀÚ·ÎºÎÅÍ Á¾·á ¸Þ½ÃÁö¸¦ ¹ÞÀ»¶§±îÁö ¸Þ½ÃÁö·çÇÁ¸¦ µ½´Ï´Ù
	while (true)
	{
		// À©µµ¿ì ¸Þ½ÃÁö¸¦ Ã³¸®ÇÕ´Ï´Ù
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// Á¾·á ¸Þ½ÃÁö¸¦ ¹ÞÀ» °æ¿ì ¸Þ½ÃÁö ·çÇÁ¸¦ Å»ÃâÇÕ´Ï´Ù
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// ±× ¿Ü¿¡´Â Frame ÇÔ¼ö¸¦ Ã³¸®ÇÕ´Ï´Ù.
			if (!Frame())
			{
				MessageBox(m_hwnd, L"Frame Processing Failed", L"Error", MB_OK);
				break;
			}
		}
		
		// »ç¿ëÀÚ°¡ ESCÅ°¸¦ ´­·¶´ÂÁö È®ÀÎ ÈÄ Á¾·á Ã³¸®ÇÔ
		if(m_Input->IsEscapePressed() == true)
		{
			break;
		}
	}
}


bool SystemClass::Frame()
{
	// ½Ã½ºÅÛ Åë°è¸¦ ¾÷µ¥ÀÌÆ® ÇÕ´Ï´Ù
	m_Timer->Frame();

	// ÀÔ·Â ÇÁ·¹ÀÓ Ã³¸®¸¦ ¼öÇàÇÕ´Ï´Ù
	if(!m_Input->Frame())
	{
		return false;
	}

	// 업데이트 된 위치를 계산하기 위한 프레임 시간을 설정합니다
	m_Position->SetFrameTime(m_Timer->GetTime());

	// 왼쪽 또는 오른쪽 화살표 키를 눌렀는지 확인하십시오 그렇지 않으면 카메라를 적절히 회전하십시오
	bool keyDown = m_Input->IsLeftArrowPressed();
	m_Position->TurnLeft(keyDown);

	keyDown = m_Input->IsRightArrowPressed();
	m_Position->TurnRight(keyDown);

	// 현재 뷰 포인트 회전을 가져옵니다
	float rotationY = 0.0f;
	m_Position->GetRotation(rotationY);

	// 그래픽 객체에 대한 프레임 처리를 수행합니다
	if(!m_Graphics->Frame(rotationY))
	{
		return false;
	}

	return m_Graphics->Render();
}


LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}


void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	// ¿ÜºÎ Æ÷ÀÎÅÍ¸¦ ÀÌ °´Ã¼·Î ÁöÁ¤ÇÕ´Ï´Ù
	ApplicationHandle = this;

	// ÀÌ ÇÁ·Î±×·¥ÀÇ ÀÎ½ºÅÏ½º¸¦ °¡Á®¿É´Ï´Ù
	m_hinstance = GetModuleHandle(NULL);

	// ÇÁ·Î±×·¥ ÀÌ¸§À» ÁöÁ¤ÇÕ´Ï´Ù
	m_applicationName = L"tutorial 16";

	// windows Å¬·¡½º¸¦ ¾Æ·¡¿Í °°ÀÌ ¼³Á¤ÇÕ´Ï´Ù.
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// windows class¸¦ µî·ÏÇÕ´Ï´Ù
	RegisterClassEx(&wc);

	// ¸ð´ÏÅÍ È­¸éÀÇ ÇØ»óµµ¸¦ ÀÐ¾î¿É´Ï´Ù
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int posX = 0;
	int posY = 0;

	// FULL_SCREEN º¯¼ö °ª¿¡ µû¶ó È­¸éÀ» ¼³Á¤ÇÕ´Ï´Ù.
	if (FULL_SCREEN)
	{
		// Ç®½ºÅ©¸° ¸ðµå·Î ÁöÁ¤Çß´Ù¸é ¸ð´ÏÅÍ È­¸é ÇØ»óµµ¸¦ µ¥½ºÅ©Åé ÇØ»óµµ·Î ÁöÁ¤ÇÏ°í »ö»óÀ» 32bit·Î ÁöÁ¤ÇÕ´Ï´Ù.
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Ç®½ºÅ©¸°À¸·Î µð½ºÇÃ·¹ÀÌ ¼³Á¤À» º¯°æÇÕ´Ï´Ù.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
	else
	{
		// À©µµ¿ì ¸ðµåÀÇ °æ¿ì 800 * 600 Å©±â¸¦ ÁöÁ¤ÇÕ´Ï´Ù.
		screenWidth = 800;
		screenHeight = 600;

		// À©µµ¿ì Ã¢À» °¡·Î, ¼¼·ÎÀÇ Á¤ °¡¿îµ¥ ¿Àµµ·Ï ÇÕ´Ï´Ù.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// À©µµ¿ì¸¦ »ý¼ºÇÏ°í ÇÚµéÀ» ±¸ÇÕ´Ï´Ù.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// À©µµ¿ì¸¦ È­¸é¿¡ Ç¥½ÃÇÏ°í Æ÷Ä¿½º¸¦ ÁöÁ¤ÇÕ´Ï´Ù
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);
}


void SystemClass::ShutdownWindows()
{
	// Ç®½ºÅ©¸° ¸ðµå¿´´Ù¸é µð½ºÇÃ·¹ÀÌ ¼³Á¤À» ÃÊ±âÈ­ÇÕ´Ï´Ù.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Ã¢À» Á¦°ÅÇÕ´Ï´Ù
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// ÇÁ·Î±×·¥ ÀÎ½ºÅÏ½º¸¦ Á¦°ÅÇÕ´Ï´Ù
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// ¿ÜºÎÆ÷ÀÎÅÍ ÂüÁ¶¸¦ ÃÊ±âÈ­ÇÕ´Ï´Ù
	ApplicationHandle = NULL;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// À©µµ¿ì Á¾·á¸¦ È®ÀÎÇÕ´Ï´Ù
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	// À©µµ¿ì°¡ ´ÝÈ÷´ÂÁö È®ÀÎÇÕ´Ï´Ù
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// ±× ¿ÜÀÇ ¸ðµç ¸Þ½ÃÁöµéÀº ½Ã½ºÅÛ Å¬·¡½ºÀÇ ¸Þ½ÃÁö Ã³¸®·Î ³Ñ±é´Ï´Ù.
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}