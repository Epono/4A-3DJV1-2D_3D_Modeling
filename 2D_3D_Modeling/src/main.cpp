#include <GL/glut.h>

#include <cstdlib>
#include <vector>
#include <iostream>
#include <math.h>

#include "utils.h"
//#include "Point.h"
//#include "LineStrip.h"

#define M_PI 3.14
#define WIDTH 1280
#define HEIGHT 720

// OLD STUFF
/////////////////////////////////////////////////////////////////////////////////
int creationState = waitingForFirstClick;

//std::vector<LineStrip*> lines;
//LineStrip *currentLine = nullptr;

float windowColor[3] = {0, 0.5f, 0.5f};		// Window color
int windowVerticeToMove = -1;
bool hideControlPoints = false;
float pas = 20;
color_rgb dessinColor = color_rgb(1.f, 0.f, 0.f);

//Point clicked;
/////////////////////////////////////////////////////////////////////////////////

// Camera management
/////////////////////////////////////////////////////////////////////////////////
float zoom3D = 15.0f;
float zoom2D = 15.0f;
float rotx3D = 0;
float roty3D = 0.001f;
float tx3D = 0;
float ty3D = 0;
float tx2D = 0;
float ty2D = 0;
int lastx = 0;
int lasty = 0;
unsigned char Buttons[3] = {0};
/////////////////////////////////////////////////////////////////////////////////

// Nurbs
/////////////////////////////////////////////////////////////////////////////////
float g_Points[7][3] = {
	{10, 10, 0},
	{5, 10, 2},
	{-5, 5, 0},
	{-10, 5, -2},
	{-4, 10, 0},
	{-4, 5, 2},
	{-8, 1, 0}
};

float g_Knots[] = {0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 4.0f, 4.0f, 4.0f};
unsigned int g_num_cvs = 7;
unsigned int g_degree = 3;
unsigned int g_order = g_degree + 1;
unsigned int g_num_knots = g_num_cvs + g_order;

unsigned int LOD = 20;
/////////////////////////////////////////////////////////////////////////////////

// Bezier 3D
/////////////////////////////////////////////////////////////////////////////////
/// a structure to hold a control point of the surface
struct Point {
	float x;
	float y;
	float z;
};

/// 4x4 grid of points that will define the surface
Point Points[4][4] = {
	{
		{10, 0, 10},
		{5, 0, 10},
		{-5, 0, 10},
		{-10, 0, 10}
	},
	{
		{10, 0, 5},
		{5, 6, 5},
		{-5, 6, 5},
		{-10, 0, 5}
	},
	{
		{10, 0, -5},
		{5, 6, -5},
		{-5, 6, -5},
		{-10, 0, -5}
	},
	{
		{10, 0, -10},
		{5, 0, -10},
		{-5, 0, -10},
		{-10, 0, -10}
	}
};

// the level of detail of the surface
//unsigned int LOD = 20;
/////////////////////////////////////////////////////////////////////////////////

bool is3DMode = true;
bool displayGrid = true;

int main(int argc, char **argv) {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glColor3f(0.0, 0.0, 0.0);
	glPointSize(4.0);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("FunStuffWithOpenGL");

	//createMenu();

	reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(keyboard);

	glEnable(GL_DEPTH_TEST);

	glutMainLoop();
}

void reshape(int w, int h) {
	if(w == 0)
		h = 1;

	if(is3DMode) {
		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45, (float) w / h, 0.1, 100);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
	else {
		//glViewport(0, 0, w, h);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(-w / 2, w / 2, -h / 2, h / 2);
		//glOrtho(-w/2, w/2, -h/2, h/2, -100.0f, 100.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPushMatrix();
	}
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	if(is3DMode) {
		gluLookAt(1, 10, 26,	//	eye pos
				  0, 5, 0,		//	aim point
				  0, 1, 0);		//	up direction

		glTranslatef(0, 0, -zoom3D);
		glTranslatef(tx3D, ty3D, 0);
		glRotatef(rotx3D, 1, 0, 0);
		glRotatef(roty3D, 0, 1, 0);
		if(displayGrid) drawGrid3D();
	}
	else {
		glScalef(zoom2D, zoom2D, zoom2D);
		glTranslatef(tx2D, ty2D, 0);
		//glRotatef(rotx3D, 1, 0, 0);
		//glRotatef(roty3D, 0, 1, 0);
		if(displayGrid) drawGrid2D();
	}

	drawNurbsCurveExample();
	drawBezier3D();

	glutSwapBuffers();
}

