#include "stdafx.h"
#include "GLWidget.h"

// Initialisation de la scène OpenGL
GLWidget::GLWidget(QWidget *parent) :
	QOpenGLWidget(parent), 
	m_theta(180.0f),
	m_phi(0.0f),
	m_aspectRatio(1.0)
{
	int seconde = 1000; // 1 seconde = 1000 ms
	int timerInterval = seconde / 60;
	t_Timer = new QTimer(this);
	connect(t_Timer, SIGNAL(timeout()), this, SLOT(timeOutSlot()));
	t_Timer->start(timerInterval);
	setMouseTracking(true);
	setFocusPolicy(Qt::StrongFocus);
	depthBetweenPoints = 0;
	m_scale = 1;
	m_incrementScale = 1;

	/*QOpenGLShader vertexShader(QOpenGLShader::Vertex);
	QByteArray code = "uniform vec4 color;\n"
		"uniform highp mat4 matrix;\n"
		"void main(void) { gl_Position = gl_Vertex*matrix; }";
	vertexShader.compileSourceCode(code);

	QOpenGLShader fragmentShader(QOpenGLShader::Fragment);
	code = "uniform vec4 color;\n"
		"void main(void) { gl_FragColor = color; }";
	fragmentShader.compileSourceCode(code);

	QOpenGLShaderProgram program;
	program.addShader(&vertexShader);
	program.addShader(&fragmentShader);
	program.link();*/
}

GLWidget::~GLWidget()
{
	delete[] t_Timer;
}

// Pour garder les FPS à 60
void GLWidget::timeOutSlot()
{
	update();
}

