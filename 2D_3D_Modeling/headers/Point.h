#pragma once

class Point {
private:
	float _x;
	float _y;

public:
	Point() : _x(0.f), _y(0.f){}
	Point(float x, float y) : _x(x), _y(y){}
	Point(const Point& p) : _x(p._x), _y(p._y){}
	Point& operator=(const Point& p)
	{
		if (this != &p)
		{
			_x = p._x;
			_y = p._y;
		}
		return *this;
	}

	float getX(){ return _x; }
	float getY(){ return _y; }
	void setX(float x){ _x = x; }
	void setY(float y){ _y = y; }
};