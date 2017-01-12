// B3Math.H : 수학헤더

#pragma once

//OpenGL 표준 헤더파일. 필수요소
//#include <GL/gl.h>
//#include <GL/glu.h>

// Radian or Degree 바꾸기 매크로
#define YN_PI   3.141592f
#define YNToRadian(a)  (a/180.0f * YN_PI)
#define YNToAngle(r)   (r * 180.0f / YN_PI)
#define YNToDegree(r)  YNToAngle(r)

// 벡터 데이터.
//
// 멤버데이터를 '수평:행'으로 나열했지만,
// 벡터/행렬 연산시에는 '수직:열'기준 처리됨을 기억합시다.
// 아래의 벡터 표현식 참조.
//
//  x   or   x
//  y        y
//  z        z
//           w
//

// 2성분 벡터 구조체.
//
struct B3YVECTOR2 {
	float x, y;
};


// 3성분 벡터 구조체.
//
struct B3YVECTOR3 {
	float x, y, z;
};


// 4성분 벡터.구조체.
//
struct B3YVECTOR4 {
	float x, y, z, w;
};



// VECTOR2  일반 구조체 + 생성자/연산자 오버로딩.
//
class B3YXVECTOR2
{
public:
	float x, y;
public:
	B3YXVECTOR2() { x = 0;  y = 0; }
	B3YXVECTOR2(float _x, float _y) { x = _x;  y = _y; }
	B3YXVECTOR2(B3YVECTOR2 v) { x = v.x;  y = v.y; }

	//연산자 오버로딩.★
	B3YXVECTOR2 operator + (B3YXVECTOR2 rhs);
	B3YXVECTOR2 operator - (B3YXVECTOR2 rhs);
	B3YVECTOR2 operator = (B3YXVECTOR2 rhs);
	operator B3YVECTOR2* ();
};
typedef B3YXVECTOR2 VECTOR2;

// 2성분 외적 : 외적의 결과가 양수면, 'CCW'.Back - Face Culling 에 사용됨
//
float B3YXVec2CCW(B3YXVECTOR2* v0, B3YXVECTOR2* v1);	//★

// VECTOR3  일반 구조체 + 생성자/연산자 오버로딩.
//
class B3YXVECTOR3
{
public:
	float x, y, z;
public:
	B3YXVECTOR3(float _x, float _y, float _z) { x = _x;  y = _y;   z = _z; }
	B3YXVECTOR3(B3YVECTOR3 v) { x = v.x;  y = v.y;  z = v.z; }
};
typedef B3YXVECTOR3 VECTOR3;



// VECTOR4 일반 구조체 + 생성자/연산자 오버로딩.
//
class B3YXVECTOR4
{
public:
	float x, y, z, w;
public:
	B3YXVECTOR4(float _x, float _y, float _z, float _w) { x = _x;  y = _y;   z = _z;  w = _w; }
	B3YXVECTOR4(B3YVECTOR4 v) { x = v.x;  y = v.y;  z = v.z; w = v.w; }

};
typedef B3YXVECTOR4 VECTOR4;




//컬러 구조체.
struct B3YCOLOR4 {
	float r, g, b, a;
};
typedef B3YCOLOR4 COLOR;

#define TOCOLOR(col)  (*(COLOR*)&(col))
COLOR   B3YXColor(float r, float g, float b, float a);




/************* end of file "B3Math.h" *******************************************/





