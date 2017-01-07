#pragma once

#include <vector>
#include <algorithm>

using namespace std;

static int globalId = 0;

struct Point
{
	int id;
	QVector3D coord;
	Point() 
	{
		id = -1;
	};

	Point(QVector3D pt)
	{
		coord = pt;
		id = globalId++;
	}
};

void resetGlobalID();
vector<Point> EnvelopeJarvis(vector<Point> pts);