#include "B3Yena.h"

/////////////////////////////////////////////////////////////////////////////// 
//
// B3YenaVertexBuffer : 정점 버퍼.(DX9 대응)
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
	DWORD FVF,			//'정점' 규격.
	B3YENAPOOL Pool		//시스템메모리 사용. (현재는 이 옵션만 유효) 
)
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
// 실제 DX 의 그것과는 역활이 다르지만, 동작의 결과는 동일합니다.
//
int B3YenaVertexBuffer9::Lock(UINT OffsetToLock,		//[in]접근할 원본 버퍼의 옵셋 주소. 기본값 0. 
	void ** ppbData			//[out]리턴받을 포인터.
)
{
	if (YN_INVALIED(*ppbData)) return YN_FAIL;			//유효성 검사.

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

	//디바이스 개체 생성. : 단순하게 DX9 만 대응합니다
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

	m_pVB = NULL;
	m_FVF = 0;
	m_Stride = 0;

	m_PrimCnt = 0;
	m_StartVtx = 0;

	//렌더상태 초기화.
	_InitRenderState();		
}

B3YenaDevice9::~B3YenaDevice9(void)
{
	//Back-Buffer : Render Target 제거.
	_RenderTargetRelease();

	//정점 버퍼 자체를 제거하는 것은 사용자가 책임을 집니다.
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
)
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
)
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
)
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
int B3YenaDevice9::_VertexPipeLine()
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
int B3YenaDevice9::_GeometryPipeLine()
{

	//이번 데모는 이미 2D 좌표를 가지고 있으므로
	//추가 변환이 필요없고, 바로 화면에 출력합니다.
	//...

	return YN_OK;
}

