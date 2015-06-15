#pragma once

#include "Point.h"

class Line {
private:
	Point& _startPoint;
	Point& _endPoint;

public:
	Line();
	Line(Point&, Point&);

	float getLength() const;

	Point& getStartPoint();
	Point& getEndPoint();

	~Line();
};