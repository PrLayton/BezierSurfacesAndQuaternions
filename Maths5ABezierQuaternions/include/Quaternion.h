#pragma once

#include <math.h>
#include <iostream>

#define PI 3.14159265358979

class Quaternion
{
    public:
        // Constructeurs
        Quaternion();
        Quaternion(float w, float x, float y, float z);
		static Quaternion Identity();
		static Quaternion fromVector3D(QVector3D);
		static Quaternion fromEulerAngles(QVector3D);
		static Quaternion fromMatrix(float*);
		QVector3D toVector3D();
        float* getRotateMatrix();
		float getW() { return m_w; }
		float getX() { return m_x; }
        float getY() { return m_y; }
        float getZ() { return m_z; }
		void setW(float w) { m_w = w; }
		void setX(float x) { m_x = x; }
		void setY(float y) { m_y = y; }
		void setZ(float z) { m_z = z; }
		float length();
        void normalized();
        void conjugate();
        void operator+=(Quaternion const&);
        void operator*=(Quaternion const&);
		void operator/=(float);
		QVector3D rotatedVector(const QVector3D &v);

    //attributs
    private:
		float m_w;
		float m_x;
        float m_y;
        float m_z;
};

Quaternion operator+(Quaternion const&, Quaternion const&);
Quaternion operator*(Quaternion const&, Quaternion const&);