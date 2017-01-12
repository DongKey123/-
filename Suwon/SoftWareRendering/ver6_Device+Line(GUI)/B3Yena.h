//
// B3Yena.h : DX 기본 헤더 파일.

#pragma once

#include "Define.h"
#include "B3Math.h"

/////////////////////////////////////////////////////////////////////////////// 
//
// 렌더타겟 (백버퍼) - 정보 설정 구조체 : D3DPRESENT_PARAMETERS 대응
//
struct YENAPRESENT_PARAMETERS {

	DWORD Width;			//해상도 : 클라이언트 영역기준.
	DWORD Height;
	DWORD BackBuffercnt;	//백버퍼 개수
	BOOL  Windowed;			//창모드 실행?

};

/////////////////////////////////////////////////////////////////////////////// 
//
//  해상도 정보 구조체. : D3DDISPLAYMODE 대응
//
struct YENADISPLAYMODE {
	DWORD Width, Height;
};

/////////////////////////////////////////////////////////////////////////////// 
//
// 정점 처리 - 가속화 정의.  : D3DCREATE 대응
//
enum BEHAVIOR_FLAG
{
	YENACREATE_SOFTWARE_VERTEXPROCESSING,	//CPU 가 정점 연산을 처리. 
	YENACREATE_HARDWARE_VERTEXPROCESSING,	//GPU 가 정점 연산을 처리.
	//YENACREATE_MIXED_VERTEXPROCESSING		//CPU / GPU 혼성 연산 가능.
	//YENACREATE_MULTITHREADED				//다중쓰레드 환경 호환성 추가.
	//...
};

// 메모리 관리 영역 옵션.★
enum B3YENAPOOL {
	B3YPOOL_DEFAULT = 1,	//기본값 : 비디오 메모리 사용. 
	B3YPOOL_MANAGED,		//자동관리 : 비디오 - 시스템 혼용.
	B3YPOOL_SYSTEM,			//시스템메모리 사용. (현재는 이 옵션만 유효) ★

};

// 기하도형 타입.★
//
enum B3YPRIMITIVETYPE
{
	B3YPT_POINTLIST = 1,
	B3YPT_LINELIST,
	B3YPT_LINESTRIP,
	B3YPT_TRIANGLELIST,
	B3YPT_TRIANGLESTRIP,
	B3YPT_TRIANGLEFAN,

};

/////////////////////////////////////////////////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////// 

// 정점 규격 정의 (다수의 값을 조합하여 정의/사용합니다) : D3DFVF 대응 ★
// 
#define  B3YFVF_XY			 0x0001		//2D 좌표.(DX 에는 존재하지 않음.Yena 전용)★
//#define  B3YFVF_XYZ		 0x0002		//3D 좌표.(미 변환)(Local)
//#define  B3YFVF_XYZRHW	 0x0004		//3D 좌표.(변환 완료) XY + Depth
//#define  B3YFVF_NORMAL	 0x0010		//노멀. 
#define  B3YFVF_DIFFUSE		 0x0040		//확산색(Diffuse) 색상
//#define  B3YFVF_SPECULAR	 0x0080		//정반사(Specular) 색상. 
//#define  B3YFVF_TEX1		 0x0100		//텍스처 좌표.1 
//#define  B3YFVF_TEX2		 0x0200		//텍스처 좌표.2 



#define CHECK( value, bit) (((value) & (bit)) == bit)



//기타 클래스에서 B3YenaDevice  를 인식하도록 미리 선언함.
class B3YenaDevice9;
typedef B3YenaDevice9  B3YENADEVICE9;		//DX9 대응.
typedef B3YenaDevice9* LPB3YENADEVICE9;		//DX9 대응.

#define YENA_VERSION  9		//DX9.0


/////////////////////////////////////////////////////////////////////////////// 
//
// B3YenaVertexBuffer : 정점 버퍼.관리 클래스 : IDirect3DVertexBuffer9 대응 ★ 
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

class B3YenaVertexBuffer9		//★
{
	friend class B3YenaDevice9;
protected:
	void* m_pVBuffer;		//정점버퍼 : 실제 데이터가 저장됨.

	DWORD m_SizeInByte;		//정점버퍼 크기 (바이트)
	DWORD m_FVF;			//정점버퍼 규격 조합 플래그.
	UINT  m_Stride;			//정점버퍼 안의 1마디(정점 구조 하나) 의 크기.


protected:
	B3YenaVertexBuffer9(void);			//객체생성 방지용, 생성자 숨김. 

	void* _GetVBuffer() { return m_pVBuffer; }
public:
	virtual ~B3YenaVertexBuffer9(void);


	//정점버퍼 생성 : (DX) 주어진 옵션에 맞추어 VRAM  을 확보합니다.
	//
	int Create(UINT Length,		//'정점 버퍼'의 전체 크기 (바이트)	
		DWORD FVF,				//'정점' 규격.  
		B3YENAPOOL Pool			//메모리 사용 옵션.  
	);		//★

	//정점버퍼 주소 얻기 : 실제 DX 의 그것과는 역활이 다르지만, 
	//동작의 결과는 동일합니다.★
	int Lock(UINT OffsetToLock,		//[in]접근할 원본 버퍼의 옵셋 주소. 기본값 0. 
		void** ppbData				//[out]리턴받을 포인터.
	);