void mouse(int button, int state, int x, int y) {
	lastx = x;
	lasty = y;
	switch(button) {
	case GLUT_LEFT_BUTTON:
		Buttons[0] = ((GLUT_DOWN == state) ? 1 : 0);
		break;
	case GLUT_MIDDLE_BUTTON:
		Buttons[1] = ((GLUT_DOWN == state) ? 1 : 0);
		break;
	case GLUT_RIGHT_BUTTON:
		Buttons[2] = ((GLUT_DOWN == state) ? 1 : 0);
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void motion(int x, int y) {
	int diffx = x - lastx;
	int diffy = y - lasty;
	lastx = x;
	lasty = y;

	if(Buttons[2]) {
		if(is3DMode) {
			zoom3D -= (float) 0.05f * diffy;
		}
		else {
			zoom2D += (float) 0.05f * diffy;
		}
	}
	else {
		if(Buttons[0]) {
			rotx3D += (float) 0.5f * diffy;
			roty3D += (float) 0.5f * diffx;
		}
		else {
			if(Buttons[1]) {
				if(is3DMode) {
					tx3D += (float) 0.05f * diffx;
					ty3D -= (float) 0.05f * diffy;
				}
				else {
					tx2D += (float) 0.05f * diffx;
					ty2D -= (float) 0.05f * diffy;
				}
			}
		}
	}

	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	switch(key) {
	case 'd': // Switch to connected strip lines creation
		if(creationState == selectPoint) {
			creationState = waitingForNextClick;
		}
		else if(creationState != waitingForFirstClick) {
			printf("Switching to window creation\n");
			creationState = waitingForFirstClick;
		}
		break;
	case 'v': // Validates
		creationState = waitingForFirstClick;
		//if(currentLine != nullptr) {
		//	lines.push_back(currentLine);
		//	currentLine = new LineStrip();
		//}
		break;
	case 'c': // Clear the window
		creationState = waitingForFirstClick;
		//lines.clear();
		//currentLine = new LineStrip();
		break;
	case 's':
		// select point
		if(creationState != selectPoint) {
			printf("Switching to select point\n");
			creationState = selectPoint;
		}
		break;
	case 'h':
		// hide control points
		hideControlPoints = !hideControlPoints;
		break;
	case '+':
		// Increase the LOD
		++LOD;
		break;
	case '-':
		// Decrease the LOD
		--LOD;
		// have a minimum LOD value
		if(LOD < 3)
			LOD = 3;
		break;
	case 't':
		creationState = translating;
		break;
	case 'r':
		creationState = rotating;
		break;
	case 'o':
		creationState = scaling;
		break;
	case '3':
		if(!is3DMode) {
			is3DMode = true;
			reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
			std::cout << "Going in 3D mode !" << std::endl;
		}
		break;
	case '2':
		if(is3DMode) {
			is3DMode = false;
			reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
			std::cout << "Going in 2D mode !" << std::endl;
		}
		break;
	case '1':
		displayGrid = !displayGrid;
		break;
	case 127:
		// Suppr
		break;
	case 27:
		exit(0);
	}

	glutPostRedisplay();
}

void createMenu() {
	int mainMenu;

	mainMenu = glutCreateMenu(menu);

	glutAddMenuEntry("Vert", 1);
	glutAddMenuEntry("Rouge", 2);
	glutAddMenuEntry("Bleu", 3);
	glutAddMenuEntry("Nouvelle courbe", 4);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void menu(int opt) {
	switch(opt) {
	case 1:
		std::cout << "Vert" << std::endl;
		//currentLine->setColor(0.f, 1.f, 0.f);
		break;
	case 2:
		std::cout << "Rouge" << std::endl;
		//currentLine->setColor(1.f, 0.f, 0.f);
		break;
	case 3:
		std::cout << "Bleu" << std::endl;
		//currentLine->setColor(0.f, 0.f, 1.f);
		break;
	case 4:
		std::cout << "Nouvelle courbe" << std::endl;
		//if(currentLine != nullptr)
		//	lines.push_back(currentLine);
		//currentLine = new LineStrip();
		creationState = waitingForFirstClick;
		break;
	default:
		printf("What ? %d choisie mais pas d'option\n", opt);
		break;
	}
	display();
}

void setPolygonColor(float colors[3], float r, float g, float b) {
	*colors = r;
	*(colors + 1) = g;
	*(colors + 2) = b;
}

void drawGrid3D() {
	glLineWidth(5.0f);
	glBegin(GL_LINES);

	// Red = x
	// Green = y
	// Blue = z
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0, 0, 0);
	glVertex3f(5, 0, 0);

	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 5, 0);

	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 5);
	glEnd();

	glLineWidth(1.0f);
	glBegin(GL_LINES);
	glColor3f(1.0, 1.0, 1.0);
	for(int i = -10; i <= 10; ++i) {
		glVertex3f(i, 0, -10);
		glVertex3f(i, 0, 10);

		glVertex3f(10, 0, i);
		glVertex3f(-10, 0, i);
	}

	glEnd();
}