// Initialisation du module OpenGL
void GLWidget::initializeGL()
{
	glClearColor(0, 0, 0, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	range = 100.0;
	generateControlPoints();
}

// Redimensionner de la scène pour adapter à la fenêtre principale
void GLWidget::resizeGL(int width, int height)
{
	screenH = height;
	screenW = width;

	if (height == 0)
		height = 1;

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	m_aspectRatio = double(width) / double(height);
	//gluOrtho2D(0, width, height, 0);
	if (width <= height)
		glOrtho(-range, range, -range / m_aspectRatio, range / m_aspectRatio, range*4, -range*4);
	else
		glOrtho(-range * m_aspectRatio, range * m_aspectRatio, -range, range, range*4, -range*4);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// Fonction mettre à jour de la scène OpenGL
void GLWidget::paintGL()
{
	glClearColor(0, 0, 0, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3f(1.0f, 0.0f, 0.0f);

	// Shape Marking
	glPushMatrix();
	glRotatef(m_theta, 1.0f, 0.0f, 0.0f);
	glRotatef(m_phi, 0.0f, 0.0f, 1.0f);
	glScalef(m_scale, m_scale, m_scale);

	// Draw scene
	drawScene();

	glPopMatrix();
}

// Fonction rendu de la scène
void GLWidget::drawScene()
{
	// Afficher la grille et les Axes dans la scène
	if (showGrid)
		drawGridandAxes();

	// Affichage de la Triangulation
	QElapsedTimer timer;
	int time;
	// Affichage de l'envelope convexe
	vector<QVector3D> envelop;
	switch (modeEnvelop)
	{
	case 1:	// Enveloppe par méthode marche de Jarvis
		/*timer.start();
		envelop = EnvelopeJarvis(points);
		time = timer.nsecsElapsed() / 1000;
		labelTimer[0] = QString::number(time) + " us";
		emit labelChanged(0);
		drawPoly(envelop, QVector3D(150.0f, 150.0f, 150.0f), 3);*/
		break;
	case 2:	// Enveloppe par méthode Graham-Scan
		break;
	default:
		break;
	}
	drawPoints(points, QVector3D(255.0f, 255.0f, 255.0f));
	drawPoints(ptsSurf, QVector3D(0, 255.0f, 0));
	drawSurfaceBezier();
}

void GLWidget::generateControlPoints()
{
	if (precision < 1 || degU < 1 || degV < 1)
		return;
	bezier.clear();
	points.clear();
	bezier.resize(degU);
	/*for (int i = 0; i < degU; i++)
	{
		bezier[i].resize(degV);
		for (int j = 0; j < degV; j++)
		{
			int x = randomGeneration(-130, 130);
			int y = randomGeneration(-90, 90);
			int z = randomGeneration(-50, 50);
			bezier[i][j] = QVector3D(x, y, z);
			points.push_back(bezier[i][j]);
		}
	}*/

	int x = -degU/2*20;
	int y = -degV/2*20;
	int zx = 0;
	int zy = 0;
	for (int i = 0; i < degU; i++)
	{
		bezier[i].resize(degV);
		for (int j = 0; j < degV; j++)
		{
			bezier[i][j] = QVector3D(x, y, zy);
			points.push_back(bezier[i][j]);
			y += 20;
			if (j < degV / 2) {
				zy += 5 * depthBetweenPoints;
			}
			else
			{
				zy -= 5 * depthBetweenPoints;
			}
		}
		y = -degV/2*20;
		x += 20;
		if (i < degU / 2) {
			zx += 5 * depthBetweenPoints;
		}
		else
		{
			zx -= 5 * depthBetweenPoints;
		}
		zy = zx;
	}
	calculateSurfaceBezier();
}

void GLWidget::calculateSurfaceBezier()
{
	if (precision < 1 || degU < 1 || degV < 1)
		return;
	surfBezier.clear();
	ptsSurf.clear();
	surfBezier = calcSurfaceBezier(bezier, precision);
	for (int i = 0; i < precision; i++)
		for (int j = 0; j < precision; j++)
			ptsSurf.push_back(surfBezier[i][j]);
}

void GLWidget::drawSurfaceBezier()
{
	if (precision < 1 || degU < 1 || degV < 1)
		return;
	bool color = false;
	float black[] = { 0.0, 0.0, 0.0 };
	float white[] = { 1.0, 1.0, 1.0 };
	float red[] = { 1.0, 0.0, 0.0 };
	glColor3fv(red);
	for (int i = 0; i < precision - 1; i++)
	{
		for (int j = 0; j < precision-1; j++)
		{
			if (showWireframe) {
				glBegin(GL_LINE_STRIP);
			}
			else
			{
				glBegin(GL_TRIANGLE_STRIP);
			}
			glVertex3f(surfBezier[i][j].x(), surfBezier[i][j].y(), surfBezier[i][j].z());
			glVertex3f(surfBezier[i + 1][j].x(), surfBezier[i + 1][j].y(), surfBezier[i + 1][j].z());
			glVertex3f(surfBezier[i + 1][j + 1].x(), surfBezier[i + 1][j + 1].y(), surfBezier[i + 1][j + 1].z());
			glVertex3f(surfBezier[i][j].x(), surfBezier[i][j].y(), surfBezier[i][j].z());
			glVertex3f(surfBezier[i + 1][j + 1].x(), surfBezier[i + 1][j + 1].y(), surfBezier[i + 1][j + 1].z());
			glVertex3f(surfBezier[i][j + 1].x(), surfBezier[i][j + 1].y(), surfBezier[i][j + 1].z());
			glEnd();
		}
		/*glBegin(GL_TRIANGLE_STRIP);
		for (int vi = 0; vi < precision; vi++) 
		{
			if (color)
				glColor3fv(white);
			else
				glColor3fv(black);
			QVector3D p1 = surfBezier[ui][vi];
			QVector3D p2 = surfBezier[ui + 1][vi];
			glVertex3f(p1.x(), p1.y(), p1.z());
			glVertex3f(p2.x(), p2.y(), p2.z());
			color = !color;
		}
		if (precision % 2 == 1) 
			color = !color;
		glEnd();*/
	}
}

// Conversion de coordonnées d'écran à coordonnées de la scène OPENGL
QVector3D GLWidget::convertXY(int X, int Y)
{
	return QVector3D((int)((float)X * 2.0 * range * m_aspectRatio / screenW - range * m_aspectRatio), (int)((float)Y * 2.0 * range / screenH - range), 0);
}

// Callback pour les click de la souris
void GLWidget::mousePressEvent(QMouseEvent *event)
{
	pointSelected = findNearestPoint(event->pos());
	QElapsedTimer timer;
	// Ajout d'un point
	if (event->buttons() & Qt::LeftButton)
	{
		if (pointSelected == -1) 
		{  
			points.push_back(convertXY(event->pos().x(), event->pos().y()));
			needUpdate = true;
		}
	}
	// Suppression d'un point
	else if (event->buttons() & Qt::RightButton)
	{
		if (pointSelected != -1) 
		{
			points.erase(points.begin() + pointSelected);
			needUpdate = true;
		}
	}
}

// Callback pour le mouvement de la souris
void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
	mousePos = event->pos();
	if (event->buttons() & Qt::LeftButton)
	{
		if (pointSelected >= 0)
		{
			points[pointSelected] = convertXY(event->pos().x(), event->pos().y());
			needUpdate = true;
		}
	}
}

// Callback pour la relâche de la souris
void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton && pointSelected >= 0)
	{
		//update();
	}
}

// Chercher du point (dans la nuage existante) la plus proche de la souris
int GLWidget::findNearestPoint(QPoint p)
{
	int nbPoints = points.size();
	for (int i = 0; i < nbPoints; i++)
	{
		QVector3D d = convertXY(p.x(), p.y()) - QVector3D(points[i].x(), points[i].y(), 0);
		if (sqrt(pow(d.x(), 2) + pow(d.y(), 2) <= POINT_SIZE*4))
			return i;
	}
	return -1;
}