int B3YenaDevice9::_PixelPipeLine()
{

	//이번 데모는 이미 2D 좌표를 가지고 있으므로
	//추가 변환이 필요없고, 바로 화면에 출력합니다.
	//
	UINT vtxCnt = m_pVB->m_SizeInByte / m_Stride;		//정점 개수 확인.

	void* pOrgVB = m_pVB->_GetVBuffer();	//원본 버퍼.

	BYTE* pCurrVB = (BYTE*)pOrgVB;			//현재 처리중인 정점버퍼 주소. 
	BYTE* pCurrVtx = NULL;					//현재 처리중인 정점의 주소.
	UINT  faceCnt = 0;

	m_FaceNum = -1;

	// 현제 데모는 Triangle-List 만 처리합니다
	// Triangle-List 은 Face 마다 3개의 정점을 사용합니다.  
	//
	//for(int i=0; i<vtxCnt; i+=3)				//3개씩 처리..(전부 다 그리기..)
	for (UINT i = m_StartVtx; i < vtxCnt; i += 3)		//지정 정점부터, 그리기
	{
		//m_VtxNum = i;
		m_FaceNum++;


		//현재 기하데이터(삼각형) 포인터 얻기...
		pCurrVB = (BYTE*)pOrgVB + (i)*m_Stride;

		//--------------------------------------------------------------- 
		// 외부에서 설정된 정점버퍼에서 정점 데이터를 구합니다.
		//
		// 정확한 데이터 획득을 위해서는  FVF 검사가 필요하지만 
		// 간결한 진행을 위해, 이를 생략합니다. 
		//---------------------------------------------------------------- 

		//첫번째 정점 
		pCurrVtx = pCurrVB;
		B3YVECTOR2  v0 = *(B3YVECTOR2*)pCurrVtx;		//좌표 얻기.


		//2번째 정점..
		pCurrVtx = pCurrVB + m_Stride;					//1개 크기만큼 이동. 
		B3YVECTOR2  v1 = *(B3YVECTOR2*)pCurrVtx;		//좌표 얻기.


		//3번째 정점..
		pCurrVtx = pCurrVB + (2)*m_Stride;				//1개 크기만큼 이동. 
		B3YVECTOR2  v2 = *(B3YVECTOR2*)pCurrVtx;		//좌표 얻기.


		//--------------------------------------------------------------- 
		// n번째 삼각형의 정보를 모두 구했습니다.
		//
		// 삼각형의 가시성(Visualization) 을 판정을 통해 보이지 않는
		// 삼각형들을 렌더링 파이프라인에서 제외시키면 전반적인 
		// 렌더링 속도 향상을 이끌어 낼 수 있습니다.
		// 
		// 이러한 동작은 대부분 Geometry Pipeline 에서 처리되지만
		// 일단 이 곳에서 처리해 봅시다.
		//---------------------------------------------------------------  
		if (_FaceCulling(v0, v1, v2))
		{
			//true 가 리턴되면, 컬링해야 하는 삼각형으로 판정, 
			//다음 삼각형으로 넘어간다.

			//지정 개수 이상의 삼각형이 그려지면, 작업 종료... 			
			if (++faceCnt >= m_PrimCnt)
				break;
			else
				continue;
		}
					
		//--------------------------------------------------------------- 
		// n번째 삼각형의 정보를 모두 구했으니,  
		// 삼각형을 그려봅시다.  
		//--------------------------------------------------------------- 

		//기하 도형 그리기 : "Fill Mode" 상태에 따른 그리기. 일단 이곳에서 처리.
		if (m_RState[B3YRS_FILLMODE] == B3YFILL_WIREFRAME)	
		{
			//라인 그리기. 
			_DrawLine(v0, v1);
			_DrawLine(v0, v2);
			_DrawLine(v1, v2);
		}
		else
		{
			//삼각형 그리기 (단색) 
			_DrawFace(v0, v1, v2);
		}


		//지정 개수 이상의 삼각형이 그려지면, 작업 종료... 
		if (++faceCnt >= m_PrimCnt)
			break;
	}



	return YN_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
// 라인 그리기. (ver.User)
//
int B3YenaDevice9::_DrawLine(B3YVECTOR2& v0, B3YVECTOR2& v1)
{

	int X = v1.x - v0.x;  //x값의 범위 확인.
	int Y = v1.y - v0.y;  //y값의 범위 확인.
	/////////////////////////////////////
	////abs() -> 정수의 절대값 리턴.
	/////////////////////////////////////
	//					  
	////축 계산 처리.////////////////////
	int steps = max(abs(X), abs(Y));
	/////////////////////////////////////
	//					  
	float x_incre = (float)X / (float)steps;
	float y_incre = (float)Y / (float)steps;

	float x = (float)v0.x;
	float y = (float)v0.y;

	for (int k = 0; k <= steps; k++)
	{
		x = x + x_incre;
		y = y + y_incre;
		SetPixel(m_hSurfaceRT, (int)x, (int)y, RGB(0, 0, 0));
	}
	/*MoveToEx(m_hSurfaceRT, v0.x, v0.y, NULL);
	LineTo(m_hSurfaceRT, v1.x, v1.y);*/

	return YN_OK;
}




///////////////////////////////////////////////////////////////////////////////
//
// 라인 그리기. (ver.GDI)
//
//int B3YenaDevice9::_DrawLine(B3YVECTOR2& v0, B3YVECTOR2& v1)
//{
//	MoveToEx(m_hSurfaceRT, (int)v0.x, (int)v0.y, NULL);
//	LineTo(m_hSurfaceRT, (int)v1.x, (int)v1.y);
//
//	return YN_OK;
//}



///////////////////////////////////////////////////////////////////////////////
//
// 라인 그리기. (ver.GDI)
//
//int B3YenaDevice9::_DrawLine(B3YVECTOR2& v0, B3YVECTOR2& v1)
//{
//	MoveToEx(m_hSurfaceRT, (int)v0.x, (int)v0.y, NULL);
//	LineTo(m_hSurfaceRT, (int)v1.x, (int)v1.y);
//
//	return YN_OK;
//}


///////////////////////////////////////////////////////////////////////////////
//
// 삼각형 그리기. 
//
int B3YenaDevice9::_DrawFace(B3YVECTOR2& v0, B3YVECTOR2& v1, B3YVECTOR2& v2)
{

	B3YVECTOR2 ver0, ver1, ver2;
	_SortVert3(ver0, ver1, ver2, v0, v1, v2);
	
	if (ver0.y == ver1.y || ver1.y == ver2.y || ver0.y == ver2.y) // v1.y 가 v0.y 와 v2.y가 같을때
	{
		if (ver1.y == ver2.y)
		{
			//윗삼각형
			if (ver1.x < ver2.x)
			{
				B3YVECTOR2 temp;
				temp = ver1;
				ver1 = ver2;
				ver2 = temp;
			}
			float left_incre = (float)(ver0.x - ver2.x) / (float)(ver0.y - ver2.y);
			float right_incre = (float)(ver0.x - ver1.x) / (float)(ver0.y - ver1.y);
			float startX = ver0.x;
			float endX = ver0.x;
			for (int spY = ver0.y; spY <= (int)ver1.y; ++spY)
			{
				startX += left_incre;
				endX += right_incre;
				int spX = (int)startX;
				int epX = (int)endX;
				for (; spX <= epX; spX++)
				{
					SetPixel(m_hSurfaceRT, spX, spY, RGB(0, 0, 0));
				}
			}
		}
		if (ver1.y == ver0.y)
		{
			//아랫 삼각형
			if (ver1.x < ver0.x)
			{
				B3YVECTOR2 temp;
				temp = ver1;
				ver1 = ver0;
				ver0 = temp;
			}
			float left_incre = (float)(ver0.x - ver2.x) / (float)(ver0.y - ver2.y);
			float right_incre = (float)(ver1.x - ver2.x) / (float)(ver1.y - ver2.y);
			float startX = ver0.x;
			float endX = ver1.x;
			for (int spY = ver1.y; spY <= (int)ver2.y; ++spY)
			{
				startX += left_incre;
				endX += right_incre;
				int spX = (int)startX;
				int epX = (int)endX;
				for (; spX <= epX; spX++)
				{
					SetPixel(m_hSurfaceRT, spX, spY, RGB(0, 0, 0));
				}
			}
		}
	}
	else if (IsCW(ver0.x, ver0.y, ver1.x, ver1.y, ver2.x, ver2.y) == 1)
	{


		//윗삼각형
		float left_incre = (float)(ver0.x - ver2.x) / (float)(ver0.y - ver2.y);
		float right_incre = (float)(ver0.x - ver1.x) / (float)(ver0.y - ver1.y);
		float startX = ver0.x;
		float endX = ver0.x;
		for (int spY = ver0.y; spY <= (int)ver1.y; ++spY)
		{
			startX += left_incre;
			endX += right_incre;
			int spX = (int)startX;
			int epX = (int)endX;
			for (; spX <= epX; spX++)
			{
				SetPixel(m_hSurfaceRT, spX, spY, RGB(0, 0, 0));
			}
		}
		//아랫 삼각형
		right_incre = (float)(ver2.x - ver1.x) / (float)(ver2.y - ver1.y);
		for (int spY = ver1.y; spY <= (int)ver2.y; ++spY)
		{
			startX += left_incre;
			endX += right_incre;
			int spX = (int)startX;
			int epX = (int)endX;
			for (; spX <= epX; spX++)
			{
				SetPixel(m_hSurfaceRT, spX, spY, RGB(0, 0, 0));
			}
		}
	}
	else if (IsCW(ver0.x, ver0.y, ver1.x, ver1.y, ver2.x, ver2.y) == -1)
	{
		//윗삼각형
		float left_incre = (float)(ver0.x - ver1.x) / (float)(ver0.y - ver1.y);
		float right_incre = (float)(ver0.x - ver2.x) / (float)(ver0.y - ver2.y);
		float startX = ver0.x;
		float endX = ver0.x;
		for (int spY = ver0.y; spY <= (int)ver1.y; ++spY)
		{
			startX += left_incre;
			endX += right_incre;
			int spX = (int)startX;
			int epX = (int)endX;
			for (; spX <= epX; spX++)
			{
				SetPixel(m_hSurfaceRT, spX, spY, RGB(0, 0, 0));
			}
		}
		//아랫 삼각형
		left_incre = (float)(ver2.x - ver1.x) / (float)(ver2.y - ver1.y);
		for (int spY = ver1.y; spY <= (int)ver2.y; ++spY)
		{
			startX += left_incre;
			endX += right_incre;
			int spX = (int)startX;
			int epX = (int)endX;
			for (; spX <= epX; spX++)
			{
				SetPixel(m_hSurfaceRT, spX, spY, RGB(0, 0, 0));
			}
		}
	}


	return YN_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
// high , middle , bottom (out) v0,v1,v2 (in) y값에 따른 정렬 
// 
//
int B3YenaDevice9::_SortVert3(B3YVECTOR2& high, B3YVECTOR2& middle, B3YVECTOR2& bottom,//out
	B3YVECTOR2& v0, B3YVECTOR2& v1, B3YVECTOR2& v2) //in
{

	if (v0.y > v1.y)
	{
		if (v1.y > v2.y)
		{
			high = v2;
			middle = v1;
			bottom = v0;
		}
		else
		{
			if (v2.y > v0.y)
			{
				high = v1;
				middle = v0;
				bottom = v2;
			}
			else
			{
				high = v1;
				middle = v2;
				bottom = v0;
			}
		}
	}
	else
	{
		if (v0.y > v2.y)
		{
			high = v2;
			middle = v0;
			bottom = v1;
		}
		else
		{
			if (v2.y > v1.y)
			{
				high = v0;
				middle = v1;
				bottom = v2;
			}
			else
			{
				high = v0;
				middle = v2;
				bottom = v1;
			}
		}
	}

	return YN_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
// 렌더링 상태 옵션 변경  
//
int B3YenaDevice9::SetRenderState(B3YRENDERSTATETYPE state, DWORD value)
{
	m_RState[state] = value;

	return YN_OK;
}



///////////////////////////////////////////////////////////////////////////////
//
// 렌더링 상태 얻기   
//
void B3YenaDevice9::GetRenderState(B3YRENDERSTATETYPE state, DWORD* value)
{
	*value = m_RState[state];
}



///////////////////////////////////////////////////////////////////////////////
//
// 각 렌더링 상태별 초기화 값 설정. 
//
void B3YenaDevice9::_InitRenderState()
{
	::ZeroMemory(m_RState, sizeof(DWORD)*B3YRS_MAX_);

	// 각 렌더링 상태별 초기화 값 설정.
	//
	m_RState[B3YRS_FILLMODE] = B3YFILL_SOLID;	//단일색 채우기 
	m_RState[B3YRS_CULLMODE] = B3YCULL_CCW;		//반시계방향 컬링 ★

}

///////////////////////////////////////////////////////////////////////////////
//
// 페이스 컬링. : 2D 버전(Screen 좌표) 
//                : +Y 축이 화면아랫방향임에 주의!  3D 좌표계와 다르다!
//
// 기본 모드는 CCW. true 리턴시, 그리기 동작 없음.★
// 
bool B3YenaDevice9::_FaceCulling(B3YXVECTOR2 v0, B3YXVECTOR2 v1, B3YXVECTOR2 v2)
{
	bool bCull = false;

	B3YXVECTOR2 goV1 = v1 - v0;
	B3YXVECTOR2 goV2 = v2 - v1;

	int z = B3YXVec2CCW(&goV1, &goV2);

	switch (m_RState[B3YRS_CULLMODE])
	{
	case B3YCULL_NONE:  bCull = false;		//컬링 없음. 무조건 그리기.
		break;

	case B3YCULL_CW:		//정점 순서가 시계방향이면, 컬링. 그리지 않음.
	{
		//...과제 완성...★
		if (z > 0)
			bCull = true;  //곧, 컬링되어야 함.	  
	}
	break;

	case B3YCULL_CCW:		//정점 순서가 반시계방향이면, 컬링. 그리지 않음.
	{
		//...과제 완성...★
		if (z <= 0)
			bCull = true;  //곧, 컬링되어야 함.	  
	}
	break;
	}


	return bCull;
}



/**************** end of "B3Yena.h" ***********************************/
