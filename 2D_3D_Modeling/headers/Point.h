#pragma once

class Point {
private:
	float _x;
	float _y;
	float _z;

public:
	Point() : _x(0.f), _y(0.f), _z(0.f) {}
	Point(float x, float y, float z) : _x(x), _y(y), _z(z) {}
	Point(const Point& p) : _x(p._x), _y(p._y), _z(p._z) {}
	Point& operator=(const Point& p)
	{
		if (this != &p)
		{
			_x = p._x;
			_y = p._y;
			_z = p._z;
		}
		return *this;
	}

	float getX(){ return _x; }
	float getY(){ return _y; }
	float getZ(){ return _z; }
	void setX(float x){ _x = x; }
	void setY(float y){ _y = y; }
	void setZ(float z){ _z = z; }
};