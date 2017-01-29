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
	m_scale = 1;
	m_incrementScale = 1;
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
	/*QOpenGLShader vertexShader(QOpenGLShader::Vertex);
	QByteArray code = "uniform vec4 color;\n"
		"uniform highp mat4 matrix;\n"
		"void main(void) { gl_Position = gl_Vertex*matrix; }";
	vertexShader.compileSourceCode(code);

	QOpenGLShader fragmentShader(QOpenGLShader::Fragment);
	code = "uniform vec4 color;\n"
		"void main(void) { gl_FragColor = color; }";
	fragmentShader.compileSourceCode(code);

	program.addShader(&vertexShader);
	program.addShader(&fragmentShader);
	program.link();*/


	glClearColor(0, 0, 0, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	range = 100.0;

	m_shader.addShaderFromSourceCode(QOpenGLShader::Vertex, "VertexShader.vs");
	m_shader.addShaderFromSourceCode(QOpenGLShader::Fragment, "FragmentShader.fs");
	m_shader.link();
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
	/*m_projectionMatrix.setToIdentity();
	qreal
		ratio = qreal(window()->width()) / qreal(window()->height());
	m_projectionMatrix.perspective(90, ratio, 0.5, 40);
	m_viewMatrix.setToIdentity();
	QVector3D eye = QVector3D(0, 0, 2);
	QVector3D center = QVector3D(0, 0, 0);
	QVector3D up = QVector3D(0, 1, 0);
	m_viewMatrix.lookAt(eye, center, up);
	*/
	m_modelMatrix.setToIdentity();
	m_modelMatrix.rotate(45, 0, 1, 0);
	QMatrix4x4 modelViewMatrix = m_viewMatrix*m_modelMatrix;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3f(1.0f, 0.0f, 0.0f);

	// Shape Marking
	glPushMatrix();
	glRotatef(m_theta, 1.0f, 0.0f, 0.0f);
	glRotatef(m_phi, 0.0f, 0.0f, 1.0f);
	glScalef(m_scale, m_scale, m_scale);

	// Draw scene
	drawScene(modelViewMatrix);

	glPopMatrix();


}

// Fonction rendu de la scène
void GLWidget::drawScene(QMatrix4x4 mvMatrix)
{
	/*m_shader.bind();
	m_shader.setAttributeArray("Vertex", GL_FLOAT, m_data.constData(), 3, sizeof(ScenePoint));
	m_shader.enableAttributeArray("Vertex");
	m_shader.setAttributeArray("Normal", GL_FLOAT, &m_data[0].normal, 3, sizeof(ScenePoint));
	m_shader.enableAttributeArray("Normal");
	m_shader.setUniformValue("material.ka", QVector3D(0.1, 0, 0.0));
	m_shader.setUniformValue("material.kd", QVector3D(0.7, 0.0, 0.0));
	m_shader.setUniformValue("material.ks", QVector3D(1.0, 1.0, 1.0));
	m_shader.setUniformValue("material.shininess", 128.0f);
	m_shader.setUniformValue("light.position", QVector3D(2, 1, 1));
	m_shader.setUniformValue("material.shininess", QVector3D(1,1,1));

	m_shader.setUniformValue("projectionMatrix", m_projectionMatrix);
	m_shader.setUniformValue("modelViewMatrix", mvMatrix);
	m_shader.setUniformValue("mvpMatrix", m_projectionMatrix*mvMatrix);
	m_shader.setUniformValue("normalMatrix", mvMatrix.normalMatrix());

	glDrawArrays(GL_TRIANGLES, 0, 6);*/

	/*QMatrix4x4 m = { 10,0,0,0, 0,10,0,0, 0,0,10,0, 0,0,0,10};
	QColor color = Qt::red;
	program.setUniformValue("matrix", m);
	program.setUniformValue("color", color);*/

	// Afficher la grille et les Axes dans la scène
	if (showGrid)
		drawGridandAxes();

	drawPointsMatrix(bezier, QVector3D(255.0f, 255.0f, 255.0f), POINT_SIZE);
	drawPointsMatrix(surfBezier, QVector3D(0, 255.0f, 0), 5);
	drawSurfaceBezier();
}

