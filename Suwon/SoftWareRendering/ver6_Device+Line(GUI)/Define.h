//Define.h : 기본 정의 문서

#pragma once


#include "windows.h"
#pragma warning(disable 4305)
/////////////////////////////////////////////////////////////////////////////
//
#ifdef YES_
#undef YES_
#endif
#define YES_ 1
#define NO_	 0

/////////////////////////////////////////////////////////////////////////////
//
#include "assert.h"
#define ASSERT(Val) assert((Val))

#define YN_INVALIED(res) ((res) == NULL)
#define YN_VALIED(res) ((res) != NULL)
#define YN_FAILED(res) ((res) < 0 )

#define YN_OK       0 
#define YN_FALSE   -1 
#define YN_FAIL    YN_FALSE
#pragma region assert 설명
/*
assert()매크로는 표명을 구현하여 프로그램이 추정한 것을 확인하며 거짓인 경우 진단 메시지를 출력
*/
#pragma endregion 

/////////////////////////////////////////////////////////////////////////////
//
#define _LOGFILE_ON_			//로그 파일 생성하기..

///////////////////////////////////////////////////////////////////////////////////////
//
// DXUtil.h 의 것을 사용함. 2004.06.07. 
//

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(pBuff) if((pBuff)){ (pBuff)->Release(); (pBuff) = NULL; }
#endif
#ifndef SAFE_DELETE
#define SAFE_DELETE(pBuff)	if((pBuff)){ delete (pBuff); (pBuff) = NULL; }
#endif
#ifdef SAFE_DELETE_ARRAY
#define SAFE_DELARRY  SAFE_DELETE_ARRAY
#else
#define SAFE_DELARRY(pBuff) if((pBuff)){ delete [] (pBuff); (pBuff) = NULL; }
#endif

/////////////////////////////////////////////////////////////////////////////
//
//'도'->'라디안' 으로 변경 메크로.
// pi : 180 = r : 1 
// r = pi / 180      즉  1 라디안 : 0.017444444444444444
// 
#define B3Y_PI  3.141592f 
#define MAKERADIAN(do) ( (float)(B3Y_PI/180.0f)*((float)do) )

/*********** end of file "B3D_Define.h" *******************************/