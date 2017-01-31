#pragma once

#include <vector>
#include <algorithm>
#include <random>

using namespace std;

QVector3D getCasteljauPoint(vector<QVector3D> poly, double t);
vector<vector<QVector3D>> calcSurfaceBezier(vector<vector<QVector3D>> pts, int precision);
int randomGeneration(int min, int max);
GLfloat* convertVector3D(QVector3D p);
QColor convertColor(QVector3D col);