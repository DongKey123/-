//
// Render.cpp
#include "Windows.h"  
#include "Render.h"
#include "Device.h"
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

char* g_WindowName = "B3Yena SW-Renderer 04 :RenderState 02 Back-Face Culling";

///////////////////////////////////////////////////////////////////////////// 
//
// 정점 관련 데이터들.
//

// 정점 버퍼 : 정점 데이터 관리용 개체.
//
LPB3YENAVERTEXBUFFER9 g_pVB = NULL;


// 렌더링할 정점의 형식(VERTEX FORMAT) 을 결정합니다.
// 2D 용 { 출력 좌표(Screen), 색상 } 으로 구성.
//
struct COLVTX
{
	float x, y;			//2D 화면좌표.
};

// 정점 형식(규격) 
#define FVF_COLVTX (B3YFVF_XY)

int	InitVB();


/////////////////////////////////////////////////////////////////////////////
// 
// 추가 데이터 및 변수는 여기에... 
BOOL g_bWireFrame = FALSE;		//와이어 프레임 출력 플래그.
BOOL g_bCulling = FALSE;			//뒷면제거 플래그.★


//#pragma region 더블 버퍼링 설명
///*
//1. 동적 연결 라이브러리
//=> 윈도우즈에는 세가지 동적 연결 라이브러리로 구성되있다.
//KERNEL : 메모리를 관리하고 프로그램을 실행시키는 라이브러리
//USER : 유저 인터페이스와 윈도우를 관리하는 라이브러리
//GDI : 화면처리와 그래픽을 담당하는 라이브러리
//
//2.DC는 Device Context의 약자로 출력에 관한 모든 정보를 가지는 데이터 구조체
//즉 DC는 그리기에 필요한 다양한 정보들의 저장소 역할을 함
//GDI 모듈에 의해 관리되고 그리는 함수들의 참조하는 것이다.
//
//3.더블 버퍼링
//hdc가 뿌려준 기존의 화면 -> 다음에 그려줄 이미지를 BackDC에 임시 저장 
//-> BackDC에 내용을 MemDC에 삽입한다. -> MemDC에 내용을 hdc로 넘긴다. -> 
//hdc에서 화면에 뿌려준다.
//
//*/
//#pragma endregion



