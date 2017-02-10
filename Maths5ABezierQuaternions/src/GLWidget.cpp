#include "stdafx.h"
#include "GLWidget.h"
#include <GL/glu.h>

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
	lights[0].posLight = { 0, 0, 300 };
	lights[0].iAmbiant = { 1.0,1.0,1.0 };
	lights[0].iDiffuse = { 1.0,1.0,1.0 };
	lights[1].posLight = { -100, 150, 150 };
	lights[1].iAmbiant = { 1.0,0.0,1.0 };
	lights[1].iDiffuse = { 0.0,0.0,1.0 };
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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	range = 100.0;

	LoadGLTextures("grass1.jpg");

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

	//gluLookAt(20,20,20, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	//QGLCamera camera;
	//gluLookAt(20, 20, 0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//gluPerspective(60.0f, 1.0*width / height, 0.1f, 100.0f);

	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, (float)width / height, 1, 1000);
	glEnable(GL_DEPTH_TEST);*/
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
	

	/*glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.75, 0.75, 0.75, 0, 0, 0, 0, 0, 1);

	glBegin(GL_QUADS);
	//carré rouge
	glColor3ub(255, 0, 0);
	glVertex3d(1, 0, 0);
	glVertex3d(1, 1, 0);
	glVertex3d(1, 1, 1);
	glVertex3d(1, 0, 1);
	//carré vert
	glColor3ub(0, 255, 0);
	glVertex3d(1, 1, 0);
	glVertex3d(0, 1, 0);
	glVertex3d(0, 1, 1);
	glVertex3d(1, 1, 1);
	//carré bleu
	glColor3ub(0, 0, 255);
	glVertex3d(1, 1, 1);
	glVertex3d(1, 0, 1);
	glVertex3d(0, 0, 1);
	glVertex3d(0, 1, 1);
	glEnd();*/
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

	drawSurfaceBezier();
	vector<QVector3D> ptlight = { lights[0].posLight };
	vector<QVector3D> ptlight2 = { lights[1].posLight };
	drawPoints(ptlight, lights[0].iAmbiant, 20);
	drawPoints(ptlight2, lights[1].iAmbiant, 20);
	if (showPts)
	{
		drawPointsMatrix(ptsControl, QVector3D(1.0, 0, 0), POINT_SIZE);
		drawPointsMatrix(ptsBezier, QVector3D(0, 1.0, 0), 5);
	}
	// Surbriller les points de raccordement
	drawPoints(ptsHighlighted, QVector3D(0, 0, 1.0), 10);
}

