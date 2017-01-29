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

public slots:
	void timeOutSlot();
	void setDegreeU(int m) { degU = m; generateControlPoints(); }
	void setDegreeV(int n) { degV = n; generateControlPoints(); }
	void setPrecision(int p) { precision = p; generateSurfaceBezier(); }
	// Fonctions pour mettre à jour les paramètres de l'UI
	void setGrid(int g) { showGrid = g == 0 ? false : true; }
	void setWireframe(int e) { showWireframe = e == 0 ? false : true; }
	// Réinitialiser les données
	void resetData();
	// Réinitialiser le caméra au paramètres par défaut
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

	// Les paramètres de caméra OPENGL
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

	QVector3D iAmbiant;
	QVector3D iDiffuse;
	float kAmbiant;
	float kDiffuse;

	QOpenGLShaderProgram m_shader;
	QMatrix4x4 m_modelMatrix;
	QMatrix4x4 m_viewMatrix;
	QMatrix4x4 m_projectionMatrix;

	// Les données
	vector<QVector3D> points;
	vector<vector<QVector3D>> bezier, surfBezier;
	int pointSelected = -1;

	// Les paramètres de l'UI
	int modeGenPts = 1;		// 1 pour Aléatoire, 2 pour réglage de l'hauteur
	int degU = 0;
	int degV = 0;
	int precision = 10;
	int depthBetweenPoints = 0;
	bool needUpdate = false;
	bool showWireframe = false;
	bool showGrid = false;

	struct ScenePoint 
	{
		QVector3D coords;
		QVector3D normal;
		ScenePoint() {};
		//ScenePoint(const QVector3D &c, const QVector3D &n);
	};
	QVector<ScenePoint> m_data;

	float dot(QVector3D p, QVector3D op) { return p.x()*op.x() + p.y()*op.y() + p.z()*op.z(); } // Produit scalaire de 2 points
	float norme(QVector3D p) { return float(sqrt(p.x()*p.x() + p.y()*p.y() + p.z()*p.z())); } // Norme du point (longueur du vecteur.)
	//T angle(Point<T> p) { return acos(static_cast<T>(dot(p) / (norme() * p.norme()))); } // Return l'angle non orienté formé par les 2 vecteurs.
	QVector3D crossProduct(QVector3D p, QVector3D op) { QVector3D t(p.y()*op.z() - p.z()*op.y(), p.z()*p.x() - p.x()*op.z(), p.x()*op.y() - p.y()*op.x()); return t; } //Produit vectoriel de 2 points.
	QVector3D normalize(QVector3D p) { float n = norme(p); return QVector3D(p.x() / n, p.y() / n, p.z() / n); } //Normalisation du point.
	QVector3D crossProductNormalized(QVector3D p, QVector3D op)
	{
		QVector3D final;
		final = crossProduct(p, op);
		final = normalize(final);
		return final;
	}
};