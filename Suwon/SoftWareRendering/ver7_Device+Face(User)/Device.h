///////Device.h : 기본 렌더링 및 디바이스 설정 헤더 파일


#pragma once
#pragma warning(disable:4996)

#include "B3Yena.h"  // Yena 메인 헤더
					 // Project Property > C/C++ > Additional Include Directories 에 등록..  

int		YenaSetup(HWND hwnd);
void	YenaRelease();


void	CameraSetup();
float	GetEngineTime();

void	PutFPS(int x, int y);


int BeginScene();
int EndScene();
int Clear(COLORREF col);
int Present();

int DrawText(int x, int y, COLORREF col, char* msg, ...);

///////////////////////////////////////////////////////////////////////////////
//

//비동기 키보드 입력 처리.
#define IsKeyDown(k) ((GetAsyncKeyState(k) & 0x8000 ) == 0x8000)
#define IsKeyUp(k) ((GetAsyncKeyState(k) & 0x8001) == 0x8001)

#pragma region 키보드 입력 처리 설명
/*
GetAsyncKeyState() 함수와 GetKeyState() 함수는 키보드의 키가 눌렸는지를 체크하는 함수들이다.
GetAsyncKeyState() 함수는 비 동기(Asynchronism)로 처리한다.즉, 호출된 시점에서 키 상태를 조사하여,
메시지 큐를 거치지 않고 바로 리턴을 해 주므로 키 입력을 바로 처리해 줄 수가 있다.

0x8000 은 현재 키가 눌려진 상태를 뜻하고, 0x0001은 지난번 호출과 이번 호출 사이에 키가 눌려진 적이 있었다 라는 것을 뜻한다( 즉 키를 땠을때)
*/
#pragma endregion

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)  if((p)){ (p)->Release(); (p) = NULL; }
#define SAFE_DELETE(p)  if((p)){ delete (p); (p) = NULL; }
#define SAFE_DELARRY(p)  if((p)){ delete[](p); (p) = NULL; }
#endif

#pragma region 전처리문 설명
/*
파일 처리를 위한 전처리문		 : #include
형태 정의를 위한 전처리문		 : #define, #undef
조건 처리를 위한 전처리문		 : #if, #ifdef, #ifndef, #else, #elif, #endif
에러 처리를 위한 전처리문		 : #error
디버깅을 위한 전처리문			 : #line
컴파일 옵션 처리를 위한 전처리문 : #pragma

#if		: …이 참이라면
#ifdef	: …이 정의되어 있다면
#else	: #if나 #ifdef에 대응된다.
#elif	: “else + if”의 의미
#endif	: #if, #ifdef, #infdef 이 끝났음을 알린다.

#include : 헤더 파일과 같은 외부 파일을 읽어서 포함시키고자 할때(이진파일이 아닌 C의 소스파일과 같은 형태의 일반 문서파일)
< >		 : 컴파일러의 표준 포함 디렉토리에서 찾는 것
" "		 : 현재의 디렉토리를 기본으로 파일을 찾는 것

#define			: 상수값을 지정하기 위한 예약어(매크로)
#undef			: #define으로 이미 정의된 매크로를 무효화 함
#if~#endif		: if와 유사 , 어떠한 구문을 컴파일할지 안할지를 지정
#ifdef~#endif	: if + define 만약 디파인으로 정의되어 있다면 컴파일할지 알할지를 지정

#ifndef 헤더명_H__ ~ #endif : 헤더파일을 겹치는 것을 막기 위한 일종의 매크로 ( #pragma  once)
#defined : define이 여러 개 되어 있는지를 검사할 때 사용
#ifdef #if defined 의 차이 :
#ifdef는 정의가 되어 있는지를 테스트 하기 때문에, 한번에 여러 개를 사용할 수 없다.
여러 개가 정의되어 있는지를 테스트 하기 위해서는
#if defined(MACRO1) || defined(MACRO2) 를 사용

#error :
소스 라인에 직접 에러 메세지를 출력한다. 전처리기가 #error 문을 만나면 그 즉시 컴파일을 중단하고 다음과 같은 에러 메시지를 출력한다.
ERROR : XXXXX.c ########: Error directive: 내용
- XXXXX.c –> 현재 컴파일 중인 파일명
- ####### –> 전처리기가 #error 문을 만난 시점에서의 행 번호(헤더 포함)
#ifdef __LARGE__
#error This program must be compiled in LARGE memory model!
#endif
이 내용은 만일 프로그램이 LARGE 모델이 아니라면 “#error” 뒤에 표시된 메세지를 출력하고 컴파일을 중지하게 된다.

#line :
이 명령은 소스 코드의 행번호를 지정하기 위한 것으로 주로 컴파일러에 의해 미리 정의된 __LINE__과 함께 사용된다. C에서는 베이식과 같은 행번호를 사용하지 않는다. 하지만 디버깅을 하다 보면 “행번호를 알 수 있으면 좋을텐데”하는 경우가 있다. 예를 들자면 어떤 표현들이 있고, 그 표현들 중의 어딘가에 잘못된 부분이 있는 것 같은데 정확히 그 표현이 있는 행을 찾지 못한다면 “#line”을 사용함으로써 그 일을 보다 수월하게 할 수 있게 된다.
__LINE__과 __FILE__을 각각 행번호와 파일명으로 변경한다.
#include <stdio.h>
#define DEBUG
void main(void)
{
int count = 100;
#line 100  다음 줄번호를 100으로 설정한다
<-- 이 줄의 번호가 100이다
#ifdef DEBUG  <-- 이 줄의 번호가 101이다
printf("line:%d, count = %d\n", __LINE__, count);
#endif
count = count * count - 56;
#ifdef DEBUG
printf("line:%d, count = %d\n", __LINE__, count);
#endif
count = count / 2 + 48;
#ifdef DEBUG
printf("line:%d, count = %d\n", __LINE__, count);
#endif
}

*/
#pragma endregion

extern BOOL g_ShowFrame;
extern HWND g_hWnd;
extern YENADISPLAYMODE g_Mode;

extern LPB3YENADEVICE9 g_pDevice;


/**************** end of "Device.h" ***********************************/