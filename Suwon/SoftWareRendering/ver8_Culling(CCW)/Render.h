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
//
//////////end