void GLWidget::generateControlPoints()
{
	if (precision < 1 || degU < 1 || degV < 1)
		return;
	bezier.clear();
	bezier.resize(degU);
	int x, y, z, zx, zy;
	switch (modeGenPts)
	{
	case 1:
		for (int i = 0; i < degU; i++)
		{
			for (int j = 0; j < degV; j++)
			{
				x = randomGeneration(-130, 130);
				y = randomGeneration(-90, 90);
				z = randomGeneration(-50, 50);
				bezier[i].push_back(QVector3D(x, y, z));
			}
		}
		break;
	case 2:
		x = -degU/2*20;
		y = -degV/2*20;
		zx = 0;
		zy = 0;
		for (int i = 0; i < degU; i++)
		{
			for (int j = 0; j < degV; j++)
			{
				bezier[i].push_back(QVector3D(x, y, zy));
				y += 20;
				if (j < degV / 2)
					zy += 5 * depthBetweenPoints;
				else
					zy -= 5 * depthBetweenPoints;
			}
			y = -degV/2*20;
			x += 20;
			if (i < degU / 2) 
				zx += 5 * depthBetweenPoints;
			else
				zx -= 5 * depthBetweenPoints;
			zy = zx;
		}
		break;
	default:
		break;
	}
	generateSurfaceBezier();
}

void GLWidget::generateSurfaceBezier()
{
	if (precision < 1 || degU < 1 || degV < 1)
		return;
	surfBezier.clear();
	QElapsedTimer timer;
	timer.start();
	surfBezier = calcSurfaceBezier(bezier, precision);
	int time = timer.nsecsElapsed() / 1000;
	labelTimer = QString::number(time) + " us";
	emit labelChanged();
}

