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

// Structure pour la lumi�re
struct Light
{
	QVector3D posLight;
	QVector3D iAmbiant;
	QVector3D iDiffuse;
};

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
	void doRotation(QVector3D rot, bool obj);
	// Les variables publiques pour interagir avec l'UI
	QString labelTimer;
	QPoint mouse;
	QVector3D mouseWorld;
	QVector3D objectColor = { 1.0, 0.6, 0.6 };
	Light lights[2];
	QVector3D rotObj = QVector3D(0, 0, 0);
	QVector3D rotCam = QVector3D(0, 0, 0);
	QVector3D posCam = QVector3D(500, 200, 200);

protected:
	// Les �v�nements Qt de souris et du clavier
	void mouseMoveEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent * event);
	virtual void keyPressEvent(QKeyEvent* e);

signals:
	// Signal Qt pour mettre � jour les labels de Timers
	void labelChanged();
	void mouseMoved();

public slots:
	void timeOutSlot();
	// Fonctions pour mettre � jour les param�tres de l'UI
	void setDegreeU(int m) { degU = m; generateControlPoints(); }
	void setDegreeV(int n) { degV = n; generateControlPoints(); }
	void setPrecision(int p) { precision = p; generateSurfaceBezier(); }
	void setJoinOrder(int j) { joinOrder = j; }
	void setGrid(int g) { showGrid = g == 0 ? false : true; }
	void setShowPts(int s) { showPts = s == 0 ? false : true; }
	void setWireframe(int e) { showWireframe = e == 0 ? false : true; }
	void setShowTexture(int g) { showTexture = g == 0 ? false : true; }
	void setShowLight1(int h) { showLight1 = h == 0 ? false : true; }
	void setShowLight2(int h) { showLight2 = h == 0 ? false : true; }
	void setShowLightDiffuse(int i) { showLightDiffuse = i == 0 ? false : true; }
	void setShowLightSpecular(int i) { showLightSpecular = i == 0 ? false : true; }
	// Cr�er une nouvelle surface pour le raccordement
	void generateJoinPatch();
	// Supprimer la surface de raccordement
	void cancelJoin();
	// R�initialiser les donn�es
	void resetData();
	// R�initialiser le cam�ra � la vue par d�faut
	void resetCamera();

private:
	// Fonction rendu de la sc�ne
	void drawScene(QMatrix4x4 mvMatrix);
	// Conversion de coordonn�es d'�cran � coordonn�es de la sc�ne OPENGL
	QVector3D convertXY(int X, int Y);
	bool pointsGenerated();
	// Dessiner la grille et les axes
	void drawGridandAxes();
	// Dessiner des points
	void drawPoints(vector<QVector3D> points, QVector3D color, int ptSize);
	void drawPointsMatrix(vector<vector<QVector3D>> pts, QVector3D color, int ptSize);
	// BEZIER
	void generateControlPoints();
	void generateSurfaceBezier();
	void drawSurfaceBezier();
	// Textures
	void GLWidget::LoadGLTextures(const char * name);

	// Param�tres de cam�ra OPENGL
	float m_theta;	// Rotation x-axis
	float m_phi;	// Rotation  y-axis
	// Scale de la sc�ne (zoom)
	float m_scale;
	// Pas de l'incrementation du zoom
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

	// Param�tres des lumi�res
	float kAmbiant = 0.3;
	float kDiffuse = 0.4;
	float kSpecular = 0.5;
	//QVector3D objectColor = { 1.0,1.0,1.0 };
	QVector3D processLighting(QVector3D p1Face, QVector3D p2Face, QVector3D p3Face, QVector3D p4Face, Light light);
	// Les donn�es
	vector<QVector3D> ptsHighlighted;
	vector<vector<QVector3D>> ptsControl, ptsJoin, ptsBezier, ptsBezierJoin;
	int pointSelected = -1;
	GLuint texture[1];

	// Les param�tres de l'UI
	int modeGenPts = 2;		// 1 pour Al�atoire, 2 pour r�glage de l'hauteur
	int modeRotation = 0;	// 0 pour Objet, 1 pour Cam�ra
	int degU = 0;
	int degV = 0;
	int precision = 10;
	int joinOrder = 0;		// Raccordement 0-2
	int depthBetweenPoints = 0;
	bool showWireframe = false;
	bool showPts = false;
	bool showGrid = false;
	bool showTexture = false;
	bool showLight1 = false;
	bool showLightDiffuse = false;
	bool showLight2 = false;
	bool showLightSpecular = false;

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