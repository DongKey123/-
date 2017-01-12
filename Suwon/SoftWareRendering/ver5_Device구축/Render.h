//Render.h
#pragma once

int DataLoading();
void DataRelease();

void SceneRender();

extern HWND g_hWnd;

//extern POINT g_Sp, g_Ep;//선그리기가 아닌 삼각형그리기
//extern POINT g_Vtx[3];
//extern POINT g_VtxTemp;




//void LineUpdate(POINT pt);
//void LineDraw();
//void LineDraw(POINT sp, POINT ep);
//void LineDashDraw(POINT sp, POINT ep);
//void LineClear();
//void FaceDraw();
//
//
//void SortVert(POINT* v0, POINT* v1, POINT* v2);
//void FaceDraw(POINT v0, POINT v1, POINT v2);
//
//void CrossDraw(POINT pt,char* name,...);
//
//int  RenderTargetCreate(HWND hwnd);
//void RenderTargetRelease();
//
//#pragma region CW와 CCW의 차이점
///*
//그리는 방향에 따라 CW 와 CCW를 나눔 -> 게임내에선 앞면과 뒷면을 나누는 기준이 됨
//CW(ClockWise) : 시계방향
//CCW(CounterClockWise) : 반시계방향
//// 위에서 시작할때 (밑에서 시작할때 res 값 반대)
//res = (A.x * B.y + B.x * C.y + C.x * A.y) - (A.x * C.y + B.x * A.y + C.x * B.y)
//res == 0 : 점 C는 선분 AB의 일직선상에 있습니다. (세 점 모두 일직선상에 있습니다.)
//res < 0  : 점 C는 선분 AB의 반시계방향에 있습니다. 
//res > 0  : 점 C는 선분 AB의 시계방향에 있습니다. 
//*/
//#pragma endregion
//inline int IsCW(int x1, int y1, int x2, int y2, int x3, int y3) {
//	int temp = x1*y2 + x2*y3 + x3*y1;
//	temp = temp - y1*x2 - y2*x3 - y3*x1;
//	if (temp > 0) {
//		return 1;
//	}
//	else if (temp < 0) {
//		return -1;
//	}
//	else {
//		return 0;
//	}
//}
//
//////////end