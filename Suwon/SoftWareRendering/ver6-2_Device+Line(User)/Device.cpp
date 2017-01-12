//
// Device.cpp :기본 렌더링 및 디바이스 설정/제거 소스.
//
// B3Yena S/W Renderer ver.1.0 
//
//
//
#include "windows.h"
#include "mmsystem.h"
#include "stdio.h"


#include "Device.h"

/////////////////////////////////////////////////////////////////////////////
//
// 디바이스 관련 전역 데이터들. 
//


// 렌더링 디바이스 관련 데이터.
LPB3YENA g_pB3Y = NULL;					//★
LPB3YENADEVICE9  g_pDevice = NULL;		//★

//백버퍼 접근...
#define  hRT   g_pDevice->GetRT()

// 화면 모드 설정 전역 변수.  
//
YENADISPLAYMODE g_Mode = { 800, 600 }; //★

// 시스템 폰트.
HFONT g_hSysFont = NULL;
COLORREF g_SysFnColor = RGB(0, 255, 0);	//기본 시스템 폰트 색상.

/////////////////////////////////////////////////////////////////////////////
//
// 기타 상태 변수들. 
//
BOOL g_ShowFrame = FALSE;


/////////////////////////////////////////////////////////////////////////////
//
// 렌더러 예나 및 관련 기타 정보 구성 : "엔진 셋업" 시 한번만 호출.
//               
int YenaSetup(HWND hwnd)
{

	// 예나 생성
	//
	g_pB3Y = B3YenaCreate9(YENA_VERSION);
	if (YN_INVALIED(g_pB3Y))
	{
		//Error...
		return YN_FAIL;
	}


	// 디스플레이 정보 구성.
	YENAPRESENT_PARAMETERS pp;
	ZeroMemory(&pp, sizeof(pp));
	pp.Width = g_Mode.Width;		//해상도 결정.(백버퍼 크기)
	pp.Height = g_Mode.Height;
	pp.BackBuffercnt = 1;			//백버퍼 개수. (현재는 1개만 가능)
	pp.Windowed = TRUE;				//풀스크린, 창모드 선택.(현재는 창모드만 가능)



	// 예나를 통해 렌더링 디바이스 생성 : DX9 대응★	
	//
	g_pB3Y->CreateDevice(g_hWnd,				//디바이스의 렌더링 목표 윈도우 핸들.
		&pp,									//디바이스 화면 구성 정보.
		YENACREATE_SOFTWARE_VERTEXPROCESSING,   //정점연산 방법 결정 (현재는 SW 만 가능)
		&g_pDevice								//성공시 리턴받을 디바이스 개체 포인터.
	);

	if (YN_INVALIED(g_pDevice))
	{
		//Error..
		return YN_FAIL;
	}


	// 디바이스 기본 상태 옵션 결정.
	// ...
	// ...



	// 시스템 폰트 생성.
	//
	g_hSysFont = CreateFont(
		12, 6,
		0, 0, 1, 0, 0, 0,
		DEFAULT_CHARSET,	//HANGUL_CHARSET  
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		FF_DONTCARE,
		(LPSTR)"굴림"
	);
	if (g_hSysFont == NULL)
	{
		// error..! 
		return YN_FAIL;
	}
	SelectObject(hRT, g_hSysFont);



	return YN_OK;

}

/////////////////////////////////////////////////////////////////////////////
//
// "엔진" 정보 제거 : 어플리케이션 종료시 한번만 호출.
//
void YenaRelease()
{
	// 폰트 제거.
	DeleteObject(g_hSysFont);


	// 렌더링 개체들 제거. 
	// 생성된 역순으로 제거해야 됩니다 
	// (그렇게 되도록 설계되었다 가정합니다.)
	//..
	SAFE_DELETE(g_pDevice);	//디바이스 제거★
	SAFE_DELETE(g_pB3Y);	//예나 제거★ 

}

/////////////////////////////////////////////////////////////////////////////
//
// 초당 프레임수 출력.
//
// 인자 : int x, y : 출력할 화면 좌표.
//
void PutFPS(int x, int y)
{
	HDC hdc = hRT;

	static int frm = 0;
	static UINT oldtime = GetTickCount();
	static UINT frmcnt = 0;
	static float  fps = 0.0f;

	++frmcnt;

	char msg[80];
	int time = GetTickCount() - oldtime;
	if (time >= 999)						// 0~999 밀리세컨드.. 1~1000이 아님
	{
		oldtime = GetTickCount();

		//1초간 증가된 프레임 수를 구합니다..
		frm = frmcnt;	frmcnt = 0;

		//초당 프래임 수를 계산합니다.
		fps = (float)(frm * 1000) / (float)time;
	}

	SetTextColor(hRT, g_SysFnColor);
	sprintf(msg, "fps=%.1f/%d    ", fps, time);
	TextOut(hdc, x, y, msg, strlen(msg));

}

/////////////////////////////////////////////////////////////////////////////
//
// 엔진 동기화용 경과시간 획득.
//
float GetEngineTime()
{
	static int oldtime = GetTickCount();
	int nowtime = GetTickCount();
	float dTime = (nowtime - oldtime) * 0.001f;
	oldtime = nowtime;

	return dTime;
}

/////////////////////////////////////////////////////////////////////////////
//
// 문자열 출력  
//
int DrawText(int x, int y, COLORREF col, char* msg, ...)
{
	va_list vl;
	char buff[4096] = "";
	va_start(vl, msg);
	vsprintf(buff, msg, vl);
	RECT rc = { x, y, x + 800, y + 600 };

	SetTextColor(hRT, col);
	int res = DrawText(hRT, buff, strlen(buff), &rc, DT_WORDBREAK);
	SetTextColor(hRT, g_SysFnColor);

	return res;
}




/*********************** end of file "Device.cpp" *********************/