void GLWidget::drawSurfaceBezier()
{
	if (precision < 1 || degU < 1 || degV < 1)
		return;
	float black[] = { 0.0, 0.0, 0.0 };
	float white[] = { 1.0, 1.0, 1.0 };
	float red[] = { 1.0, 0.0, 0.0 };
	glColor3fv(red);
	QVector3D posLight = { 0,0,100 };
	QVector3D normal;
	iAmbiant = { 1.0,1.0,1.0 };
	kAmbiant = 0.2;
	kDiffuse = 0.8;
	iDiffuse = { 1.0,1.0,1.0 };
	for (int i = 0; i < precision; i++)
	{
		for (int j = 0; j < precision; j++)
		{
			//glColor3fv(iAmbiant*kAmbiant);
			QVector3D u = surfBezier[i + 1][j] - surfBezier[i][j];
			QVector3D v = surfBezier[i + 1][j + 1] - surfBezier[i][j];
			QVector3D normal = crossProductNormalized(u, v);
			/*glColor3fv(red);
			glBegin(GL_LINES);
			glVertex3f(surfBezier[i][j].x(), surfBezier[i][j].y(), surfBezier[i][j].z());
			glVertex3f(normal.x()*100 - surfBezier[i][j].x(), normal.y()*100 - surfBezier[i][j].y(), normal.z()*100 - surfBezier[i][j].z());
			glEnd();*/

			QVector3D dir = posLight - surfBezier[i][j];
			float angle = dot(normal, dir)/(sqrt(pow(normal.x(),2)+ pow(normal.y(), 2)+ pow(normal.z(), 2))*sqrt(pow(dir.x(), 2) + pow(dir.y(), 2) + pow(dir.z(), 2)));
			if (angle <= 0) angle = 0;
			QVector3D _red = iAmbiant*kAmbiant + iDiffuse*kDiffuse * angle;
			float __red[] = { _red.x(), _red.y(),_red.z() };
			glColor3fv(__red);

			if (showWireframe)
				glBegin(GL_LINE_STRIP);
			else
				glBegin(GL_TRIANGLE_STRIP);
			glVertex3fv(convertVector3D(surfBezier[i][j]));
			glVertex3fv(convertVector3D(surfBezier[i + 1][j]));
			glVertex3fv(convertVector3D(surfBezier[i + 1][j + 1]));
			glVertex3fv(convertVector3D(surfBezier[i][j + 1]));
			glVertex3fv(convertVector3D(surfBezier[i][j]));
			glVertex3fv(convertVector3D(surfBezier[i + 1][j + 1]));
			glEnd();
		}
		/*
		bool color = false;
		glBegin(GL_TRIANGLE_STRIP);
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

// Dessiner des points
void GLWidget::drawPoints(vector<QVector3D> pts, QVector3D color, int ptSize)
{
	int nbPoints = pts.size();
	if (nbPoints == 0)
		return;
	glColor3fv(convertVector3D(color));
	glPointSize(ptSize);
	glBegin(GL_POINTS);
	for (int i = 0; i < nbPoints; i++)
		glVertex3fv(convertVector3D(pts[i]));
	glEnd();
}

// Dessiner des points
void GLWidget::drawPointsMatrix(vector<vector<QVector3D>> pts, QVector3D color, int ptSize)
{
	if (pts.size() == 0 || pts[0].size() == 0)
		return;
	int m = pts.size();
	int n = pts[0].size();
	glColor3fv(convertVector3D(color));
	glPointSize(ptSize);
	glBegin(GL_POINTS);
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			glVertex3fv(convertVector3D(pts[i][j]));
	glEnd();
}

// Conversion de coordonnées d'écran à coordonnées de la scène OPENGL
QVector3D GLWidget::convertXY(int X, int Y)
{
	return QVector3D((int)((float)X * 2.0 * range * m_aspectRatio / screenW - range * m_aspectRatio), (int)((float)Y * 2.0 * range / screenH - range), 0);
}

// Callback pour les click de la souris
void GLWidget::mousePressEvent(QMouseEvent *event)
{
	//pointSelected = findNearestPoint(event->pos());
	// Ajout d'un point
	if (event->buttons() & Qt::LeftButton)
	{
		if (pointSelected == -1) 
		{  
			//points.push_back(convertXY(event->pos().x(), event->pos().y()));
			needUpdate = true;
		}
	}
	// Suppression d'un point
	else if (event->buttons() & Qt::RightButton)
	{
		if (pointSelected != -1) 
		{
			//points.erase(points.begin() + pointSelected);
			needUpdate = true;
		}
	}
}

// Callback pour le mouvement de la souris
void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
	mouse = event->pos();
	mouseWorld = convertXY(mouse.x(), mouse.y());
	emit mouseMoved();
	if (event->buttons() & Qt::LeftButton)
	{
		if (pointSelected >= 0)
		{
			//points[pointSelected] = convertXY(event->pos().x(), event->pos().y());
			needUpdate = true;
		}
	}
}

// Callback pour la relâche de la souris
void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton && pointSelected >= 0)
	{
	}
}

// Chercher du point (dans la nuage existante) la plus proche de la souris
int GLWidget::findNearestPoint(vector<QVector3D> pts, QPoint p)
{
	int nbPoints = pts.size();
	for (int i = 0; i < nbPoints; i++)
	{
		QVector3D d = convertXY(p.x(), p.y()) - QVector3D(pts[i].x(), pts[i].y(), 0);
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

// Les contrôles de clavier
void GLWidget::keyPressEvent(QKeyEvent* e)
{
	switch (e->key())
	{
		case Qt::Key_Escape:
			qApp->quit();
			break;
		case Qt::Key_Left:
			m_phi += 2.0f;
			break;
		case Qt::Key_Right:
			m_phi -= 2.0f;
			break;
		case Qt::Key_Up:
			m_theta += 2.0f;
			break;
		case Qt::Key_Down:
			m_theta -= 2.0f;
			break;
		case Qt::Key_Minus:
			depthBetweenPoints -= 1;
			generateControlPoints();
			break;
		case Qt::Key_Plus:
			depthBetweenPoints += 1;
			generateControlPoints();
			break;
		default:
			break;
	}
}

void GLWidget::wheelEvent(QWheelEvent * event)
{	  
	m_incrementScale += event->delta() / 120;
	if (m_incrementScale == 0 && event->delta() / 120 > 0) 
	{
		m_incrementScale = 1.0f;
	}
	if (m_incrementScale == 0 && event->delta() / 120 < 0) 
	{
		m_incrementScale = -1.0f;
	}
	if (m_incrementScale < 0) 
	{
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
	bezier.clear();
	surfBezier.clear();
	points.clear();
	degU = 0;
	degV = 0;
	needUpdate = true;
}