void GLWidget::generateControlPoints()
{
	if (precision < 2 || degU < 2 || degV < 2)
		return;
	ptsControl.clear();
	ptsHighlighted.clear();
	ptsControl.resize(degU);
	int x, y, z, zx, zy;
	switch (modeGenPts)
	{
	case 1:
		x = (-degU / 2) * randomGeneration(10, 50);
		for (int i = 0; i < degU; i++)
		{
			y = (-degV / 2) * randomGeneration(10, 50);
			for (int j = 0; j < degV; j++)
			{
				//if (j == degV - 1)
					//ptsControl[i].push_back(ptsControl[i][0]);
				//else
				z = randomGeneration(-200, 200);
				ptsControl[i].push_back(QVector3D(x, y, z));
				y += randomGeneration(10, 50);
			}
			x += randomGeneration(10, 50);
		}
		break;
	case 2:
		x = -degU / 2 * 20;
		y = -degV / 2 * 20;
		zx = 0;
		zy = 0;
		for (int i = 0; i < degU; i++)
		{
			for (int j = 0; j < degV; j++)
			{
				ptsControl[i].push_back(QVector3D(x, y, zy));
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
	doRotation(rotationValue);
}

// Vérifier si les points sont déjà générés ou pas, pour éviter les erreurs de mémoires
bool GLWidget::pointsGenerated()
{
	if (precision < 2 || ptsControl.size() < 2 || ptsControl[0].size() < 2)
		return false;
	return true;
}

// Raccordement du patch actuel à un patch aléatoire au même niveau
void GLWidget::Join()
{
	if (!pointsGenerated())
		return;
	int x, y, z;
	int newDegree = degU + degU;
	ptsControl.resize(newDegree);
	ptsHighlighted.clear();
	switch (joinOrder)
	{
	case 0:
		// Raccorder ordre 0
		for (int j = 0; j < degV; j++)
		{
			ptsControl[degU].push_back(ptsControl[degU - 1][j]);
			ptsHighlighted.push_back(ptsControl[degU - 1][j]);
		}
		for (int i = degU + 1; i < newDegree; i++)
			for (int j = 0; j < degV; j++)
			{
				x = ptsControl[i - 1][j].x() + randomGeneration(10, 50);
				y = ptsControl[i - 1][j].y() + randomGeneration(10, 50);
				z = randomGeneration(-200, 200);
				ptsControl[i].push_back(QVector3D(x, y, z));
			}
		break;
	case 1:
		for (int j = 0; j < degV; j++)
		{
			ptsControl[degU].push_back(ptsControl[degU - 1][j]);
			ptsHighlighted.push_back(ptsControl[degU - 1][j]);
		}
		for (int i = degU + 1; i < newDegree; i++)
			for (int j = 0; j < degV; j++)
			{
				x = ptsControl[i - 1][j].x() + randomGeneration(10, 50);
				y = ptsControl[i - 1][j].y() + randomGeneration(10, 50);
				z = randomGeneration(-200, 200);
				ptsControl[i].push_back(QVector3D(x, y, z));
			}
		break;
	case 2:
		break;
	default:
		break;
	}
	doRotation(rotationValue);
}

void GLWidget::cancelJoin()
{
	ptsControl.resize(degU);
	ptsHighlighted.clear();
	doRotation(rotationValue);
}

void GLWidget::doRotation(QVector3D rot)
{
	rotationValue = rot;
	if (!pointsGenerated())
		return;
	if (rot.lengthSquared() != 0)
	{
		Quaternion quat = Quaternion::fromEulerAngles(rot);
		//QQuaternion quat = QQuaternion::fromEulerAngles(rot);
		for (int i = 0; i < degU; i++)
			for (int j = 0; j < degV; j++)
				ptsControl[i][j] = quat.rotatedVector(ptsControl[i][j]);
	}
	generateSurfaceBezier();
}

void GLWidget::generateSurfaceBezier()
{
	if (!pointsGenerated())
		return;
	ptsBezier.clear();
	QElapsedTimer timer;
	timer.start();
	ptsBezier = calcSurfaceBezier(ptsControl, precision);
	int time = timer.nsecsElapsed() / 1000;
	labelTimer = QString::number(time) + " us";
	emit labelChanged();
}

void GLWidget::LoadGLTextures(const char * name)
{
	QImage img;

	if (!img.load(name)) {
		std::cerr << "ERROR in loading image" << std::endl;
	}

	QImage t = QGLWidget::convertToGLFormat(img);

	glGenTextures(1, &texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, t.width(), t.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, t.bits());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GLWidget::drawSurfaceBezier()
{
	glPointSize(3);
	float t[] = { 300,200,200 };
	glBegin(GL_POINTS);
		glVertex3fv(t);
	glEnd();
	
	if (showTexture) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
	}

	if (!pointsGenerated())
		return;
	for (int i = 0; i < precision; i++)
	{
		for (int j = 0; j < precision; j++)
		{
			GLfloat tempVector3D[3];
			if (showWireframe)
			{
				float white[] = { 1.0, 1.0, 1.0 };
				glColor3fv(white);
				glBegin(GL_LINE_LOOP);
			}
			else
			{
				QVector3D red = { 1.0f, 0.6f, 0.6f};
				QVector3D light1 = { 0.0f, 0.f, 0.f };
				QVector3D light2 = { 0.0f, 0.f, 0.f };
				if (showLight1)
					light1 = processLighting(ptsBezier[i][j], ptsBezier[i + 1][j], ptsBezier[i][j + 1], ptsBezier[i + 1][j + 1], lights[0]);
				if (showLight2)
					light2 = processLighting(ptsBezier[i][j], ptsBezier[i + 1][j], ptsBezier[i][j + 1], ptsBezier[i + 1][j + 1], lights[1]);
				if (showLight1 || showLight2)
					glVector3D(light1 + light2, false);
				else
					glVector3D(red, false);
				glBegin(GL_TRIANGLES);
			}

			// Coordonnées des points avec coordonnées de la texture
			if (showTexture) {
				glTexCoord2f(0.0f, 0.0f);  glVector3D(ptsBezier[i][j], true);
				glTexCoord2f(1.0f, 1.0f);  glVector3D(ptsBezier[i + 1][j + 1], true);
				glTexCoord2f(1.0f, 0.0f);  glVector3D(ptsBezier[i + 1][j], true);
				glTexCoord2f(0.0f, 0.0f);  glVector3D(ptsBezier[i][j], true);
				glTexCoord2f(0.0f, 1.0f);  glVector3D(ptsBezier[i][j + 1], true);
				glTexCoord2f(1.0f, 1.0f);  glVector3D(ptsBezier[i + 1][j + 1], true);
			}
			else
			{
				glVector3D(ptsBezier[i][j], true);
				glVector3D(ptsBezier[i + 1][j + 1], true);
				glVector3D(ptsBezier[i + 1][j], true);
				glVector3D(ptsBezier[i][j], true);
				glVector3D(ptsBezier[i][j + 1], true);
				glVector3D(ptsBezier[i + 1][j + 1], true);
			}

			glEnd();
		}
	}

	glDisable(GL_TEXTURE_2D);
}

QVector3D GLWidget::processLighting(QVector3D p1Face, QVector3D p2Face, QVector3D p3Face, QVector3D p4Face, Light light) 
{
	//glColor3fv(iAmbiant*kAmbiant);

	QVector3D u = p2Face - p1Face;
	QVector3D v = p4Face - p1Face;
	QVector3D normal = QVector3D::crossProduct(u, v).normalized();

	/*glColor3fv(red);
	glBegin(GL_LINES);
	glVertex3f(surfBezier[i][j].x(), surfBezier[i][j].y(), surfBezier[i][j].z());
	glVertex3f(normal.x()*100 - surfBezier[i][j].x(), normal.y()*100 - surfBezier[i][j].y(), normal.z()*100 - surfBezier[i][j].z());
	glEnd();*/

	QVector3D dir = light.posLight - p1Face;
	float cosAngle = QVector3D::dotProduct(normal, dir) / (normal.length() * dir.length());
	cosAngle = (cosAngle <= 0) ? 0 : cosAngle;
	QVector3D R = dir - 2 * normal*(QVector3D::dotProduct(normal, dir));
	float ns = QVector3D::dotProduct(R, p1Face - QVector3D(200, 200, 200)) / (R.length() * (p1Face - QVector3D(200, 200, 200)).length());

	ns = (ns <= 0 || cosAngle <= 0) ? 0 : ns;
	ns = pow(ns, 32);

	/*float red[] = { 1,0,0 };
	glColor3fv(red);
	glBegin(GL_LINES);
	glVertex3f(p1Face.x(), p1Face.y(), p1Face.z());
	glVertex3fv(t);
	glEnd();

	float ble[] = { 0,0,1 };
	glColor3fv(ble);
	glBegin(GL_LINES);
	glVertex3f(p1Face.x(), p1Face.y(), p1Face.z());
	glVertex3f(p1Face.x() - R.x() * 100, p1Face.y() - R.y() * 100, p1Face.z() - R.z() * 100 );

	glEnd();*/

	float ior = 1.517f;
	QVector3D refraction;
	float k = 1.0 - ior * ior * (1.0 - QVector3D::dotProduct(normal, dir) * QVector3D::dotProduct(normal, dir));
	if (k < 0.0)
		R = { 0,0,0 };
	else
		R = ior * dir - (ior * QVector3D::dotProduct(normal, dir) + sqrt(k)) * normal;

	QVector3D l = light.iAmbiant * kAmbiant * objectColor;

	if (showLightDiffuse)
		l += light.iDiffuse * objectColor * kDiffuse * cosAngle;
	if (showLightSpecular)
		l += light.iDiffuse * kSpecular * ns;

	return l;
}

// Dessiner des points
void GLWidget::drawPoints(vector<QVector3D> pts, QVector3D color, int ptSize)
{
	int nbPoints = pts.size();
	if (nbPoints == 0)
		return;
	glVector3D(color, false);
	glPointSize(ptSize);
	glBegin(GL_POINTS);
	for (int i = 0; i < nbPoints; i++)
		glVector3D(pts[i], true);
	glEnd();
}

// Dessiner des points
void GLWidget::drawPointsMatrix(vector<vector<QVector3D>> pts, QVector3D color, int ptSize)
{
	if (pts.size() == 0 || pts[0].size() == 0)
		return;
	int m = pts.size();
	int n = pts[0].size();
	glVector3D(color, false);
	glPointSize(ptSize);
	glBegin(GL_POINTS);
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			glVector3D(pts[i][j], true);
	glEnd();
}

// Conversion de coordonnées d'écran à coordonnées de la scène OPENGL
QVector3D GLWidget::convertXY(int X, int Y)
{
	return QVector3D((int)((float)X * 2.0 * range * m_aspectRatio / screenW - range * m_aspectRatio), (int)((float)Y * 2.0 * range / screenH - range), 0);
}

// Callback pour le mouvement de la souris
void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
	mouse = event->pos();
	mouseWorld = convertXY(mouse.x(), mouse.y());
	emit mouseMoved();
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
	ptsControl.clear();
	ptsHighlighted.clear();
	ptsBezier.clear();
	degU = 0;
	degV = 0;
}