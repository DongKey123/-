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

char* g_WindowName = "B3Yena SW-Renderer 00 : LIne";




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

DWORD g_PtCheck = PT_NONE_;

//출력화면 DC (RenderTarget)
HDC g_hDC;


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
		MoveToEx(g_hDC, g_Sp.x, g_Sp.y, NULL);
		LineTo(g_hDC, g_Ep.x, g_Ep.y);
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
	MoveToEx(g_hDC, pt.x - 5, pt.y, NULL);
	LineTo(g_hDC, pt.x + 5, pt.y);

	MoveToEx(g_hDC, pt.x, pt.y - 5, NULL);
	LineTo(g_hDC, pt.x, pt.y + 5);
}

////////////////////////////////////////////////////////////////////////////// 
//
// 장면 그리기 시작. : 렌더링에 필요한 (디바이스의) 선위 작업을 수행.
//
void BegineScene()
{
	g_hDC = GetDC(g_hWnd);
	SetBkMode(g_hDC, TRANSPARENT);
}


////////////////////////////////////////////////////////////////////////////// 
//
// 장면 그리기 종료. : 렌더링에 필요한 (디바이스의) 후위 작업을 수행.
//
void EndScene()
{
	ReleaseDC(g_hWnd, g_hDC);
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
	FillRect(g_hDC, &rc, hBrush);
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

	//Sleep(5);		//--> 적당히 느리게..연출..
}


/////////////////////////////////////////////////////////////////////////////
//
// 초당 프레임수 출력.
//
// 인자 : int x, y : 출력할 화면 좌표.
//
void PutFPS(int x, int y)
{
	HDC hdc = g_hDC;

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

////////////////////////////////////////////////////////////////////////////// 
//
//  도움말. 
//
void ShowHelp()
{
	int x = 5, y = 5;
	char msg[512];

	PutFPS(x, y);

	SetTextColor(g_hDC, RGB(255, 255, 255));
	//SetBkMode(hdc, TRANSPARENT);

	sprintf(msg, "■ Software Renderer \" B3Yena \" : Line Drawing");
	TextOut(g_hDC, x, y += 16, msg, strlen(msg));

	sprintf(msg, "점 2개를 이용한 직선 그리기");
	TextOut(g_hDC, x, y += 16, msg, strlen(msg));

	sprintf(msg, "좌표입력 : L-Button");
	TextOut(g_hDC, x, y += 16, msg, strlen(msg));

	sprintf(msg, "Sp = {%d, %d}", g_Sp.x, g_Sp.y);
	TextOut(g_hDC, x, y += 16, msg, strlen(msg));

	sprintf(msg, "Ep = {%d, %d}", g_Ep.x, g_Ep.y);
	TextOut(g_hDC, x, y += 16, msg, strlen(msg));

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
		Clear(RGB(128, 128, 128));
		LineDraw();
		ShowHelp();
	}
	EndScene();

	// 장면 출력...
	// ... 
	Present();
}






/****************** end of file "Render.cpp" *********************************/