//
// B3Yena.h : DX 기본 헤더 파일.

#pragma once

#include "Define.h"

/////////////////////////////////////////////////////////////////////////////// 
//
// 렌더타겟 (백버퍼) - 정보 설정 구조체 : D3DPRESENT_PARAMETERS 대응 ★
//
struct YENAPRESENT_PARAMETERS {

	DWORD Width;			//해상도 : 클라이언트 영역기준.
	DWORD Height;
	DWORD BackBuffercnt;	//백버퍼 개수
	BOOL  Windowed;			//창모드 실행?

};

/////////////////////////////////////////////////////////////////////////////// 
//
//  해상도 정보 구조체. : D3DDISPLAYMODE 대응 ★
//
struct YENADISPLAYMODE {
	DWORD Width, Height;
};

/////////////////////////////////////////////////////////////////////////////// 
//
// 정점 처리 - 가속화 정의.  : D3DCREATE 대응 ★
//
enum BEHAVIOR_FLAG
{
	YENACREATE_SOFTWARE_VERTEXPROCESSING,	//CPU 가 정점 연산을 처리. 
	YENACREATE_HARDWARE_VERTEXPROCESSING,	//GPU 가 정점 연산을 처리.
	//YENACREATE_MIXED_VERTEXPROCESSING		//CPU / GPU 혼성 연산 가능.
	//YENACREATE_MULTITHREADED				//다중쓰레드 환경 호환성 추가.
	//...
};

/////////////////////////////////////////////////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////// 

//기타 클래스에서 B3YenaDevice  를 인식하도록 미리 선언함.★ 
class B3YenaDevice9;
typedef B3YenaDevice9  B3YENADEVICE9;		//DX9 대응.
typedef B3YenaDevice9* LPB3YENADEVICE9;		//DX9 대응.

#define YENA_VERSION  9		//DX9.0


/////////////////////////////////////////////////////////////////////////////// 
//
// B3Yena SW Renderer : 각종 렌더링 '디바이스 Device' 들을 관리할 최상위 클래스
//					  : Direct3D 대응 ★
//                    : DX 메소드와 (거의)동일한 시그니쳐(Signature) 를 
//                      구현하는 것이 목표입니다.
//
/////////////////////////////////////////////////////////////////////////////// 

#pragma region C++ friend 설명
/*
friend 는 접근 지정자를 완벽히 무시할 수 있는 예외적 기능
private 건 protected 건 friend로 선언된 대상에게는 완벽하게 public으로 작용.
객체 지향 개념의 정보 은닉 개념에 정면으로 위배됨
friend class : 특정 클래스 입장에서 다른 클래스를 친구로 선언하는것 public처럼 인식
friend 멤버 함수 : 클래스 전체가 아닌 클래스의 특정 멤버 함수만을 프렌드로 선언
friend 전역 함수 : private , proteced 접근 지정자를 무시하고 클래스 내부의 멤버에 접근가능
*/
#pragma endregion


class B3Yena							//★
{
	friend B3Yena* B3YenaCreate9(DWORD ver);
	//friend class B3YenaDevice9;			 


protected:
	B3Yena(void);			//객체생성 방지용, 생성자 숨김. 
public:
	virtual ~B3Yena(void);


	// 외부 노출 메소드들 : DX 메소드와 (거의)동일한 시그니쳐(Signature) 를 
	//                        구현하는 것이 목표입니다. 
	//
	int CreateDevice(HWND hwnd,						//[in]디바이스의 렌더링 목표 윈도우 핸들.
		YENAPRESENT_PARAMETERS* pp,	//[in]디바이스 화면 구성 정보.	
		DWORD vp,						//[in]정점연산 방법 결정 (현재는 SW 만 가능)
		LPB3YENADEVICE9* pDev			//[out]성공시 리턴받을 디바이스 개체 포인터.
	);

};
typedef B3Yena* LPB3YENA;


// B3Yena9 개체 생성 함수
B3Yena* B3YenaCreate9(DWORD ver);


/////////////////////////////////////////////////////////////////////////////// 
//
// B3YenaDevice9 : DX9 전용 렌더링 "디바이스" 클래스, 
//                 IDirect3DDevice9 대응 ★
//                 
//                DX 또는 GL 의 지원여부 및 세부버전을 선택(DX8, DX9, DX10, DX11 등..) 하거나
//                해상도, 렌더타겟 개수 또는 HWVP 또는 SWVP 을 결정하는 등의 렌더링 관련 정보를 
//                처리하는 실제 작업용 클래스
//
//               : DX 메소드와 (거의)동일한 시그니쳐(Signature) 를 
//                구현하는 것이 목표입니다.
//
/////////////////////////////////////////////////////////////////////////////// 

class B3YenaDevice9				//★
{
	friend class B3Yena;

protected:

	// 출력 화면 관련 정보.
	HWND		 m_hWnd;
	YENAPRESENT_PARAMETERS m_PresentParam;
	DWORD		 m_VertexProcessing;
	//YENADISPLAYMODE  m_Display; 


	// 렌더타겟(Back-Buffer) 구성용 핸들.
	//
	HBITMAP  m_hBmpRT;				//★
	HDC		 m_hSurfaceRT;			//★
	COLORREF m_BkColor;




protected:

	// 내부 메소드들. '밑줄 _ '을 접두어로 표시합니다.
	//
	int  _RenderTargetCreate();
	void _RenderTargetRelease();


protected:
	B3YenaDevice9(void);			//객체생성 방지용, 생성자 숨김. 
public:
	virtual ~B3YenaDevice9(void);

	// 멤버데이터 접근자 Accessors
	HDC		 GetRT();
	COLORREF GetBkColor();

	// 외부 노출 메소드들 : DX 메소드와 (거의)동일한 시그니쳐(Signature) 를 
	//                        구현하는 것이 목표입니다. 
	// 
	int BeginScene();
	int EndScene();
	int Clear(COLORREF col);
	int Present();


	//int SetRenderState(DWORD, DWORD); 
	//int SetTransform(DWORD, MATRIX*);
	//int DrawPrimitive(); 
};


/////////////////////////////////////////////////////////////////////////////// 
//
// 기타 디바이스 대응 클래스의 확장 선언 예시는 아래와 같습니다.
// 확장성을 고려한 클래스 설계베이스 입니다만, 이번 프로젝트에서는 
// 제작/사용하지 않습니다.  "이런식으로 쓸 수 있겠군.." 정도로 보아 주십시요
//

/////////////////////////////////////////////////////////////////////////////// 
//
// B3YenaDevice10 : DX10 대응 렌더링 "디바이스" 클래스 
//                : ID3D10Device 대응 ★
//
/////////////////////////////////////////////////////////////////////////////// 

class B3YenaDevice10
{
protected:

	//...DX10 대응 Device ...
};

/////////////////////////////////////////////////////////////////////////////// 
//
// B3YenaDevice11 : DX11 대응 렌더링 "디바이스" 클래스 
//                : ID3D11Device 대응 ★
//
//
/////////////////////////////////////////////////////////////////////////////// 


class B3YenaDevice11
{
protected:

	//...DX11 대응 Device ...
};

/////////////////////////////////////////////////////////////////////////////// 
//
// B3YenaDeviceGL : OpenGL 대응 렌더링 "디바이스" 클래스 
//
//
/////////////////////////////////////////////////////////////////////////////// 


class B3YenaDeviceGL
{
protected:

	//...OpenGL 대응 Device ...
};





/**************** end of "B3Yena.h" ***********************************/
