#pragma region 소개글
// 
// 김기홍교수님께 감사의 말씀을 올립니다.
// 
// 
// DX 내부구성 따라하기
// main.cpp :
// 2016.07.21 kdh2183@gmail.com
#pragma endregion

// Include Header
#include <windows.h>
#include "Render.h"

#pragma region 전역변수
// 전역변수들
HWND g_hWnd = NULL;
BOOL g_bLoop = TRUE;

int g_WindowWidth = 800;
int g_WindowHeight = 600;

char* g_ClassName = "SW-Rendering BFX";
extern char* g_WindowName;
#pragma endregion


//윈도우 설정 관련 함수
BOOL InitWindow(int width, int height);
void ResizeWindow(HWND hWnd, UINT NewWidth, UINT NewHeight);
INT MessagePump();

//메시지 처리 프로시져
LRESULT CALLBACK MsgProc(HWND, UINT, WPARAM, LPARAM);
#pragma region CALLBACK(__stdcall) 설명
////CALLBACK(__stdcall) 이란 ?
//인자를 오른쪽에서 왼쪽으로 불러들여 stack에 쌓는 것
//즉, 맨왼쪽에 있는 인자가 제일 위로 올라감
//// __cdecl 과 __stdcall 의 차이(윗 설명은 공통점)
// __cdel - 가변인자를 받을수 있음,caller(함수를 호출하는쪽)가 stack을 정리하여 stack을 정리하기 위한 명령어가 한줄 추가됨
// __stdcall - 가변인자를 받을수 없음,callee(함수 자신)가 stack을 정리한다. 따로 명령어가 추가되지 않음
//// 유의사항
// __stdcall은 빠르지만 가변인자를 받을 수 없다. OS가 사용하는 함수나 DLL에서는 __stdcall을 가능하면 사용

#pragma endregion

//////////////////////////////////////////////////////////////////////////
//WinMain : 메인함수
//
int APIENTRY WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd)
{
	//------------------------------------------------------------------//
	// 기본적인 윈도우 프레임 생성..                                //
	//------------------------------------------------------------------//
	if (!InitWindow(g_WindowWidth, g_WindowHeight)) return 0;

	//------------------------------------------------------------------//
	// DX 관련 객체 초기화                               //
	//------------------------------------------------------------------//
	//....
	RenderTargetCreate(g_hWnd);		//Back-Buffer : Render Target 생성


	//------------------------------------------------------------------//
	// 데이터 로딩                         //
	//------------------------------------------------------------------//
	if (!DataLoading())
	{
		g_bLoop = FALSE;
	}

	//------------------------------------------------------------------//
	// 메인 루프                      //
	//------------------------------------------------------------------//
	while (g_bLoop) //데이터 로딩이 끝날 시
	{
		if (!MessagePump())  //메세지 펌프 - 메세지를 꺼내는 행위
			break;

		SceneRender(); //Rendering
	}

	//------------------------------------------------------------------//
	// 어플리케이션 종료                                        // 
	//------------------------------------------------------------------//
	DataRelease();				//데이터 제거.
	RenderTargetRelease();		//Back-Buffer : Render Target 제거

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
//
// MsgProc :  윈도우 메세지 프로시져.
// 
LRESULT CALLBACK MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	//case WM_PAINT:
	//	{
	//		char* msg;
	//		PAINTSTRUCT rc;
	//		HDC hdc = BeginPaint(hwnd, &rc);
	//		SetTextColor(hdc, RGB(255, 255, 255));
	//		SetBkMode(hdc, TRANSPARENT); //백그라운드 모드 설정 - 투명하게
	//		msg = "SoftWare Renderer Project";
	//		TextOut(hdc, 270, 250, msg, strlen(msg));
	//		msg = "Hello World!";
	//		TextOut(hdc, 350, 270, msg, strlen(msg));
	//		EndPaint(hwnd, &rc);
	//	}
	//	break;
	case WM_LBUTTONDOWN:
	{
		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		LineUpdate(pt);
	}
	break;
	case WM_MOUSEMOVE: //★
	{
		//POINT pt; 
		g_VtxTemp.x = LOWORD(lParam);
		g_VtxTemp.y = HIWORD(lParam);
		//LineUpdate(pt);
	}
	break;
	case WM_RBUTTONDOWN://★
	{
		LineClear();			//★
	}
	break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			SendMessage(hwnd, WM_DESTROY, 0, 0);
			break;
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}
	return DefWindowProc(hwnd, msg, wParam, lParam);

}