// Dessiner la grille et les axes dans la scène
void GLWidget::drawGridandAxes()
{
	// Grid
	glLineWidth(1);
	glBegin(GL_LINES);
	glColor3f(0.1, 0.1, 0.1);
	for (float x = -100; x < 100; x += 5)
	{
		glVertex3d(x, -100, 0);
		glVertex3d(x, 100, 0);
	}
	glEnd();
	glBegin(GL_LINES);
	glColor3f(0.1, 0.1, 0.1);
	for (float z = -100; z < 100; z += 5)
	{
		glVertex3d(-100, z, 0);
		glVertex3d(100, z, 0);
	}
	glEnd();

	// Axis
	glColor3f(0, 1, 0);
	glBegin(GL_LINES);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 50, 0);
	glEnd();

	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	glVertex3d(0, 0, 0);
	glVertex3d(50, 0, 0);
	glEnd();

	glColor3f(0, 0, 1);
	glBegin(GL_LINES);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, 50);
	glEnd();
}

// Dessiner des côtés à partir des couples de points
void  GLWidget::drawLinesFromPoints(vector<QVector3D> pts)
{
	int nbSides = pts.size();
	if (nbSides == 0)
		return;
	glColor3f(150.0f, 0.0f, 0.0f);
	//glPushAttrib is done to return everything to normal after drawing
	glPushAttrib(GL_ENABLE_BIT);
	glLineStipple(10, 0xAAAA);
	//glEnable(GL_LINE_STIPPLE);
	glBegin(GL_LINES);
	for (int i = 0; i < nbSides; i+=2)
	{
		glVertex3f(pts[i].x(), pts[i].y(), pts[i].z());
		glVertex3f(pts[i + 1].x(), pts[i + 1].y(), pts[i + 1].z());
	}
	glEnd();
	glPopAttrib();
}

// Dessiner d'un polygone à partir d'un ensemble des points
void GLWidget::drawPoly(vector<QVector3D> pts, QVector3D color, float width)
{
	int nbPoints = pts.size();
	if (nbPoints == 0)
		return;
	glColor3f(color.x(), color.y(), color.z());
	glLineWidth(width);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < nbPoints; i++)
		glVertex3f(pts[i].x(), pts[i].y(), pts[i].z());
	glEnd();
}

// Dessiner des points
void GLWidget::drawPoints(vector<QVector3D> points, QVector3D color)
{
	int nbPoints = points.size();
	if (nbPoints == 0)
		return;
	glColor3f(color.x(), color.y(), color.z());
	glPointSize(POINT_SIZE);
	glBegin(GL_POINTS);
	for (int i = 0; i < nbPoints; i++)
		glVertex3f(points[i].x(), points[i].y(), points[i].z());
	glEnd();
}

// Les contrôles de clavier
void GLWidget::keyPressEvent(QKeyEvent* e)
{
	switch (e->key())
	{
	case Qt::Key_Escape:
		QCoreApplication::instance()->quit();
		break;

	case Qt::Key_Left:
		m_phi += 2.0f;
		//update();
		break;

	case Qt::Key_Right:
		m_phi -= 2.0f;
		//update();
		break;

	case Qt::Key_Up:
		m_theta += 2.0f;
		//update();
		break;

	case Qt::Key_Down:
		m_theta -= 2.0f;
		//update();
		break;
	case Qt::Key_Minus:
		depthBetweenPoints -= 1;
		generateControlPoints();
		break;
	case Qt::Key_Plus:
		depthBetweenPoints += 1;
		generateControlPoints();
		break;
	}
}

void GLWidget::wheelEvent(QWheelEvent * event)
{	  
	m_incrementScale += event->delta() / 120;
	if (m_incrementScale == 0 && event->delta() / 120 > 0) {
		m_incrementScale = 1.0f;
	}
	if (m_incrementScale == 0 && event->delta() / 120 < 0) {
		m_incrementScale = -1.0f;
	}
	if (m_incrementScale < 0) {
		m_scale = 1 / -m_incrementScale;
	}
	else
	{
		m_scale = m_incrementScale;
	}
	//resizeGL(screenH, screenW);
};

// Réinitialiser le caméra au paramètres par défaut
void GLWidget::resetCamera() 
{
	m_theta = 180.0f;
	m_phi = 0.0f;
	//QApplication::setOverrideCursor(Qt::PointingHandCursor);
}

// Réinitialiser les données
void GLWidget::resetData()
{
	points.clear();
	needUpdate = true;
}