void drawGrid2D() {
	glLineWidth(5.0f);
	glBegin(GL_LINES);

	// Red = x
	// Green = y
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0, 0, 0);
	glVertex3f(5, 0, 0);

	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 5, 0);

	glEnd();

	glLineWidth(1.0f);
	glBegin(GL_LINES);
	glColor3f(1.0, 1.0, 1.0);
	for(int i = -10; i <= 10; ++i) {
		glVertex3f(i, -10, 0);
		glVertex3f(i, 10, 0);

		glVertex3f(10, i, 0);
		glVertex3f(-10, i, 0);
	}

	glEnd();
}

// Nurbs
/////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------	CoxDeBoor()
//
float CoxDeBoor(float u, int i, int k, const float* Knots) {
	if(k == 1) {
		if(Knots[i] <= u && u <= Knots[i + 1]) {
			return 1.0f;
		}
		return 0.0f;
	}
	float Den1 = Knots[i + k - 1] - Knots[i];
	float Den2 = Knots[i + k] - Knots[i + 1];
	float Eq1 = 0, Eq2 = 0;
	if(Den1 > 0) {
		Eq1 = ((u - Knots[i]) / Den1) * CoxDeBoor(u, i, k - 1, Knots);
	}
	if(Den2 > 0) {
		Eq2 = (Knots[i + k] - u) / Den2 * CoxDeBoor(u, i + 1, k - 1, Knots);
	}
	return Eq1 + Eq2;
}

//------------------------------------------------------------	GetOutpoint()
//
void GetOutpoint(float t, float OutPoint[]) {

	// sum the effect of all CV's on the curve at this point to 
	// get the evaluated curve point
	// 
	for(unsigned int i = 0; i != g_num_cvs; ++i) {

		// calculate the effect of this point on the curve
		float Val = CoxDeBoor(t, i, g_order, g_Knots);

		if(Val > 0.001f) {

			// sum effect of CV on this part of the curve
			OutPoint[0] += Val * g_Points[i][0];
			OutPoint[1] += Val * g_Points[i][1];
			OutPoint[2] += Val * g_Points[i][2];
		}
	}
}

void drawNurbsCurveExample() {
	// Courbe en elle même
	glColor3f(1, 1, 0);
	glBegin(GL_LINE_STRIP);
	for(int i = 0; i != LOD; ++i) {

		float t = g_Knots[g_num_knots - 1] * i / (float) (LOD - 1);

		if(i == LOD - 1)
			t -= 0.001f;

		float Outpoint[3] = {0, 0, 0};

		GetOutpoint(t, Outpoint);

		glVertex3fv(Outpoint);
	}
	glEnd();

	// Points de contrôle
	glColor3f(1, 0, 0);
	glPointSize(3);
	glBegin(GL_POINTS);
	for(int i = 0; i != g_num_cvs; ++i) {
		glVertex3fv(g_Points[i]);
	}
	glEnd();
}
/////////////////////////////////////////////////////////////////////////////////