	//정점 버퍼 닫기. (특별히 하는 일은 없음)
	void Unlock() { }



};

typedef B3YenaVertexBuffer9* LPB3YENAVERTEXBUFFER9;


/////////////////////////////////////////////////////////////////////////////// 
//
// B3Yena SW Renderer : 각종 렌더링 '디바이스 Device' 들을 관리할 최상위 클래스
//					  : Direct3D 대응
//                    : DX 메소드와 (거의)동일한 시그니쳐(Signature) 를 
//                      구현하는 것이 목표입니다.
//
/////////////////////////////////////////////////////////////////////////////// 




class B3Yena							
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
	int CreateDevice(HWND hwnd,			//[in]디바이스의 렌더링 목표 윈도우 핸들.
		YENAPRESENT_PARAMETERS* pp,		//[in]디바이스 화면 구성 정보.	
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
//                 IDirect3DDevice9 대응 
//                 
//                DX 또는 GL 의 지원여부 및 세부버전을 선택(DX8, DX9, DX10, DX11 등..) 하거나
//                해상도, 렌더타겟 개수 또는 HWVP 또는 SWVP 을 결정하는 등의 렌더링 관련 정보를 
//                처리하는 실제 작업용 클래스
//
//               : DX 메소드와 (거의)동일한 시그니쳐(Signature) 를 
//                구현하는 것이 목표입니다.
//
/////////////////////////////////////////////////////////////////////////////// 

class B3YenaDevice9				
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
	HBITMAP  m_hBmpRT;				
	HDC		 m_hSurfaceRT;			
	COLORREF m_BkColor;

	//정점 버퍼 (이번에 렌더링할 ..) ★
	//DrawPrimitive 에 사용.
	LPB3YENAVERTEXBUFFER9 m_pVB;		// 렌더링용 등록된 정점 버퍼.
	DWORD m_FVF;						// 렌더링용 등록된 정점 규격
	UINT  m_Stride;						// 렌더링용 등록된 정점 1마디의 크기

	//그리기 옵션.(DrawPrimitive 호출시 입력됨)★
	UINT m_PrimCnt;		//이번에 그려질 Face 개수.
	UINT m_StartVtx;	//이번에 그려질 (시작) 정점 번호.

protected:

	// 내부 메소드들. '밑줄 _ '을 접두어로 표시합니다.
	//
	int  _RenderTargetCreate();
	void _RenderTargetRelease();

	// 렌더링 파이프 라인별 연산 메소드.★
	//
	int _VertexPipeLine();		//정점 파이프 라인.
	int _GeometryPipeLine();	//기하 파이프 라인.	
	int _PixelPipeLine();		//픽셀 파이프 라인.	

	//그리기 함수.★
	int _DrawLine(B3YVECTOR2& start, B3YVECTOR2& end);

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

	// 렌더링 관련 메소드들.
	//

	// 정점 버퍼 생성.★
	int CreateVertexBuffer(
		UINT Length,					//[in]'정점 버퍼'의 전체 크기 (바이트)	
		DWORD FVF,						//[in]'정점' 규격.
		B3YENAPOOL Pool,				//[in]시스템메모리 사용. (현재는 이 옵션만 유효) 
		LPB3YENAVERTEXBUFFER9* ppVB		//[out] 성공시 리턴되는 버퍼 포인터ㅣ
	);

	// 디바이스에 정점 버퍼 등록.★
	int SetVertexBuffer(
		LPB3YENAVERTEXBUFFER9 pVB,			//렌더링할 버퍼 포인터. 
		UINT Stride							//렌더링할 버퍼의 1마디(정점데이터) 크기
	);

	// 정점 버퍼의 규격 등록.★
	int SetFVF(DWORD FVF);									//정점 규격 : '이번에 렌더링할 정점버퍼의 그것과 동일해야 함.

															// 렌더링..(이전에 등록된 정점 버퍼를 사용)
	int DrawPrimitive(B3YPRIMITIVETYPE PrimitiveType,		//렌더링 기하 타입.
		UINT StartVertex,					//렌더링할 정점버퍼의 시작 정점 인덱스.(정점버퍼 안)
		UINT PrimitiveCount					//렌더링할 기하도형 개수.
	);

	// 렌더링 상태 변화 조절.★
	int SetRenderState(DWORD state, DWORD value);
	//int SetTransform(DWORD, MATRIX*);
};
//typedef B3YenaDevice9* LPB3YENADEVICE9;		//DX9 대응.

#define SetStreamSource  SetVertexBuffer

/////////////////////////////////////////////////////////////////////////////// 
//
// 기타 디바이스 대응 클래스의 확장 선언 예시는 아래와 같습니다.
// 확장성을 고려한 클래스 설계베이스 입니다만, 이번 프로젝트에서는 
// 제작/사용하지 않습니다.  "이런식으로 쓸 수 있겠군.." 정도로 보아 주십시요
//

/////////////////////////////////////////////////////////////////////////////// 
//
// B3YenaDevice10 : DX10 대응 렌더링 "디바이스" 클래스 
//                : ID3D10Device 대응 
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
//                : ID3D11Device 대응 
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
