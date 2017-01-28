#include "stdafx.h"
#include "AlgoMath.h"

// Génération de la courbe de Bézier par l'algorithme de Casteljau
vector<QVector3D> calculateCasteljau(vector<QVector3D> poly, int nbPas)
{
	vector<QVector3D> bezier;
	if (poly.size() > 0)
	{
		double pas = 1 / nbPas;
		for (double t = 0; t <= 1; t += pas)
			bezier.push_back(getCasteljauPoint(poly, t));
	}
	return bezier;
}

// Fonction intermédiaire pour l'algo de Casteljau
QVector3D getCasteljauPoint(vector<QVector3D> poly, double t)
{
	int N = poly.size();
	for (int k = 1; k < N; k++)
		for (int i = 0; i < N - k; i++)
			poly[i] = (1 - t)*poly[i] + t*poly[i + 1];
	return poly[0];
}

// Fonction intermédiaire pour l'algo de Casteljau
vector<vector<QVector3D>> calcSurfaceBezier(vector<vector<QVector3D>> pts, int precision)
{
	vector<vector<QVector3D>> curvePoints;
	if (pts.size() > 0 && pts[0].size() > 0 && precision > 0)
	{
		int nbU = pts.size();
		int nbV = pts[0].size();
		curvePoints.resize(precision);
		for (int ui = 0; ui < precision; ui++) 
		{
			float u = (float)ui / (float)precision;
			curvePoints[ui].resize(precision);
			for (int vi = 0; vi < precision; vi++) 
			{
				float v = (float)vi / (float)precision;
				vector<QVector3D> qPoints;
				qPoints.resize(nbU);
				for (int i = 0; i < nbU; i++) 
					qPoints[i] = getCasteljauPoint(pts[i], v);
				curvePoints[ui][vi] = getCasteljauPoint(qPoints, u);
			}
		}
	}
	return curvePoints;
}

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

// ------------------------------------------ Jarvis -------------------------------------------------------------
vector<QVector3D> EnvelopeJarvis(vector<QVector3D> pts)
{
	vector<QVector3D> poly;
	if (pts.size() <= 0)
		return poly;
	
	// Supprimer les points confondus dans la liste
	vector<int> id;
	for (int i = 0; i < pts.size() - 1; i++)
		for (int j = i + 1; j < pts.size(); j++)
			if (pts[j] == pts[i])
			{
				id.push_back(i);
				break;
			}
	for each (int i in id)
		pts.erase(pts.begin() + i);

	// Algo de Jarvis
	QVector3D minPoint = pts[0];
	int i0 = 0;
	int N = pts.size();
	for (int i = 1; i < N; i++)
	{
		if (pts[i].x() < minPoint.x() || (pts[i].x() == minPoint.x() && pts[i].y() < minPoint.y()))
		{
			minPoint = pts[i];
			i0 = i;
		}
	}

	QLineF v(0, 0, 0, -1);
	int i = i0;
	do
	{
		QVector3D Pi = pts[i];
		poly.push_back(Pi);
		if (N <= 1)
			break;
		// recherche du point suivant
		// initialisation de angle_min et lmax avec le premier point d'indice différent de i
		int j = (i == 0) ? 1 : 0;
		QLineF PiPj(Pi.x(), Pi.y(), pts[j].x(), pts[j].y());
		qreal angle_min = v.angleTo(PiPj);
		float lmax = Pi.distanceToPoint(pts[j]);
		int inew = j;
		// recherche du point le plus proche
		for (j = inew + 1; j < N; j++)
		{
			if (j != i)
			{
				PiPj = QLineF(Pi.x(), Pi.y(), pts[j].x(), pts[j].y());
				qreal angle = v.angleTo(PiPj);
				float l = Pi.distanceToPoint(pts[j]);
				if (angle_min > angle || (angle_min == angle && lmax < l))
				{
					angle_min = angle;
					lmax = l;
					inew = j;
				}
			}
		}
		// mise à jour du pivot et du vecteur directeur
		v = QLineF(pts[i].x(), pts[i].y(), pts[inew].x(), pts[inew].y());
		i = inew;
	} while (i != i0);
	return poly;
}