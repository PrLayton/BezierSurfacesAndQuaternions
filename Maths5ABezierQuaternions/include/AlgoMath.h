#pragma once

#include <vector>
#include <algorithm>
#include <random>
#include "Quaternion.h"

using namespace std;

QVector3D getCasteljauPoint(vector<QVector3D> poly, double t);
vector<vector<QVector3D>> calcSurfaceBezier(vector<vector<QVector3D>> pts, int precision);
int randomGeneration(int min, int max);
void glVector3D(QVector3D p, bool vertex);
QColor convertColor(QVector3D col);
void clearMatrixPoints(vector<vector<QVector3D>> &pts);