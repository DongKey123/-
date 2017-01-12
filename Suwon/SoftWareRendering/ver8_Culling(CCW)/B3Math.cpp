#include "Windows.h"  
#include "stdio.h"
#include "stdlib.h" 
#include "math.h"

#include "B3Math.h"

///////////////////////////////////////////////////////////////////////////////
//
// VECTOR2
//
///////////////////////////////////////////////////////////////////////////////

B3YXVECTOR2 B3YXVECTOR2::operator + (B3YXVECTOR2 rhs)
{
	B3YXVECTOR2 v;
	v.x = x + rhs.x;
	v.y = y + rhs.y;

	return v;
}

B3YXVECTOR2 B3YXVECTOR2::operator - (B3YXVECTOR2 rhs)
{
	B3YXVECTOR2 v;
	v.x = x - rhs.x;
	v.y = y - rhs.y;

	return v;
}

B3YXVECTOR2::operator B3YVECTOR2*()
{
	return (B3YVECTOR2*)this;
}

///////////////////////////////////////////////////////////////////////////////
//
// 2성분 외적 : 외적의 결과가 양수면, 'CCW' .Back-Face Culling 에 사용됨
//  
// 2성분으로는 구조적으로 외적이 불가능하므로 3차원으로 확장사용. 아래의 공식참조.
// v0=(x1,y1,0) cross  v1=(x2,y2,0). 
// 또는 아래 수식 참조.
/* [3성분 외적 참조]
VECTOR3 v;
v.x = v0.y * v1.z - v0.z * v1.y;
v.y = v0.z * v1.x - v0.x * v1.z;
v.z = v0.x * v1.y - v0.y * v1.x;  <-- z 정보만으로도 판정 가능
									[2D Screen 공간 기준] z<=0 이면, "뒷면", z>0 이면 "앞면"
									[3D 공간(DX, ViewSpace) 기준]  z<=0 이면 "앞면", z>0 이면 "뒷면"
*/

float B3YXVec2CCW(B3YXVECTOR2* v0, B3YXVECTOR2* v1)	//★
{
	float z = v0->x * v1->y - v0->y * v1->x;
	return z;
}

/************* end of file "Math.cpp" *******************************************/
