#include <GL/glut.h>

#include <cstdlib>
#include <vector>
#include <iostream>
#include <math.h>

#include "utils.h"
#include "Point.h"
#include "LineStrip.h"

#define M_PI 3.14
#define WIDTH 1280
#define HEIGHT 720

// OLD STUFF
/////////////////////////////////////////////////////////////////////////////////
int creationState = waitingForFirstClick;

std::vector<LineStrip*> lines;
LineStrip *currentLine = nullptr;

float windowColor[3] = {0, 0.5f, 0.5f};		// Window color
int windowVerticeToMove = -1;
bool hideControlPoints = false;
float pas = 20;
color_rgb dessinColor = color_rgb(1.f, 0.f, 0.f);

Point clicked;
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

bool is3DMode = true;

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
		drawGrid3D();
	}
	else {
		glScalef(zoom2D, zoom2D, zoom2D);
		glTranslatef(tx2D, ty2D, 0);
		drawGrid2D();
	}

	drawNurbsCurveExample();

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
		if(currentLine != nullptr) {
			lines.push_back(currentLine);
			currentLine = new LineStrip();
		}
		break;
	case 'c': // Clear the window
		creationState = waitingForFirstClick;
		lines.clear();
		currentLine = new LineStrip();
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
	case '-':
		if(pas > 0) --pas;
		break;
	case '+':
		// decrease step
		++pas;
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
		currentLine->setColor(0.f, 1.f, 0.f);
		break;
	case 2:
		std::cout << "Rouge" << std::endl;
		currentLine->setColor(1.f, 0.f, 0.f);
		break;
	case 3:
		std::cout << "Bleu" << std::endl;
		currentLine->setColor(0.f, 0.f, 1.f);
		break;
	case 4:
		std::cout << "Nouvelle courbe" << std::endl;
		if(currentLine != nullptr)
			lines.push_back(currentLine);
		currentLine = new LineStrip();
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

