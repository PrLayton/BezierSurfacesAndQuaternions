#pragma once

#include <vector>
#include <algorithm>
#include <random>

using namespace std;

vector<QVector3D> calculateCasteljau(vector<QVector3D> poly, int nbPas);
QVector3D getCasteljauPoint(vector<QVector3D> poly, double t);
vector<vector<QVector3D>> calcSurfaceBezier(vector<vector<QVector3D>> pts, int precision);
int randomGeneration(int min, int max);

vector<QVector3D> EnvelopeJarvis(vector<QVector3D> pts);