/////////////////////////////////////////////////////////////////////////////
//
// InitWindow  : 윈도우 생성 함수.
//
// width  : 생성할 윈도우 가로 크기 (pixel)
// height : 생성할 윈도우 세로 크기 (pixel)
//
// 입력받은 크기에 맞추어 렌더링 영역을 생성하기 위해서 
// 클라이언트 영역을 포함 윈도우 전체 크기를 재조정합니다.
//
#pragma region 윈도우 프로그램 설명
/*
2D(2 - Demensional)게임 이건 3D 게임이건 윈도우에서는 윈도우 프로그래밍을 기본으로 합니다.
왜일까요 ? 대답은 간단합니다.프로그램이 윈도우 운영체제 위에서 동작 하기 때문

그런데 일반적인 윈도우 프로그램은 화면 출력을 위해 윈도우 운영체제가 제공하는 GDI를
사용하는데 게임에서는 2D나 3D를 표현하기 위해서 윈도우의 GDI를 반드시 이용하지는 않고
GDI보다 화면에 보다 많은 그래픽 자료들을 표현할 수 있도록 하드웨어 가속을 받는 그래픽
라이브러리를 이용합니다. 대표적인 그래픽 라이브러리가 DirectX와 OpenGL

라이브러리를 사용하려면, 즉 컴퓨터의 메모리에 적재하기 위해서 꼭 윈도우 핸들이 필요합니다.
그래서 윈도우 프로그래밍이 기본이 되는 것입니다.

윈도우 프로그램은 기억해야 될 함수와 개념들이 많이 있지만 윈도우 프로그램은 메시지
기반이라는 것과 
GetMessage()와 PeekMessage() 
//GetMessage() - 메세지를 받을때까지 대기
//PeekMessage() - 메세지를 받지않으면 다음행위 수행
함수의 차이점 정도만 꼭 기억하라고 이야기 하겠습니다.

이렇게 운영체제가 메시지 기반으로 작동을 하는 이유는 컴퓨터 자원의 효율을 높이기
위해서 입니다. 예를 들면 빨래를 하기 위해 세탁기를 돌린다고 할 때 빨래 다 될 때까지
세탁기 앞에서 가만히 있는 것이 아니라 세탁기 작동시키고 밥을 한다든가 청소를 한다든가
하는 것입니다. 아니면 소파에 앉아서 편안하게 쉬고 있다가 빨래가 다 끝나면 세탁기에서
종료 신호가 울리면 그 때 세탁기를 가서 빨래를 거두기만 하면 되는 데 바로 빨래가
다 되었다는 신호는 윈도우 운영체제에서는 메시지에 해당이 됩니다. 이러한 것을 컴퓨터에서는
작업-스케줄링(Job-Scheduling)이라고 하는데 좀 더 자세한 내용은 운영체제 관련 책들을
참고 하기 바랍니다.

그리고 윈도우 프로그래밍의 코드 흐름을 살펴본다면
윈도우 기반의 프로그램 코드는 다음과 같은 순서로 진행됩니다.
main() → 프로그램 핸들 얻기 → 윈도우 클래스 등록 → 윈도우 생성
→ Run → 윈도우 소멸 → 윈도우 클래스 해제

main() → GetModuleHandle() → RegisterClass() → CreateWindow()
→ PeekMessage() or GetMessage() → TranslateMessage() → DispatchMessage()
→ WindowProcedure() → DestroyWindow() → UnRegisterClass()

< 1-1 프로그램 핸들 얻기>
콘솔(도스 창이라고 생각하면 됩니다.)기반 프로그램이라면 GetModuleHandle()이라는
함수가 필요하지만 윈도우 프로그램이라면 WinMain(HINSTANCE hInst)에서
GetModuleHandle() 함수를 사용하지 않고, hInst 인수를 이용하면 됩니다.
(GetModuleHandle() 함수로 HINSTANCE 값을 구해보면 hInst와 동일합니다.)
그리고 WinMain을 사용하면 콘솔 창이 만들어지지 않습니다.


< 1-2 윈도우 클래스 등록>
WNDCLASS 구조체의 인스턴스를 생성하고 이 인스턴스에 데이터를 채워서 RegisterClass()
함수에 전달을 하면 윈도우 클래스가 등록이 됩니다. WNDCLASS 인스턴스의 내용에서 꼭
기억해야 할 것이 있는데 className 과 다음에 윈도우를 생성하는 CreateWindow 함수에서
className은 반드시 동일해야 합니다. 그렇지 않으면 윈도우가 제대로 생성이 되지 않습니다.
아주 중요한 것이 남아 있는데 운영체제가 호출하는 사용자 정의 메시지 처리함수를 이
인스턴스의 lpfnWndProc함수 포인터에 연결해야 하는 것도 잊지 말아야 합니다.


< 1-3 윈도우 생성>
윈도우 클래스를 운영체제에 등록을 했다면 이제 윈도우를 만들 차례입니다. 윈도우는
CreateWindow() 함수로 만듭니다. 간혹 CreateDialog() 함수나 DialogBox() 함수로 만들 수도
있지만 이 함수는 이 장에서는 잊으세요. CreateWindow() 함수는 윈도우 만드는 것이 성공한다면
NULL 이외의 값을 돌려 줍니다. 이 값은 HWND형에 꼭 저장하시기 바랍니다.

CreateWindow() 함수는 윈도우만 만들었지 화면에 즉시 보여주지 않습니다. 윈도우를 화면에 보이게
하려면 ShowWindow(hWnd, SW_SHOW) 함수를 호출해야 합니다. 여기서 hWnd는 CreateWindow()함수가
돌려준 값입니다.


< 1-4 함수로 분리>
위의 예제처럼 main()함수 안에 코드를 전부 구현해도 전혀 상관 없습니다. 그러나 우리가
작성하려고 하는 프로그램의 코드 량은 수만에서 수십만 줄 됩니다. 이런 것은 한 함수 안에
만든다는 것은 거의 불가능한 일입니다. 가능하더라도 이것을 시도하는 사람은 없습니다. 그러한
이유는 코드를 만드는 것뿐만 아니라 논리적인 버그, 코드의 재사용에서 바람직하지 않기
때문입니다.

프로그램을 만드는 여러 가지 암묵적인 원칙 중에 Divide and Conquer! (분할해서 정복하라!)
라는 격언을 적용하는데 이것이 하나의 커다란 프로그램을 적절히 작은 완전한 프로그램
(이것을 모듈이라고 합니다.)으로 나누어서 제작을 합니다.

C언어에서는 이것을 함수 단위로 만들 수 있습니다. 함수로 만들기 위해 어떤 기준을 가지고
함수로 나눌 것인지 원칙이 있어야 합니다.
지금은 윈도우 생성(Create) → 윈도우 가동(Run) → 윈도우 소멸(Cleanup) 세 부분으로 나누어서
함수를 만들어 봅시다.
또한 윈도우 생성 Create() 함수의 경우 성공과 실패의 두 가지가 있으므로 정수형(int) 반환을
하도록 하고 성공은 0, 실패의 경우 -1를 반환하도록 합니다. Run() 함수도 정수형 반환을 넣어서
만들도록 합니다. Cleanup() 함수는 윈도우의 해제 부분임으로 굳이 성공과 실패의 반환이
필요가 없어서 void로 합니다.
*/
#pragma endregion

