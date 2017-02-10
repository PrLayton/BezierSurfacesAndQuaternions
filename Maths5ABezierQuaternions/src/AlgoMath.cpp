#include "stdafx.h"
#include "AlgoMath.h"

// Fonction intermédiaire pour l'algo de Casteljau
QVector3D getCasteljauPoint(vector<QVector3D> poly, double t)
{
	int N = poly.size();
	for (int k = 1; k < N; k++)
		for (int i = 0; i < N - k; i++)
			poly[i] = (1 - t)*poly[i] + t*poly[i + 1];
	return poly[0];
}

// Calculer la surface de Bézier à partir d'un ensemble de points de contrôle
vector<vector<QVector3D>> calcSurfaceBezier(vector<vector<QVector3D>> pts, int precision)
{
	vector<vector<QVector3D>> curvePoints;
	if (pts.size() > 0 && pts[0].size() > 0 && precision > 0)
	{
		int degU = pts.size();
		curvePoints.resize(precision+1);
		for (int ui = 0; ui <= precision; ui++) 
		{
			float u = (float)ui / (float)precision;
			for (int vi = 0; vi <= precision; vi++) 
			{
				float v = (float)vi / (float)precision;
				vector<QVector3D> qPoints;
				for (int i = 0; i < degU; i++)
					qPoints.push_back(getCasteljauPoint(pts[i], v));
				curvePoints[ui].push_back(getCasteljauPoint(qPoints, u));
			}
		}
	}
	return curvePoints;
}

// Génération aléatoire d'un nombre entier dans l'intervalle donnée
int randomGeneration(int min, int max)
{
	// the random device that will seed the generator
	std::random_device seeder;
	// make a mersenne twister engine
	std::mt19937 engine(seeder());
	// the distribution
	std::uniform_int_distribution<int> dist(min, max);
	// generate the integer
	return dist(engine);
}

// Convertir la classe QVector3D pour utiliser avec les fonctions OpenGL
void glVector3D(QVector3D p, bool vertex)
{
	GLfloat *temp = new GLfloat[3];
	temp[0] = p.x();
	temp[1] = p.y();
	temp[2] = p.z();
	if (vertex)
		glVertex3fv(temp);
	else
		glColor3fv(temp);
	delete[] temp;
}

// Convertir les couleurs entre valeurs entières et normalisées
QColor convertColor(QVector3D col)
{
	int r = (int)(col.x() * 255);
	int g = (int)(col.y() * 255);
	int b = (int)(col.z() * 255);
	return QColor(r, g, b);
}