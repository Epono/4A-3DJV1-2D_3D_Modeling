#pragma once

//#include "Point.h"

/* Functions prototypes */
void display();										// manages displaying
void keyboard(unsigned char key, int x, int y);		// manages keyboard inputs
void keyboardSpecial(int key, int x, int y);		// manages special keyboard inputs
void mouse(int bouton, int etat, int x, int y);		// manages mouse clicks
void motion(int x, int y);							// manages mouse motions
void reshape(int w, int h);							// manages window resizing

void drawGrid2D();
void drawGrid3D();
void drawNurbsCurveExample();
void drawBezier3D();

void createMenu();
void menu(int opt);
void colorPicking(int option);
void setPolygonColor(float colors[3], float r, float g, float b);

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