BOOL InitWindow(int width, int height)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_CLASSDC; //문제점 : http://m.blog.naver.com/bastard9/140114813809
	wc.lpfnWndProc = MsgProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = ::GetModuleHandle(NULL);
	wc.hCursor = NULL;	
	wc.hIcon = NULL;
	wc.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_ClassName;
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);
	
	//윈도우 생성
	HWND hWnd = ::CreateWindow(
		g_ClassName,
		g_WindowName,
		//WS_OVERLAPPEDWINDOW, 		
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, //윈도우 스타일
		//WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME,
		0, //윈도우 생성 x 좌표
		0, //윈도우 생성 y 좌표
		width, height,
		GetDesktopWindow(),//부모 윈도우
		NULL, //메뉴 핸들
		wc.hInstance, // 인스턴스 핸들
		NULL // lparam 에 대한 포인터
	);
	if (hWnd == NULL) return FALSE;

	::ShowWindow(hWnd, SW_SHOWDEFAULT);
	::UpdateWindow(hWnd); // 화면 다시 셋팅

	//윈도우 핸들 전역변수 복사
	g_hWnd = hWnd;

	//클라이언트 영역 크기 재조정 : 필수,삭제금지
	ResizeWindow(hWnd, width, height);

	return TRUE;

}


