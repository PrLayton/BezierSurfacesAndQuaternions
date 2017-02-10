#include "stdafx.h"
#include "GLWidget.h"
#include <GL/glu.h>

// Initialisation de la scène OpenGL
GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent), m_theta(180.0f), m_phi(0.0f), m_aspectRatio(1.0)
{
	int seconde = 1000; // 1 seconde = 1000 ms
	int timerInterval = seconde / 60;
	t_Timer = new QTimer(this);
	connect(t_Timer, SIGNAL(timeout()), this, SLOT(timeOutSlot()));
	t_Timer->start(timerInterval);
	setMouseTracking(true);
	setFocusPolicy(Qt::StrongFocus);

	// Scale de la scène (zoom)
	m_scale = 1;
	m_incrementScale = 1;
	// Position des deux lumières et couleurs ambiant et diffuse
	lights[0].posLight = { 0, 0, 300 };
	lights[0].iAmbiant = { 1.0,1.0,1.0 };
	lights[0].iDiffuse = { 1.0,1.0,1.0 };
	lights[1].posLight = { -100, 150, 150 };
	lights[1].iAmbiant = { 1.0,0.0,1.0 };
	lights[1].iDiffuse = { 1.0,0.0,1.0 };
}

// A la suppression du programme
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
	// Initialisation des proprietes OpenGL du programme
	glClearColor(0, 0, 0, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Angle de la camera
	range = 100.0;

	// Chargement de la texture
	LoadGLTextures("grass1.jpg");
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

	// Définition de la matrice orthogonale de la camera
	m_aspectRatio = double(width) / double(height);
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
	// Matrice de dessin de la camera
	m_modelMatrix.setToIdentity();
	m_modelMatrix.rotate(45, 0, 1, 0);
	QMatrix4x4 modelViewMatrix = m_viewMatrix*m_modelMatrix;

	// Nettoyage des buffers
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
	// Afficher la grille et les Axes dans la scène
	if (showGrid)
		drawGridandAxes();

	// Dessin de la surface de Bezier
	drawSurfaceBezier();

	// On passe la posiiton des lights
	vector<QVector3D> ptlight = { lights[0].posLight };
	vector<QVector3D> ptlight2 = { lights[1].posLight };
	// Dessin de la représentation des lights (points)
	drawPoints(ptlight, lights[0].iAmbiant, 20);
	drawPoints(ptlight2, lights[1].iAmbiant, 20);

	// Affichage des points de controle et des vertex du patch
	if (showPts)
	{
		drawPointsMatrix(ptsControl, QVector3D(1.0, 0, 0), POINT_SIZE);
		drawPointsMatrix(ptsBezier, QVector3D(0, 1.0, 0), 5);
	}
	// Surbriller les points de raccordement
	drawPoints(ptsHighlighted, QVector3D(0, 0, 1.0), 10);
}