// Bezier 3D
/////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------	CalculateU()
// Given our 16 control points, we can imagine 4 curves travelling
// in a given surface direction. For example, if the parametric u
// value was 0.5, we could imagine evaluating 4 seperate curves
// at u=0.5.
//
// This function is basically here to perform that very task. each
// row of 4 points in the u-direction is evaluated to provide us
// with 4 new points. These new points then form a curve we can
// evaluate in the v direction to calculate our final output point.
//
Point CalculateU(float t, int row) {

	// the final point
	Point p;

	// the t value inverted
	float it = 1.0f - t;

	// calculate blending functions
	float b0 = t*t*t;
	float b1 = 3 * t*t*it;
	float b2 = 3 * t*it*it;
	float b3 = it*it*it;

	// sum the effects of the Points and their respective blending functions
	p.x = b0*Points[row][0].x +
		b1*Points[row][1].x +
		b2*Points[row][2].x +
		b3*Points[row][3].x;

	p.y = b0*Points[row][0].y +
		b1*Points[row][1].y +
		b2*Points[row][2].y +
		b3*Points[row][3].y;

	p.z = b0*Points[row][0].z +
		b1*Points[row][1].z +
		b2*Points[row][2].z +
		b3*Points[row][3].z;

	return p;
}

//------------------------------------------------------------	CalculateV()
// Having generated 4 points in the u direction, we need to
// use those points to generate the final point on the surface
// by calculating a final bezier curve in the v direction.
//     This function takes the temporary points and generates
// the final point for the rendered surface
//
Point CalculateV(float t, Point* pnts) {
	Point p;

	// the t value inverted
	float it = 1.0f - t;

	// calculate blending functions
	float b0 = t*t*t;
	float b1 = 3 * t*t*it;
	float b2 = 3 * t*it*it;
	float b3 = it*it*it;

	// sum the effects of the Points and their respective blending functions
	p.x = b0*pnts[0].x +
		b1*pnts[1].x +
		b2*pnts[2].x +
		b3*pnts[3].x;

	p.y = b0*pnts[0].y +
		b1*pnts[1].y +
		b2*pnts[2].y +
		b3*pnts[3].y;

	p.z = b0*pnts[0].z +
		b1*pnts[1].z +
		b2*pnts[2].z +
		b3*pnts[3].z;

	return p;
}

//------------------------------------------------------------	Calculate()
// On our bezier patch, we need to calculate a set of points
// from the u and v parametric range of 0,0 to 1,1. This calculate
// function performs that evaluation by using the specified u
// and v parametric coordinates.
//
Point Calculate(float u, float v) {

	// first of all we will need to evaluate 4 curves in the u
	// direction. The points from those will be stored in this
	// temporary array
	Point temp[4];

	// calculate each point on our final v curve
	temp[0] = CalculateU(u, 0);
	temp[1] = CalculateU(u, 1);
	temp[2] = CalculateU(u, 2);
	temp[3] = CalculateU(u, 3);

	// having got 4 points, we can use it as a bezier curve
	// to calculate the v direction. This should give us our
	// final point
	//
	return CalculateV(v, temp);
}


void drawBezier3D() {
	Point *points = new Point[LOD*LOD];

	glColor3f(1, 0, 1);
	glPointSize(4);
	glBegin(GL_POINTS);

	// use the parametric time value 0 to 1
	for(int i = 0; i != LOD; ++i) {
		//std::cout << " i : " << i << std::endl;

		// calculate the parametric u value
		float u = (float) i / (LOD - 1);

		for(int j = 0; j != LOD; ++j) {
			//std::cout << " j : " << j << std::endl;

			// calculate the parametric v value
			float v = (float) j / (LOD - 1);

			// calculate the point on the surface
			Point p = Calculate(u, v);
			points[i*LOD + j] = p;

			// draw point
			glVertex3f(p.x, p.y, p.z);
			//std::cout << "(" << p.x << ", " << p.y << ", " << p.z << ")" << std::endl;
		}
	}
	glEnd();

	//glEnable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(1.0f);
	for(int z = 0; z < LOD - 1; z++) {
		glBegin(GL_TRIANGLE_STRIP);
		for(int x = 0; x < LOD; x++) {
			//std::cout << "(" << points[x][z].x << ", " << points[x][z].y << ", " << points[x][z].z << ")" << std::endl;
			glVertex3f(points[x*LOD + z].x, points[x*LOD + z].y, points[x*LOD + z].z);
			glVertex3f(points[x*LOD + z + 1].x, points[x*LOD + z + 1].y, points[x*LOD + z + 1].z);
		}
		glEnd();
	}

	delete[] points;
}
/////////////////////////////////////////////////////////////////////////////////