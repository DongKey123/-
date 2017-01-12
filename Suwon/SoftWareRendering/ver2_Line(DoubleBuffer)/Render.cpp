//
// Render.cpp
#include "Windows.h"  
#include "Render.h"
#include "stdio.h"

#pragma warning(disable:4996)
/*
VS2005 부터는 CRT 라이브러리 함수의 문자열 다루는 함수의 안정성에
많이 신경을 쓴다.
그래서 기존의 문자열 함수에 _s를 붙여서
버퍼의 길이를 지정하도록 권장하고 있는데 이것 때문에
기존의 strcpy 같은 문자열 함수에서 C4996 warning 가 발생한다.
--> 경고 무시
*/

char* g_WindowName = "B3Yena SW-Renderer 00 : LIne (+BackBuffer)";




/////////////////////////////////////////////////////////////////////////////
// 
// 추가 데이터 및 변수는 여기에... 

// 라인의 시작/끝
POINT g_Sp, g_Ep;

enum {
	PT_NONE_ = 0x00,				//설정된 점 없음
	PT_SP_	= 0x01,					//1번째 점 설정됨.
	PT_EP_	= 0x02,					//2번째 점 설정됨.

	PT_COMPLETED_ = (PT_SP_|PT_EP_)	//점 입력 완료.--> 라인 그리기
};

DWORD g_PtCheck = PT_NONE_; //2개의 점이 모두 설정되었는지 확인용 변수

//출력화면 DC (RenderTarget)
HDC g_hDC;

// 렌더타겟(RenderTarget : Back-Buffer) 구성용 핸들.//★
//
HBITMAP g_hBmpRT = NULL;				
HDC		g_hSurfaceRT = NULL;			
COLORREF g_BkColor = RGB(0, 0, 255);
#define  g_hRT   g_hSurfaceRT

#pragma region 더블 버퍼링 설명
/*
1. 동적 연결 라이브러리
=> 윈도우즈에는 세가지 동적 연결 라이브러리로 구성되있다.
KERNEL : 메모리를 관리하고 프로그램을 실행시키는 라이브러리
USER : 유저 인터페이스와 윈도우를 관리하는 라이브러리
GDI : 화면처리와 그래픽을 담당하는 라이브러리

2.DC는 Device Context의 약자로 출력에 관한 모든 정보를 가지는 데이터 구조체
즉 DC는 그리기에 필요한 다양한 정보들의 저장소 역할을 함
GDI 모듈에 의해 관리되고 그리는 함수들의 참조하는 것이다.

3.더블 버퍼링
hdc가 뿌려준 기존의 화면 -> 다음에 그려줄 이미지를 BackDC에 임시 저장 
-> BackDC에 내용을 MemDC에 삽입한다. -> MemDC에 내용을 hdc로 넘긴다. -> 
hdc에서 화면에 뿌려준다.

*/
#pragma endregion



