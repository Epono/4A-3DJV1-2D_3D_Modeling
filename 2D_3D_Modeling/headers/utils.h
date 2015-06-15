#pragma once

#include "Point.h"

// Represents the state of the creation action
typedef enum creationState {
	waitingForFirstClick,
	waitingForNextClick,
	pending,
	selectPoint,
	scaling,
	rotating,
	translating
};

typedef struct color_rgb {
	color_rgb(float r, float g, float b) :_r(r), _g(g), _b(b) {
	}
	float _r;
	float _g;
	float _b;
} color_rgb;