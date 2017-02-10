#pragma once

#include <vector>
#include <algorithm>
#include <random>
#include "Quaternion.h"

using namespace std;

// Fonction intermédiaire pour l'algo de Casteljau
QVector3D getCasteljauPoint(vector<QVector3D> poly, double t);
// Calculer la surface de Bézier à partir d'un ensemble de points de contrôle
vector<vector<QVector3D>> calcSurfaceBezier(vector<vector<QVector3D>> pts, int precision);
// Génération aléatoire d'un nombre entier dans l'intervalle donnée
int randomGeneration(int min, int max);
// Convertir la classe QVector3D pour utiliser avec les fonctions OpenGL
void glVector3D(QVector3D p, bool vertex);
// Convertir les couleurs entre valeurs entières et normalisées
QColor convertColor(QVector3D col);