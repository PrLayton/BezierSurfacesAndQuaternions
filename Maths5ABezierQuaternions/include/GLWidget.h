#pragma once
#include <QtOpenGL>
#include <QGLWidget>
#include <QDebug>
#include <QKeyEvent>

#include <QVector2D>
#include <vector>
#include <math.h>
#include <time.h>
#include "GL/glu.h"
#include "AlgoMath.h"

using namespace std;

#define POINT_SIZE 10

class GLWidget : public QOpenGLWidget
{
	Q_OBJECT

public:
	GLWidget(QWidget *parent);
	~GLWidget();
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();
	void changeModeEnvelop(int mode) { modeEnvelop = mode; needUpdate = true; }
	QString labelTimer[5];

protected:
	// Les �v�nements Qt de souris et du clavier
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	virtual void keyPressEvent(QKeyEvent* e);

public slots:
	void timeOutSlot();
	// Fonctions pour mettre � jour les param�tres de l'UI
	void setVoronoi(int f) { showVoronoi = f == 0 ? false : true; }
	void setMovePoint(int m) { movePoint = m == 0 ? false : true; }
	void setGrid(int g) { showGrid = g == 0 ? false : true; }
	void setEnvelop3D(int e) { showEnvelop3D = e == 0 ? false : true; }
	// R�initialiser les donn�es
	void resetData();
	// R�initialiser le cam�ra au param�tres par d�faut
	void resetCamera();

signals:
	// Signal Qt pour mettre � jour les labels de Timers
	void labelChanged(int);

private:
	// Fonction rendu de la sc�ne
	void drawScene();
	// Conversion de coordonn�es d'�cran � coordonn�es de la sc�ne OPENGL
	QVector3D convertXY(int X, int Y);
	// Chercher du point (dans la nuage existante) la plus proche de la souris
	int findNearestPoint(QPoint p);
	// Dessiner la grille et les axes
	void drawGridandAxes();
	// Dessiner des points
	void drawPoints(vector<Point> points, QVector3D color);
	// Dessiner des c�t�s � partir des couples de points
	void drawLinesFromPoints(vector<Point> pts);
	// Dessiner d'un polygone � partir d'un ensemble des points
	void drawPoly(vector<Point> pts, QVector3D color, float width);
	
	QPoint mousePos;
	QTimer *t_Timer;

	// Les param�tres de cam�ra OPENGL
	float m_theta;	// Rotation x-axis
	float m_phi;	// Rotation  y-axis
	double range;
	float m_aspectRatio;
	bool mouseLook;
	QPoint rotValue;
	QPoint tmpMousePos;
	QPoint tmpRotValue;
	int screenW;
	int screenH;

	// Les donn�es
	vector<Point> points;
	int pointSelected = -1;

	// Les param�tres de l'UI
	int modeEnvelop = 1;
	bool needUpdate = false;
	bool showVoronoi = false;
	bool movePoint = false;
	bool showEnvelop3D = false;
	bool showGrid = false;
};