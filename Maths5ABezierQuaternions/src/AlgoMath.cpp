#include "stdafx.h"
#include "AlgoMath.h"

void resetGlobalID()
{
	globalId = 0;
}

// ------------------------------------------ Jarvis -------------------------------------------------------------
vector<Point> EnvelopeJarvis(vector<Point> pts)
{
	vector<Point> poly;
	if (pts.size() <= 0)
		return poly;
	
	// Supprimer les points confondus dans la liste
	vector<int> id;
	for (int i = 0; i < pts.size() - 1; i++)
		for (int j = i + 1; j < pts.size(); j++)
			if (pts[j].coord == pts[i].coord)
			{
				id.push_back(i);
				break;
			}
	for each (int i in id)
		pts.erase(pts.begin() + i);

	// Algo de Jarvis
	QVector3D minPoint = pts[0].coord;
	int i0 = 0;
	int N = pts.size();
	for (int i = 1; i < N; i++)
	{
		if (pts[i].coord.x() < minPoint.x() || (pts[i].coord.x() == minPoint.x() && pts[i].coord.y() < minPoint.y()))
		{
			minPoint = pts[i].coord;
			i0 = i;
		}
	}

	QLineF v(0, 0, 0, -1);
	int i = i0;
	do
	{
		Point Pi = pts[i];
		poly.push_back(Pi);
		if (N <= 1)
			break;
		// recherche du point suivant
		// initialisation de angle_min et lmax avec le premier point d'indice différent de i
		int j = (i == 0) ? 1 : 0;
		QLineF PiPj(Pi.coord.x(), Pi.coord.y(), pts[j].coord.x(), pts[j].coord.y());
		qreal angle_min = v.angleTo(PiPj);
		float lmax = Pi.coord.distanceToPoint(pts[j].coord);
		int inew = j;
		// recherche du point le plus proche
		for (j = inew + 1; j < N; j++)
		{
			if (j != i)
			{
				PiPj = QLineF(Pi.coord.x(), Pi.coord.y(), pts[j].coord.x(), pts[j].coord.y());
				qreal angle = v.angleTo(PiPj);
				float l = Pi.coord.distanceToPoint(pts[j].coord);
				if (angle_min > angle || (angle_min == angle && lmax < l))
				{
					angle_min = angle;
					lmax = l;
					inew = j;
				}
			}
		}
		// mise à jour du pivot et du vecteur directeur
		v = QLineF(pts[i].coord.x(), pts[i].coord.y(), pts[inew].coord.x(), pts[inew].coord.y());
		i = inew;
	} while (i != i0);
	return poly;
}