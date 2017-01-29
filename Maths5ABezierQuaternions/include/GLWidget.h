#pragma once
#include <QtOpenGL>
#include <QGLWidget>
#include <QDebug>
#include <QKeyEvent>
#include <QWheelEvent>

#include <QVector2D>
#include <vector>
#include <math.h>
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
	void setModeGeneration(int mode) { modeGenPts = mode; generateControlPoints(); }
	QString labelTimer;
	QPoint mouse;
	QVector3D mouseWorld;

protected:
	// Les �v�nements Qt de souris et du clavier
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent * event);
	virtual void keyPressEvent(QKeyEvent* e);

signals:
	// Signal Qt pour mettre � jour les labels de Timers
	void labelChanged();
	void mouseMoved();

public slots:
	void timeOutSlot();
	void setDegreeU(int m) { degU = m; generateControlPoints(); }
	void setDegreeV(int n) { degV = n; generateControlPoints(); }
	void setPrecision(int p) { precision = p; generateSurfaceBezier(); }
	// Fonctions pour mettre � jour les param�tres de l'UI
	void setGrid(int g) { showGrid = g == 0 ? false : true; }
	void setWireframe(int e) { showWireframe = e == 0 ? false : true; }
	// R�initialiser les donn�es
	void resetData();
	// R�initialiser le cam�ra au param�tres par d�faut
	void resetCamera();

private:
	// Conversion de coordonn�es d'�cran � coordonn�es de la sc�ne OPENGL
	QVector3D convertXY(int X, int Y);
	// Chercher du point (dans la nuage existante) la plus proche de la souris
	int findNearestPoint(vector<QVector3D> pts, QPoint p);
	// Fonction rendu de la sc�ne
	void drawScene();
	// Dessiner la grille et les axes
	void drawGridandAxes();
	// Dessiner des points
	void drawPoints(vector<QVector3D> points, QVector3D color, int ptSize);
	void drawPointsMatrix(vector<vector<QVector3D>> pts, QVector3D color, int ptSize);
	// BEZIER
	void generateControlPoints();
	void generateSurfaceBezier();
	void drawSurfaceBezier();

	// Les param�tres de cam�ra OPENGL
	float m_theta;	// Rotation x-axis
	float m_phi;	// Rotation  y-axis
	float m_scale;
	float m_incrementScale;
	double range;
	float m_aspectRatio;
	bool mouseLook;
	QPoint rotValue;
	QPoint tmpMousePos;
	QPoint tmpRotValue;
	int screenW;
	int screenH;
	QTimer *t_Timer;

	// Les donn�es
	vector<QVector3D> points;
	vector<vector<QVector3D>> bezier, surfBezier;
	int pointSelected = -1;

	// Les param�tres de l'UI
	int modeGenPts = 1;		// 1 pour Al�atoire, 2 pour r�glage de l'hauteur
	int degU = 0;
	int degV = 0;
	int precision = 10;
	int depthBetweenPoints = 0;
	bool needUpdate = false;
	bool showWireframe = false;
	bool showGrid = false;
};