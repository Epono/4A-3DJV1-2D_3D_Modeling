//Point* DC(const std::vector<Point>& p, float t) {
//	int n = p.size();
//	std::vector<Point> q(n);
//	for(int i = 0; i < n; ++i)
//		q.at(i) = p.at(i);
//	for(int k = 1; k < n; ++k)
//		for(int i = 0; i < n - k; ++i) {
//			q.at(i).setX((1 - t)*q.at(i).getX() + t*q.at(i + 1).getX());
//			q.at(i).setY((1 - t)*q.at(i).getY() + t*q.at(i + 1).getY());
//		}
//	return new Point(q.at(0));
//}
//
//void drawBezier(float pas, LineStrip& line) {
//	Point A = line.getPoints().at(0), B;
//	for(float k = 1.f; k <= pas; ++k) {
//		B = *DC(line.getPoints(), k / pas);
//		drawLine(A, B);
//		A = B;
//	}
//}
//
//void drawLine(Point& p1, Point& p2) {
//	glBegin(GL_LINES);
//	glVertex2f(p1.getX(), p1.getY());
//	glVertex2f(p2.getX(), p2.getY());
//	glEnd();
//}
//
//void drawCurve(LineStrip& line, int lineSize) {
//	glLineWidth(lineSize);
//	glColor3f(1.0f, 0.0f, 0.0f);		// Sets the drawing color
//	if(!hideControlPoints) {
//		// Draws line strip
//		glBegin(GL_LINE_STRIP);
//		for(auto &p : line.getPoints())
//			glVertex2f(p.getX(), p.getY());
//		glEnd();
//
//		// Draws vertices of the connected lines strip
//		glBegin(GL_POINTS);
//		for(auto &p : line.getPoints())
//			glVertex2f(p.getX(), p.getY());
//		glEnd();
//	}
//	if(line.getPoints().size() > 2) {
//		color_rgb c = line.getColor();
//		glColor3f(c._r, c._g, c._b);		// Sets the drawing color
//		drawBezier(pas, line);
//	}
//}
//
//void keyboardSpecial(int key, int x, int y) {
//	int modifier = glutGetModifiers();
//
//	switch(modifier) {
//	case 0: // NONE - Translation
//		switch(key) {
//		case 100: // LEFT
//			translate(-10, 0);
//			break;
//		case 101: // UP
//			translate(0, -10);
//			break;
//		case 102: // RIGHT
//			translate(10, 0);
//			break;
//		case 103: // DOWN
//			translate(0, 10);
//			break;
//		}
//		break;
//	case 1: // SHIFT - Scaling
//		switch(key) {
//		case 100: // LEFT
//			scale(0.9f, 1.0f);
//			break;
//		case 101: // UP
//			scale(1.0f, 1.1f);
//			break;
//		case 102: // RIGHT
//			scale(1.1f, 1.0f);
//			break;
//		case 103: // DOWN
//			scale(1.0f, 0.9f);
//			break;
//		}
//		break;
//	case 2: // CTRL - Rotation
//		switch(key) {
//		case 100: // LEFT
//			rotate(0.05f);
//			break;
//		case 101: // UP
//			rotate(0.05f);
//			break;
//		case 102: // RIGHT
//			rotate(-0.05f);
//			break;
//		case 103: // DOWN
//			rotate(-0.05f);
//			break;
//		}
//		break;
//	case 3: // ALT
//		break;
//	}
//
//	glutPostRedisplay();
//}
//
//void translate(int xOffset, int yOffset) {
//
//	float x, y;
//
//	float matrix[6] = {
//		1, 0, xOffset,
//		0, 1, yOffset
//	};
//
//	std::vector<Point>& points = currentLine->getPoints();
//	for(unsigned int i = 0; i < points.size(); i++) {
//		x = points.at(i).getX();
//		y = points.at(i).getY();
//
//		points.at(i).setX((x * matrix[0]) + (y * matrix[1]) + (1 * matrix[2]));
//		points.at(i).setY((x * matrix[3]) + (y * matrix[4]) + (1 * matrix[5]));
//	}
//}
//
//void scale(float scaleX, float scaleY) {
//
//	if(scaleX == 0.0f) {
//		scaleX = 1.0f;
//	}
//	if(scaleY == 0.0f) {
//		scaleY = 1.0f;
//	}
//
//	float x, y;
//
//	float matrix[4] = {
//		scaleX, 0,
//		0, scaleY
//	};
//
//	float sumX = 0;
//	float sumY = 0;
//
//	//Calcul du barycentre pour décaler
//	std::vector<Point>& points = currentLine->getPoints();
//	for(unsigned int i = 0; i < points.size(); i++) {
//		sumX += points.at(i).getX();
//		sumY += points.at(i).getY();
//	}
//
//	Point barycenter = {sumX / currentLine->getPoints().size(), sumY / currentLine->getPoints().size()};
//
//	for(unsigned int i = 0; i < points.size(); i++) {
//		// Translate barycenter to origin
//		points.at(i).setX(points.at(i).getX() - barycenter.getX());
//		points.at(i).setY(points.at(i).getY() - barycenter.getY());
//
//		x = points.at(i).getX();
//		y = points.at(i).getY();
//
//		// Scale
//		points.at(i).setX((x * matrix[0]) + (y * matrix[1]));
//		points.at(i).setY((x * matrix[2]) + (y * matrix[3]));
//
//		// Translation back
//		points.at(i).setX(points.at(i).getX() + barycenter.getX());
//		points.at(i).setY(points.at(i).getY() + barycenter.getY());
//	}
//}
//
//void rotate(float angle) {
//
//	float x, y;
//	float cos_angle = cos(angle);
//	float sin_angle = sin(angle);
//
//	float matrix[4] = {
//		cos_angle, -sin_angle,
//		sin_angle, cos_angle
//	};
//
//	float sumX = 0;
//	float sumY = 0;
//
//	//Calcul du barycentre pour décaler
//	std::vector<Point>& points = currentLine->getPoints();
//	for(unsigned int i = 0; i < points.size(); i++) {
//		sumX += points.at(i).getX();
//		sumY += points.at(i).getY();
//	}
//
//	Point barycenter = {sumX / points.size(), sumY / points.size()};
//
//	for(unsigned int i = 0; i < points.size(); i++) {
//
//		// Translate barycenter to origin
//		points.at(i).setX(points.at(i).getX() - barycenter.getX());
//		points.at(i).setY(points.at(i).getY() - barycenter.getY());
//
//		x = points.at(i).getX();
//		y = points.at(i).getY();
//
//		// Rotation around origin
//		points.at(i).setX((x * matrix[0]) + (y * matrix[1]));
//		points.at(i).setY((x * matrix[2]) + (y * matrix[3]));
//
//		// Translation back
//		points.at(i).setX(points.at(i).getX() + barycenter.getX());
//		points.at(i).setY(points.at(i).getY() + barycenter.getY());
//	}
//}