/////////////////////////////////////////////////////////////////////////////
//
int DataLoading()
{
	//추가 코드는 여기에...
	//...

	//정점버퍼 생성.
	InitVB();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
void DataRelease()
{
	//데이터 해제 코드의 추가는 여기에..
	//...  
}

///////////////////////////////////////////////////////////////////////////// 
//
// 정점 버퍼 구성 함수.
//
int	InitVB()
{
	//삼각형을 위한 3개의 정점 선언. : 정점의 좌표값은 화면(Screen)좌표.
	COLVTX	Vertices[] = {

		//Face 0 : 정삼각형.(CW) 
		{ 50.0f, 250.0f },		//v0
		{ 150.0f,  50.0f },		//v1
		{ 250.0f, 250.0f },		//v2

		//Face 1 : 역삼각형.(CCW) 방향 주의! ★
		{ 50.0f, 250.0f },
		{ 150.0f, 450.0f },
		{ 250.0f, 250.0f },

		//Face 2: 빗각 삼각형 (CW) 테스트 삼각형 
		{ 300.0f, 500.0f },
		{ 400.0f, 300.0f },
		{ 480.0f, 430.0f },

		//Face 3: 빗각 삼각형 (CCW) 테스트 삼각형 ★
		{ 500.0f, 430.0f },
		{ 680.0f, 500.0f },
		{ 600.0f, 300.0f },

		//Face 4 : 직각 삼각2 (CW)
		{ 40.0f,  30.0f },		//v12
		{ 90.0f,  30.0f },		//v13
		{ 90.0f, 100.0f },		//v14

		//Face 5 : 직각 삼각1 (CCW) ★
		{ 10.0f,  30.0f },		//v15
		{ 10.0f, 100.0f },		//v16
		{ 60.0f, 100.0f },		//v17 
								// */
	};



	//정점 버퍼 생성.
	if (YN_FAILED(g_pDevice->CreateVertexBuffer(
		sizeof(Vertices),			//'정점 버퍼'의 크기 (바이트)
		FVF_COLVTX,					//'정점' 규격.
		B3YPOOL_SYSTEM,				//시스템메모리 사용. (현재는 이 옵션만 유효) 
		&g_pVB						// 성공시 리턴되는 버퍼 포인터ㅣ
	)))
	{
		return YN_FALSE;
	}

	//버퍼 주소 얻기 : 실제 DX 의 그것과는 역활이 다르지만, 
	//동작의 결과는 동일합니다.
	VOID*	pBuff;
	if (YN_FAILED(g_pVB->Lock(0, (void**)&pBuff)))
	{
		return YN_FALSE;
	}

	//버퍼에 정점 데이터 채우기.
	memcpy(pBuff, Vertices, sizeof(Vertices));

	//DX 에서는 작업완료후 획득한 주소/메모리의 관리를 위해
	//Unlock 을 해야하지만 우리는 이 동작을 무시하겠습니다. 
	g_pVB->Unlock();


	//...작업완료...
	//...이제 적절한 때에 렌더링을 수행하면 되겠습니다.
	//...


	return YN_OK;

}



/////////////////////////////////////////////////////////////////////////////// 
//
void ObjDraw()
{

	//정점 버퍼 등록 : DX 의 그것과 동일명 사용.
	g_pDevice->SetStreamSource(g_pVB, sizeof(COLVTX));

	//정점 형식 설정.
	g_pDevice->SetFVF(FVF_COLVTX);

	//'기하도형' 렌더링 : 지금부터 모든 그리기가 시작됩니다...
	// DX 메소드와 (거의)동일한 시그니쳐(Signature) 를 구현하는 것이 목표입니다.
	g_pDevice->DrawPrimitive(B3YPT_TRIANGLELIST, 0, 6);

}



/////////////////////////////////////////////////////////////////////////////// 
//
// 렌더링 상태 등의 공통 엔진/시스템 상태 갱신.
//
void SystemUpdate()
{
	if (IsKeyUp(VK_SPACE)) g_bWireFrame ^= TRUE;

	g_pDevice->SetRenderState(B3YRS_FILLMODE, g_bWireFrame ? B3YFILL_WIREFRAME : B3YFILL_SOLID);		

	//컬링 테스트.
	if (IsKeyUp(VK_F5))  g_bCulling ^= TRUE;			//★

	//반시계방향 컬링<테스트1>★ 
	g_pDevice->SetRenderState(B3YRS_CULLMODE, g_bCulling ? B3YCULL_CCW : B3YCULL_NONE);

	//시계방향 컬링<테스트2>★ 
	//g_pDevice->SetRenderState(B3YRS_CULLMODE,  g_bCulling? B3YCULL_CW : B3YCULL_NONE);  

}



/////////////////////////////////////////////////////////////////////////////// 
//
//  도움말. 
//
void ShowInfo()
{
	static bool bShow = true;
	if (IsKeyUp(VK_F1)) bShow ^= true;

	if (!bShow)
	{
		//DrawText(1,20, D3DXCOLOR(1, 1, 0, 1), "[Help] F1"); 
		return;
	}


	// Today's Topic.
	{
		int x = 300, y = 50;
		COLORREF col = RGB(255, 255, 255);
		DrawText(x, y, col, "■ %s", g_WindowName);

		y += 24;
		char* msg =
			"1.정점(Vertex) 을 구성합니다.\n"
			"2.삼각형(Face) 을 구성합니다.\n"
			"3.정점 버퍼 클래스를 제작합니다.\n"
			"4.삼각형을 라인(WireFrame) 으로 렌더링합니다.\n"
			"5.삼각형을 지정 색상으로 채웁니다(Filling)"
			"\n"
			"6.렌더링 상태를 구현합니다 : SetRenderState\n"
			"7.채우기 상태 (Fill Mode)를 구현합니다 : SPACE BAR\n"
			"8.뒷면 컬링 상태(Back -Face Culling Mode)를 구협합니다: F5\n";

		DrawText(x, y, col, msg);
	}


	int x = 300, y = 300;
	static int cnt = 0;
	//DrawText(x, y, RGB(255, 255, 0), "■ _DrawLine (ver. GDI)");

}




////////////////////////////////////////////////////////////////////////////// 
//
// 장면 그리기 : 
//
void SceneRender()
{

	//-------------------------------
	// 엔진/시스템 갱신.
	//------------------------------- 
	//엔진 경과시간 얻기.
	//float dTime = GetEngineTime();


	// 장면 갱신 
	SystemUpdate();		
	// 기하 데이터: 주인공, 몬스터, 오브제 이동, 충돌검사 등등..
	// ...
	// 게임 로직 이벤트,층돌,점수계산..
	// ...



	//-------------------------------
	// 장면 그리기 시작.. 
	//-------------------------------

	//장면 그리기 준비..
	g_pDevice->BeginScene();					
	{
		//렌더타겟(배경) 지우기..	
		g_pDevice->Clear(RGB(0, 0, 255));		

		//..지형, 오브제 그리기..등등..
		ObjDraw();


		//도움말 및 기타 렌더링 정보 출력.
		PutFPS(1, 1);
		ShowInfo();

	}
	//장면 그리기 종료...
	g_pDevice->EndScene();						


	//-------------------------------
	// 장면 그리기 종료.
	// 1회의 게임 루프 종료.
	//------------------------------- 
	g_pDevice->Present();			//화면출력 : Flip! 

}






/****************** end of file "Render.cpp" *********************************/