// Generation des points de controle
void GLWidget::generateControlPoints()
{
	// Si l'utilisateur n'a pas changé les valeurs minimales
	if (precision < 2 || degU < 2 || degV < 2)
		return;

	// Sinon, on efface les valeurs actuels des taleaux
	ptsControl.clear();
	ptsHighlighted.clear();

	ptsControl.resize(degU);
	int x, y, z, zx, zy;

	// On fonction du style de genration de points que l'on souhaite obtenir
	switch (modeGenPts)
	{
		// Cas où l'on souhaite un placement aléatoire
	case 1:
		x = (-degU / 2) * randomGeneration(10, 50);
		for (int i = 0; i < degU; i++)
		{
			y = (-degV / 2) * randomGeneration(10, 50);
			for (int j = 0; j < degV; j++)
			{
				z = randomGeneration(-200, 200);
				ptsControl[i].push_back(QVector3D(x, y, z));
				y += randomGeneration(10, 50);
			}
			x += randomGeneration(10, 50);
		}
		break;
		// Cas où l'on souhaite un placement régulier
	case 2:
		x = -degU / 2 * 20;
		y = -degV / 2 * 20;
		zx = 0;
		zy = 0;
		// Pour chacune des valeurs de nos axes
		for (int i = 0; i < degU; i++)
		{
			for (int j = 0; j < degV; j++)
			{
				// On rajoute le point au tableau de points de controle
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

	// On effectue une rotation de nos points de controle
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
	// Vérifier si les points sont déja generés ou pas
	if (!pointsGenerated())
		return;

	ptsJoin.clear();
	ptsJoin.resize(degU);
	ptsHighlighted.clear();

	int x, y, z;

	// En fonction du type de raccordement que l'on veut avoir
	switch (joinOrder)
	{
	// Raccordement d'ordre C0
	case 0:
		for (int j = 0; j < degV; j++)
		{
			// Q0 = Pn
			ptsJoin[0].push_back(ptsControl[degU - 1][j]);
			// Stocker les points concernés pour surbriller
			ptsHighlighted.push_back(ptsControl[degU - 1][j]);
		}
		
		x = ptsJoin[0][0].x() + randomGeneration(10, 50);
		for (int i = 1; i < degU; i++)
		{
			y = ptsJoin[i - 1][0].y() + randomGeneration(10, 50);
			for (int j = 0; j < degV; j++)
			{
				z = randomGeneration(-100, 100);
				ptsJoin[i].push_back(QVector3D(x, y, z));
				y += randomGeneration(10, 50);
			}
			x += randomGeneration(10, 50);
		}
		break;
	// Raccordement d'ordre C1
	case 1:
		for (int j = 0; j < degV; j++)
		{
			// Q0 = Pn
			ptsJoin[0].push_back(ptsControl[degU - 1][j]);
			// Q1 = 2*Pn - P(n-1)
			QVector3D Q1 = 2 * ptsControl[degU - 1][j] - ptsControl[degU - 2][j];
			ptsJoin[1].push_back(Q1);
			// Stocker les points concernés pour surbriller
			ptsHighlighted.push_back(ptsControl[degU - 2][j]);
			ptsHighlighted.push_back(ptsControl[degU - 1][j]);
			ptsHighlighted.push_back(Q1);
		}

		x = ptsJoin[1][0].x() + randomGeneration(10, 50);
		for (int i = 2; i < degU; i++)
		{
			y = ptsJoin[i - 1][0].y() + randomGeneration(10, 50);
			for (int j = 0; j < degV; j++)
			{
				z = randomGeneration(-100, 100);
				ptsJoin[i].push_back(QVector3D(x, y, z));
				y += randomGeneration(10, 50);
			}
			x += randomGeneration(10, 50);
		}
		break;
	// Raccordement d'ordre C2
	case 2:
		// Vérifier si le degré initial est suffisant pour le raccordement C2
		if (degU < 3)
			break;
		for (int j = 0; j < degV; j++)
		{
			// Q0 = Pn
			ptsJoin[0].push_back(ptsControl[degU - 1][j]);
			// Q1 = 2*Pn - P(n-1)
			QVector3D Q1 = 2 * ptsControl[degU - 1][j] - ptsControl[degU - 2][j];
			ptsJoin[1].push_back(Q1);
			// Q2 = P(n-2) + 2*(Q1 - P(n-1))
			QVector3D Q2 = ptsControl[degU - 3][j] + 2 * (Q1 - ptsControl[degU - 2][j]);
			ptsJoin[2].push_back(Q2);
			// Stocker les points concernés pour surbriller
			ptsHighlighted.push_back(ptsControl[degU - 3][j]);
			ptsHighlighted.push_back(ptsControl[degU - 2][j]);
			ptsHighlighted.push_back(ptsControl[degU - 1][j]);
			ptsHighlighted.push_back(Q1);
			ptsHighlighted.push_back(Q2);
		}

		x = ptsJoin[2][0].x() + randomGeneration(10, 50);
		for (int i = 3; i < degU; i++)
		{
			y = ptsJoin[i - 1][0].y() + randomGeneration(10, 50);
			for (int j = 0; j < degV; j++)
			{
				z = randomGeneration(-100, 100);
				ptsJoin[i].push_back(QVector3D(x, y, z));
				y += randomGeneration(10, 50);
			}
			x += randomGeneration(10, 50);
		}
		break;
	default:
		break;
	}

	// Recacul des points de la surface
	ptsBezierJoin.clear();
	ptsBezierJoin = calcSurfaceBezier(ptsJoin, precision);
	ptsBezier.resize(precision + 1);

	// Mise à jour des points
	for (int i = 0; i < ptsBezierJoin.size(); i++)
		ptsBezier.push_back(ptsBezierJoin[i]);
}

// Annulation du raccordement
void GLWidget::cancelJoin()
{
	ptsControl.resize(degU);
	ptsHighlighted.clear();
	doRotation(rotationValue);
}

// Rotation du patcj
void GLWidget::doRotation(QVector3D rot)
{
	rotationValue = rot;
	if (!pointsGenerated())
		return;
	if (rot.lengthSquared() != 0)
	{
		Quaternion quat = Quaternion::fromEulerAngles(rot);
		for (int i = 0; i < degU; i++)
			for (int j = 0; j < degV; j++)
				ptsControl[i][j] = quat.rotatedVector(ptsControl[i][j]);
	}
	generateSurfaceBezier();
}

// Generation de la surface de Bezier
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
	for (int i = 0; i < ptsBezier.size() - 1; i++)
	{
		for (int j = 0; j < ptsBezier[0].size() - 1; j++)
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