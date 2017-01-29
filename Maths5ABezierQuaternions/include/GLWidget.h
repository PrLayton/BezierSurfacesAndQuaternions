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
	void doRotation(QVector3D rot);
	QString labelTimer;
	QPoint mouse;
	QVector3D mouseWorld;

protected:
	// Les événements Qt de souris et du clavier
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent * event);
	virtual void keyPressEvent(QKeyEvent* e);

signals:
	// Signal Qt pour mettre à jour les labels de Timers
	void labelChanged();
	void mouseMoved();
	void rotationReset();

public slots:
	void timeOutSlot();
	// Fonctions pour mettre à jour les paramètres de l'UI
	void setDegreeU(int m) { degU = m; generateControlPoints(); }
	void setDegreeV(int n) { degV = n; generateControlPoints(); }
	void setPrecision(int p) { precision = p; generateSurfaceBezier(); }
	void setGrid(int g) { showGrid = g == 0 ? false : true; }
	void setShowPts(int s) { showPts = s == 0 ? false : true; }
	void setWireframe(int e) { showWireframe = e == 0 ? false : true; }
	// Réinitialiser les données
	void resetData();
	// Réinitialiser le caméra à la vue par défaut
	void resetCamera();

private:
	// Fonction rendu de la scène
	void drawScene(QMatrix4x4 mvMatrix);
	// Conversion de coordonnées d'écran à coordonnées de la scène OPENGL
	QVector3D convertXY(int X, int Y);
	// Chercher du point (dans la nuage existante) la plus proche de la souris
	int findNearestPoint(vector<QVector3D> pts, QPoint p);
	// Dessiner la grille et les axes
	void drawGridandAxes();
	// Dessiner des points
	void drawPoints(vector<QVector3D> points, QVector3D color, int ptSize);
	void drawPointsMatrix(vector<vector<QVector3D>> pts, QVector3D color, int ptSize);
	// BEZIER
	void generateControlPoints();
	void generateSurfaceBezier();
	void drawSurfaceBezier();

	// Paramètres de caméra OPENGL
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
	QOpenGLShaderProgram m_shader;
	QMatrix4x4 m_modelMatrix;
	QMatrix4x4 m_viewMatrix;
	QMatrix4x4 m_projectionMatrix;

	// Paramètres des lumières
	QVector3D posLight = { 0, 0, 200 };
	QVector3D iAmbiant = { 1.0,1.0,1.0 };
	QVector3D iDiffuse = { 1.0,1.0,1.0 }; 
	float kAmbiant = 0.2;
	float kDiffuse = 0.2;
	float kSpecular = 0.5;
	QVector3D objectColor = { 1.0,0.5,0.5 };


	// Les données
	vector<QVector3D> points;
	vector<vector<QVector3D>> ptsControl, ptsRotated, ptsBezier;
	QVector3D rotationValue = QVector3D(0, 0, 0);
	int pointSelected = -1;

	// Les paramètres de l'UI
	int modeGenPts = 1;		// 1 pour Aléatoire, 2 pour réglage de l'hauteur
	int degU = 0;
	int degV = 0;
	int precision = 10;
	int depthBetweenPoints = 0;
	bool needUpdate = false;
	bool showWireframe = false;
	bool showPts = false;
	bool showGrid = false;

	struct ScenePoint 
	{
		QVector3D coords;
		QVector3D normal;
		ScenePoint() {};
		//ScenePoint(const QVector3D &c, const QVector3D &n);
	};
	QVector<ScenePoint> m_data;
	QTimer *t_Timer;
};