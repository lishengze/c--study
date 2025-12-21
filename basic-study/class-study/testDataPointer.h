#ifndef _TESET_DATAPOINTER_H_
#define _TESET_DATAPOINTER_H_

class Point3d {
	public:
	float m_x, m_y, m_z;
	static int  s_x;
	virtual void test();
};

void testDataPointerFunc();

#endif