/////////////////////////////////////////////////////////////////////////////
// 
// int MessagePump() : 윈도우 메세지 처리 함수.
//
// 보다 빠른 메세지 처리를 위해서 메세지 펌프를 수정한다            
// 윈도우즈(OS)로부터 이 어플리케이션으로 특별히 메세지가 날라오지  
// 않는다면  게임을 진행 하도록 합니다.                               
// 향후에 한번더 버전업 할 것임.    
INT MessagePump()
{
	MSG msg; ::ZeroMemory(&msg, sizeof(msg));

	while (1)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))//메세지 큐에 메세지가 있으면 처리
		{
			if (msg.message == WM_QUIT) //윈도우 종료를 위해 리턴
				return FALSE;

			//나머지 메세지 처리
			::TranslateMessage(&msg); //키보드 메시지 발생시 메시지(WM_CHAR)를 만드는 역할 
			::DispatchMessage(&msg); //메시지 큐에서 꺼낸 메시지를 윈도우의 메시지 처리(WndProc)함수로 전달
		}
		else//메세지가 없다면 게임(렌더링)을 진행
		{
			return TRUE;
		}
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
//
// void B3D_ResizeWindow(UINT NewWidth, UINT NewHeight)
//
// 창모드의 경우 해상도 사이즈에 맞추어 전체 윈도우 크기를 재조정합니다. 
// ex)
//'클라이 언트'영역을 640 * 480 에 맞추어  전체 윈도우의 크기를 
// 다시 보정합니다. --> 그래야 '게임 이미지'가 모두 화면에 보인다.
//
// 인  자 :  클라이언트 영역의 크기 - NewWidth, NewHeight
// 리턴값 :  없음. 
void ResizeWindow(HWND hWnd, UINT NewWidth, UINT NewHeight)
{
	//현재 윈도우의 스타일 구하기
	RECT oldrc;
	DWORD sytle = (DWORD) ::GetWindowLong(hWnd, GWL_STYLE);		//윈도우 스타일을 가지고옴
	DWORD exstyle = (DWORD) ::GetWindowLong(hWnd, GWL_EXSTYLE); //확장된 윈도우 스타일을 가지고옴

	//현재 윈도우 '전체'크기 (스크린좌표) 를 얻는다
	::GetWindowRect(hWnd, &oldrc);

	//새로 생성될 윈도우의 '클라이언트' 영역 크기 계산하기
	RECT newrc;
	newrc.left = 0;
	newrc.top = 0;
	newrc.right = NewWidth;
	newrc.bottom = NewHeight;

	//newrc 만큼의 클라이언트 영역을 포함하는 윈도우 '전체' 크기를 구합니다.
	//현재 '메뉴' 는 없다는 가정하에 처리되고 있음.
	//계산된 결과는 newrc 에 다시 들어온다. ( 스크린 좌표 )
	//exstyle &= ~WS_EX_TOPMOST;
	::AdjustWindowRectEx(&newrc, sytle, NULL, exstyle); //윈도우 크기셋팅

	//보정된 윈도우의 너비와 폭을 구합니다. 
	int width = (newrc.right - newrc.left);
	int height = (newrc.bottom - newrc.top);

	//새로운 크기를 윈도우에 설정합니다.
	::SetWindowPos(hWnd, HWND_NOTOPMOST,
		oldrc.left, oldrc.top,
		width, height, SWP_SHOWWINDOW);
}


//////////////////////end