/////////////////////////////////////////////////////////////////////////////
//
int DataLoading()
{

	//추가 코드는 여기에...
	//...
	g_Sp.x = 0; g_Sp.y = 0;
	g_Ep.x = 0; g_Ep.y = 0;

	g_PtCheck = PT_NONE_;


	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
//
void DataRelease()
{
	//데이터 해제 코드의 추가는 여기에..
	//...  

}

////////////////////////////////////////////////////////////////////////////// 
//
void LineUpdate(POINT pt)
{
	switch (g_PtCheck)
	{
		//초기 새로 시작할 경우..
	case PT_NONE_:
		//입력완료후 새로 입력할 경우..
	case PT_COMPLETED_:
		g_Sp = pt;   g_PtCheck = PT_SP_;			//1번째 입력 완료.
		g_Ep.x = 0;  g_Ep.y = 0;
		break;

		//2번째 입력시...
	case PT_SP_:
		g_Ep = pt;  g_PtCheck = PT_COMPLETED_;		//입력 완료 처리.
		break;
	}

}

////////////////////////////////////////////////////////////////////////////// 
//
void LineDraw()
{
	switch (g_PtCheck)
	{
	case PT_NONE_:		//그리기 없음.
		break;

	case PT_SP_:
		CrossDraw(g_Sp);		//시작점 표시.
		break;

	case PT_COMPLETED_:			//완료. 모두 그리기.
		CrossDraw(g_Sp);
		MoveToEx(g_hRT, g_Sp.x, g_Sp.y, NULL);
		LineTo(g_hRT, g_Ep.x, g_Ep.y);
		CrossDraw(g_Ep);
		break;
	}
}

////////////////////////////////////////////////////////////////////////////// 
//
// 십자선 그리기 : 마우스 클릭 위치
//
void CrossDraw(POINT pt)
{
	MoveToEx(g_hRT, pt.x - 5, pt.y, NULL);
	LineTo(g_hRT, pt.x + 5, pt.y);

	MoveToEx(g_hRT, pt.x, pt.y - 5, NULL);
	LineTo(g_hRT, pt.x, pt.y + 5);
}

////////////////////////////////////////////////////////////////////////////// 
//
// 장면 그리기 시작. : 렌더링에 필요한 (디바이스의) 선위 작업을 수행.
//
void BegineScene()
{
	//g_hDC = GetDC(g_hWnd); 화면DC 사용않함. 렌더타겟을 계속 사용...★
	SetBkMode(g_hRT, TRANSPARENT);
}


////////////////////////////////////////////////////////////////////////////// 
//
// 장면 그리기 종료. : 렌더링에 필요한 (디바이스의) 후위 작업을 수행.
//
void EndScene()
{
	//ReleaseDC(g_hWnd, g_hDC); --> 화면DC 사용않함. 렌더타겟을 계속 사용...★
}

////////////////////////////////////////////////////////////////////////////// 
//
// 렌더타겟 클리어.
//
void Clear(COLORREF col)
{
	HBRUSH hBrush = CreateSolidBrush(col);
	RECT rc;
	GetClientRect(g_hWnd, &rc);
	FillRect(g_hRT, &rc, hBrush);
	DeleteObject(hBrush);
}

////////////////////////////////////////////////////////////////////////////// 
//
// RT (Render Target) 의 내용(렌더링된 장면scene)을 Front Buffer 로 출력.
//
void Present()
{
	//Swap Chain 을 Flip 하는 함수의 '이름'을 가지고 있지만...
	//여기서는 단순 지연처리만.. 
	//너무 빠른 그리기는 눈에 잘 보이지 않는다.(또는 깜박거림Flickering..)
	//....

	RECT rc;
	GetClientRect(g_hWnd, &rc);
	HDC hdc = GetDC(g_hWnd);
	BitBlt(hdc,0,0, rc.right, rc.bottom, g_hSurfaceRT, 0, 0, SRCCOPY);
	ReleaseDC(g_hWnd, hdc);

}


/////////////////////////////////////////////////////////////////////////////
//
// 초당 프레임수 출력.
//
// 인자 : int x, y : 출력할 화면 좌표.
//
void PutFPS(int x, int y)
{
	HDC hdc = g_hRT;

	static int frm = 0;
	static UINT oldtime = GetTickCount();
	static UINT frmcnt = 0;
	static float  fps = 0.0f;
	//static float evfps = 0;		//평균 프레임.

	++frmcnt;

	char msg[80];
	int time = GetTickCount() - oldtime;
	if (time >= 999) // 0~999 밀리세컨드.. 1~1000이 아님
	{
		oldtime = GetTickCount();

		//1초간 증가된 프레임 수를 구합니다..
		frm = frmcnt;	frmcnt = 0;

		//초당 프래임 수를 계산합니다.
		fps = (float)(frm * 1000) / (float)time;
	}

	SetTextColor(hdc, RGB(255, 255, 255));
	sprintf(msg, "fps=%.1f/%d    ", fps, time);
	TextOut(hdc, x, y, msg, strlen(msg));

#pragma region GetTickCount설명
	/*
	DWORD GetTickCount(VOID);
	=> OS부팅할 때부터 지나간 시간을 msec 단위로 돌려주는 함수이다. 최대 49.7일간의 카운트를 유지할 수 있다.
	*/
#pragma endregion

}

///////////////////////////////////////////////////////////////////////////////
//
//  백버퍼용 렌더타겟을 생성.★
//
int RenderTargetCreate(HWND hwnd)
{
	RECT rc; GetClientRect(hwnd, &rc);
	HDC hdc = GetDC(hwnd);
	g_hSurfaceRT = CreateCompatibleDC(hdc);//렌더타겟,DC 핸들 생성(Surface)
										   //http://blog.naver.com/PostView.nhn?blogId=unbraid&logNo=29274684&redirect=Dlog&widgetTypeCall=true
	g_hBmpRT = (HBITMAP)CreateCompatibleBitmap(hdc, rc.right, rc.bottom);	//렌더타겟,비트멥 생성.
	SelectObject(g_hSurfaceRT, g_hBmpRT);
	ReleaseDC(hwnd, hdc);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 
// 렌더타겟 제거.★
//
void RenderTargetRelease()
{
	DeleteObject(g_hBmpRT);
	DeleteDC(g_hSurfaceRT);
}

/////////////////////////////////////////////////////////////////////////////
//
// 문자열 출력 ★   
//
int DrawText(int x, int y, COLORREF col, char* msg, ...)
{
	va_list vl;
	char buff[4096] = "";
	va_start(vl, msg);
	vsprintf(buff, msg, vl);
	RECT rc = { x, y, x + 800, y + 600 };

	SetTextColor(g_hRT, col);
	int res = DrawText(g_hRT, buff, strlen(buff), &rc, DT_WORDBREAK);
	SetTextColor(g_hRT, RGB(255, 255, 255));

	return res;
}

////////////////////////////////////////////////////////////////////////////// 
//
//  도움말. 
//
void ShowHelp()
{
	static bool bShow = true;
	if (IsKeyUp(VK_F1)) bShow ^= true;

	PutFPS(1, 1);

	if (!bShow) return;


	// Today's Topic.	 
	int x = 1, y = 16;
	COLORREF col = RGB(255, 255, 255);
	DrawText(x, y, col, "■ %s", g_WindowName);
	DrawText(x, y += 16, col, "점 2개를 이용한 직선 그리기");
	DrawText(x, y += 16, col, "좌표입력 : L-Button");


	DrawText(x, y += 16, col, "Sp = {%d, %d}", g_Sp.x, g_Sp.y);
	DrawText(x, y += 16, col, "Ep = {%d, %d}", g_Ep.x, g_Ep.y);

	y += 16;
	DrawText(x, y += 16, col, "도움말:F1");


	y += 100;
	char* msg =
		"더블버퍼링 : \"BackBuffer\" 추가.\n"
		"속도/깜박거림 비교.";
	DrawText(x, y, RGB(255, 255, 0), msg);


}



////////////////////////////////////////////////////////////////////////////// 
//
// 장면 그리기 : 
//
void SceneRender()
{

	// 장면 갱신 
	//
	// 기하 데이터: 주인공, 몬스터, 오브제 이동, 충돌검사 등등..
	// ...
	//
	// 게임 로직 이벤트,층돌,점수계산..
	// ...


	// 장면 그리기...
	// ....
	BegineScene();
	{
		Clear(RGB(0, 0, 255));
		LineDraw();
		ShowHelp();
	}
	EndScene();

	// 장면 출력...
	// ... 
	Present();
}






/****************** end of file "Render.cpp" *********************************/