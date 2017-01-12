#include "B3Yena.h"

/////////////////////////////////////////////////////////////////////////////// 
//
// B3YenaVertexBuffer : 정점 버퍼.(DX9 대응)★ 
//
//
/////////////////////////////////////////////////////////////////////////////// 

B3YenaVertexBuffer9::B3YenaVertexBuffer9(void)
{
	m_pVBuffer = NULL;

	m_SizeInByte = 0;
	m_FVF = 0;
	m_Stride = 0;
}

B3YenaVertexBuffer9::~B3YenaVertexBuffer9(void)
{
	SAFE_DELARRY(m_pVBuffer);
}

///////////////////////////////////////////////////////////////////////////////
//
// 사용자가 지정한 옵션으로 정점버퍼를 생성.
//
int B3YenaVertexBuffer9::Create(UINT Length,		//'정점 버퍼'의 전체 크기 (바이트)	
								DWORD FVF,			//'정점' 규격.★ 
								B3YENAPOOL Pool		//시스템메모리 사용. (현재는 이 옵션만 유효) ★ 
								)		//★
{
	//이미 만들어져 있다면 오류..사용자에게 해결을 요구. 
	//에러 코드를 추가하는 것을 추천합니다. ex) YN_ERR_NOTEMPTY
	if (YN_VALIED(m_pVBuffer)) return YN_FAIL;

	//PooL 옵션은 무시합니다. : 단지 DX 의 그것처럼 표현하기 위한 용도 입니다.
	//...

	m_pVBuffer = static_cast<void*>(new BYTE[Length]);
	m_SizeInByte = Length;
	m_FVF = FVF;

	//정점1개의 크기(바이트) 계산하기.
	//지정된 정점규격(FVF) 기준으로 계산을 수행.
	m_Stride = 0;
	if (CHECK(m_FVF, B3YFVF_XY))	 m_Stride += sizeof(float) * 2;		//2d 좌표.
	//if(CHECK(m_FVF, B3YFVF_DIFFUSE))  m_Stride += sizeof(DWORD);		//색상

																		//오류 체크..
	if (m_Stride <= 0) {  /* Error!!...  */  return YN_FAIL; }

	return YN_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
// 정점 버퍼에 '데이터를 입력하기 위해 해당 주소를 구합니다.
//
// 전체 버퍼중, 일부분의 주소를 얻는 것도 가능. 
// 실제 DX 의 그것과는 역활이 다르지만, 동작의 결과는 동일합니다.★
//
int B3YenaVertexBuffer9::Lock( UINT OffsetToLock,		//[in]접근할 원본 버퍼의 옵셋 주소. 기본값 0. 
							  void ** ppbData			//[out]리턴받을 포인터.
							  )
{	
	if(YN_INVALIED(*ppbData)) return YN_FAIL;			//유효성 검사.

	BYTE* pVB = (BYTE*)m_pVBuffer + OffsetToLock;		//지정 주소 계산.

	*ppbData = (void*)pVB;								//주소 넘김. 

	return YN_OK;
}

/////////////////////////////////////////////////////////////////////////////// 
//
// B3Yena 개체 생성 최상위 호출 함수
//
/////////////////////////////////////////////////////////////////////////////// 
//
B3Yena* B3YenaCreate9(DWORD ver)
{
	B3Yena* pYena = new B3Yena;

	//실패시 예외처리 필요...
	//ASSERT(pYena);

	return pYena;
}

/////////////////////////////////////////////////////////////////////////////// 
//
// B3Yena SW Renderer : 각종 렌더링 '디바이스 Device' 들을 관리할 최상위 클래스
//
/////////////////////////////////////////////////////////////////////////////// 



B3Yena::B3Yena(void)
{
}

B3Yena::~B3Yena(void)
{
}


/////////////////////////////////////////////////////////////////////////////// 
//
// 지정 렌더링 디바이스 개체 생성 메소드.
//
int B3Yena::CreateDevice(HWND hwnd,
	YENAPRESENT_PARAMETERS* pp,
	DWORD vp,
	LPB3YENADEVICE9* ppDevice
)
{
	//입력 정보 확인 : 오류별로 리턴값을 다르게 하는 것을 추천.
	if (hwnd == NULL) return YN_FAIL;
	if (pp == NULL) return YN_FAIL;
	if (ppDevice == NULL) return YN_FAIL;

	//디바이스 개체 생성. : 단순하게 DX9 만 대응합니다.★
	LPB3YENADEVICE9 pDev = new B3YenaDevice9;

	//디바이스 초기화..
	pDev->m_hWnd = hwnd;
	pDev->m_PresentParam = *pp;
	pDev->m_VertexProcessing = vp;
	pDev->_RenderTargetCreate();			//렌더타겟(백버퍼) 생성 : 현재는 1개만 만들어 집니다.

											//외부에 리턴..
	*ppDevice = pDev;

	return YN_OK;
}


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
//
// B3YenaDevice9 : DX9 전용 렌더링 "디바이스" 클래스 
//
//                DX 또는 GL 의 지원여부 및 세부버전을 선택(DX8, DX9, DX10, DX11 등..) 하거나
//                해상도, 렌더타겟 개수 또는 HWVP 또는 SWVP 을 결정하는 등의 렌더링 관련 정보를 
//                처리하는 실제 작업용 클래스
//
/////////////////////////////////////////////////////////////////////////////// 

B3YenaDevice9::B3YenaDevice9(void)
{
	m_hWnd = NULL;
	ZeroMemory(&m_PresentParam, sizeof(m_PresentParam));

	m_hBmpRT = NULL;
	m_hSurfaceRT = NULL;
	m_BkColor = RGB(0, 0, 255);

	m_pVB = NULL;	//★ 
	m_FVF = 0;		//★ 
	m_Stride = 0;	//★ 

	m_PrimCnt = 0;	//★
	m_StartVtx = 0;	//★

}

B3YenaDevice9::~B3YenaDevice9(void)
{
	//Back-Buffer : Render Target 제거.★ 
	_RenderTargetRelease();

	//정점 버퍼 자체를 제거하는 것은 사용자가 책임을 집니다.★
	//...SAFE_DELETE(m_pVB);	
}

///////////////////////////////////////////////////////////////////////////////
//
HDC  B3YenaDevice9::GetRT()
{
	return m_hSurfaceRT;
}

///////////////////////////////////////////////////////////////////////////////
//
COLORREF B3YenaDevice9::GetBkColor()
{
	return m_BkColor;
}


///////////////////////////////////////////////////////////////////////////////
//
//  백버퍼용 렌더타겟을 생성.
//
int B3YenaDevice9::_RenderTargetCreate()
{
	HDC hdc = GetDC(m_hWnd);

	m_hSurfaceRT = CreateCompatibleDC(hdc);
	m_hBmpRT = (HBITMAP)CreateCompatibleBitmap(hdc, m_PresentParam.Width, m_PresentParam.Height);
	SelectObject(m_hSurfaceRT, m_hBmpRT);

	ReleaseDC(m_hWnd, hdc);

	return YN_OK;
}

///////////////////////////////////////////////////////////////////////////////
// 
// 렌더타겟 제거.
//
void B3YenaDevice9::_RenderTargetRelease()
{
	DeleteObject(m_hBmpRT);
	DeleteDC(m_hSurfaceRT);
}

///////////////////////////////////////////////////////////////////////////////
//
// 장면 그리기 시작. : 렌더링에 필요한 (디바이스의) 선위 작업을 수행.
//
int B3YenaDevice9::BeginScene()
{
	//폰트 배경색 기본값 
	//SetBkColor(m_hSurfaceRT, m_BkColor);
	SetBkMode(m_hSurfaceRT, TRANSPARENT);

	return YN_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
// 장면 그리기 종료 ; 렌더링 종료에 필요한 (디바이스의) 후위 작업을 수행.
//
int B3YenaDevice9::EndScene()
{
	//...

	return YN_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
// 렌더타겟 클리어.
//
// 인자 : COLORREF col : RT 을 지울 지정색.
// 리턴 : 
//
int B3YenaDevice9::Clear(COLORREF col)
{
	HBRUSH hBrush = CreateSolidBrush(col);
	RECT rc = { 0, 0, m_PresentParam.Width,  m_PresentParam.Height };
	FillRect(m_hSurfaceRT, &rc, hBrush);
	DeleteObject(hBrush);

	m_BkColor = col;

	return YN_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
// RT 의 내용(렌더링된 장면)을 Front Buffer 에 출력합니다. "Flipping", "Swapping"
//
// 인자 : 없음
// 리턴 : 오류시 YN_OK 이외의 값.
//
int B3YenaDevice9::Present()
{
	HDC hdc = GetDC(m_hWnd);
	BitBlt(hdc, 0, 0, m_PresentParam.Width, m_PresentParam.Height, m_hSurfaceRT, 0, 0, SRCCOPY);
	ReleaseDC(m_hWnd, hdc);

	return YN_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
// 사용자가 지정한 옵션으로 정점버퍼를 생성.
//
int B3YenaDevice9::CreateVertexBuffer(UINT Length,
							 		DWORD FVF,
									B3YENAPOOL Pool,
									LPB3YENAVERTEXBUFFER9* ppVB
									)		//★
{
	LPB3YENAVERTEXBUFFER9 pVB = new B3YenaVertexBuffer9;	//정점버퍼 개체 생성.
	if (YN_INVALIED(pVB)) return YN_FAIL;

	if (YN_FAILED(pVB->Create(Length, FVF, Pool)))			//정점버퍼 확보.
	{
		//Error...
		return YN_FAIL;
	}


	//외부로 리턴..
	*ppVB = pVB;


	return YN_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
// 사용자 생성 정점버퍼를 디바이스에 등록.
//
int B3YenaDevice9::SetVertexBuffer(
								LPB3YENAVERTEXBUFFER9 pVB,			//렌더링할 버퍼 포인터.
								UINT Stride							//렌더링할 버퍼의 1마디(정점데이터) 크기
								)		//★
{
	if (YN_INVALIED(pVB)) return YN_FAIL;

	m_pVB = pVB;
	m_Stride = Stride;

	return YN_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
// 정점 구성 규격을 전달합니다. 
// SetStreamSource 로 등록된 또는 등록할 정점버퍼와 동일한 규격을 명시해야 합니다.
//
int B3YenaDevice9::SetFVF(DWORD fvf)
{
	m_FVF = fvf;

	return YN_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
// 기하 데이터 렌더링 
//
// 이 메소드의 호출 이전에 정점버퍼, FVF, 마디 크기(Stride) 등 렌더링에 필요한
// 모든 정보가 디바이스에 등록되어 있어야 합니다.
// 
int B3YenaDevice9::DrawPrimitive(
								B3YPRIMITIVETYPE PrimitiveType,	//렌더링 기하 타입.
								UINT StartVertex,				//렌더링할 정점 시작번호 (정점버퍼 안)
								UINT PrimitiveCount				//렌더링할 기하 개수.
								)		//★
{

	if (YN_INVALIED(m_pVB)) return YN_FAIL;
	if (YN_INVALIED(m_Stride)) return YN_FAIL;

	//렌더링 기초 옵션 저장.
	m_PrimCnt = PrimitiveCount;
	m_StartVtx = StartVertex;

	//
	// 각 렌더링 파이프라인별로 적절한 연산을 수행합니다.
	//

	//1.정점 파이프 라인...
	_VertexPipeLine();

	//2.기하 파이프 라인...
	_GeometryPipeLine();

	//3.픽셀 파이프 라인...
	_PixelPipeLine();

	//4.최종 출력...


	return YN_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
// 정점 파이프 라인 연산을 수행 : 출력결과는 GP 으로 넘어감니다.
//
int B3YenaDevice9::_VertexPipeLine()		//★
{

	//이번 데모는 이미 2D 좌표를 가지고 있으므로
	//추가 변환이 필요없고, 바로 화면에 출력합니다.
	//...


	return YN_OK;
}




///////////////////////////////////////////////////////////////////////////////
//
// 기하 파이프 라인 연산을 수행 : 출력결과는 PP 으로 넘어감니다.
//
int B3YenaDevice9::_GeometryPipeLine()		//★
{

	//이번 데모는 이미 2D 좌표를 가지고 있으므로
	//추가 변환이 필요없고, 바로 화면에 출력합니다.
	//...

	return YN_OK;
}

int B3YenaDevice9::_PixelPipeLine()		//★
{

	//이번 데모는 이미 2D 좌표를 가지고 있으므로
	//추가 변환이 필요없고, 바로 화면에 출력합니다.
	//
	UINT vtxCnt = m_pVB->m_SizeInByte / m_Stride;		//정점 개수 확인.

	void* pOrgVB = m_pVB->_GetVBuffer();	//원본 버퍼.

	BYTE* pCurrVB = (BYTE*)pOrgVB;			//현재 처리중인 정점버퍼 주소. 
	BYTE* pCurrVtx = NULL;					//현재 처리중인 정점의 주소.
	UINT  faceCnt = 0;

	// 현제 데모는 Triangle-List 만 처리합니다.★
	// Triangle-List 은 Face 마다 3개의 정점을 사용합니다.  
	//
	//for(int i=0; i<vtxCnt; i+=3)				//3개씩 처리..(전부 다 그리기..)
	for (UINT i = m_StartVtx; i<vtxCnt; i += 3)		//지정 정점부터, 그리기.★
	{
		//현재 기하데이터(삼각형) 포인터 얻기...
		pCurrVB = (BYTE*)pOrgVB + (i)*m_Stride;

		//첫번째 정점 
		pCurrVtx = pCurrVB;
		B3YVECTOR2  v0 = *(B3YVECTOR2*)pCurrVtx;		//좌표 얻기.


														//2번째 정점..
		pCurrVtx = pCurrVB + m_Stride;					//1개 크기만큼 이동. 
		B3YVECTOR2  v1 = *(B3YVECTOR2*)pCurrVtx;		//좌표 얻기.


														//3번째 정점..
		pCurrVtx = pCurrVB + (2)*m_Stride;				//1개 크기만큼 이동. 
		B3YVECTOR2  v2 = *(B3YVECTOR2*)pCurrVtx;		//좌표 얻기.


														//라인 3번 그리기.
														//.. 	
														//v0->v1
		_DrawLine(v0, v1);

		//v0->v2
		_DrawLine(v0, v2);

		//v1->v2
		_DrawLine(v1, v2);


		//지정 개수 이상의 삼각형이 그려지면, 작업 종료...★ 
		if (++faceCnt >= m_PrimCnt)
			break;
	}



	return YN_OK;
}

/*
///////////////////////////////////////////////////////////////////////////////
//
// 라인 그리기. (ver.User)
//
int B3YenaDevice9::_DrawLine(B3YVECTOR2& v0, B3YVECTOR2& v1)
{

//메소드 구현...(과제) ㅋㅋ

return YN_OK;
}
*/



///////////////////////////////////////////////////////////////////////////////
//
// 라인 그리기. (ver.GDI)
//
int B3YenaDevice9::_DrawLine(B3YVECTOR2& v0, B3YVECTOR2& v1)
{
	MoveToEx(m_hSurfaceRT, (int)v0.x, (int)v0.y, NULL);
	LineTo(m_hSurfaceRT, (int)v1.x, (int)v1.y);

	return YN_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
// 렌더링 상태 옵션 변경 : 
//
int B3YenaDevice9::SetRenderState(DWORD state, DWORD value)		//★
{
	//...
	return YN_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
//




/**************** end of "B3Yena.h" ***********************************/
