#include "stdafx.h"
#include "Quaternion.h"

// Constructeur de base
Quaternion::Quaternion()
{
	m_w = 0;
	m_x = 0;
	m_y = 0;
	m_z = 0;
}

Quaternion::Quaternion(float w, float x, float y, float z)
{
    //constructor
	m_w = w;
	m_x = x;
	m_y = y;
	m_z = z;
}

Quaternion Quaternion::fromVector3D(QVector3D vecV)
{
    return Quaternion(0, vecV.x(), vecV.y(), vecV.z());
}

Quaternion Quaternion::fromEulerAngles(QVector3D angles)
{
	angles = angles * PI / 180.0f; // convertir degré vers radian
	float cos_z_2 = cosf(0.5*angles.z());
	float cos_y_2 = cosf(0.5*angles.y());
	float cos_x_2 = cosf(0.5*angles.x());

	float sin_z_2 = sinf(0.5*angles.z());
	float sin_y_2 = sinf(0.5*angles.y());
	float sin_x_2 = sinf(0.5*angles.x());

	// and now compute quaternion
	float w = cos_z_2*cos_y_2*cos_x_2 + sin_z_2*sin_y_2*sin_x_2;
	float x = cos_z_2*cos_y_2*sin_x_2 - sin_z_2*sin_y_2*cos_x_2;
	float y = cos_z_2*sin_y_2*cos_x_2 + sin_z_2*cos_y_2*sin_x_2;
	float z = sin_z_2*cos_y_2*cos_x_2 - cos_z_2*sin_y_2*sin_x_2;
	return Quaternion(w, x, y, z);
}

QVector3D Quaternion::toVector3D()
{
    return QVector3D(m_x, m_y, m_z);
}

Quaternion Quaternion::Identity()
{
    return Quaternion(1, 0, 0, 0);
}

float* Quaternion::getRotateMatrix()
{
    //renvoie une matrice de rotation 3*3
	float *rot_mat;
	rot_mat = new float[9];
    rot_mat[0] = 1-2*(m_y*m_y + m_z*m_z);
    rot_mat[1] = 2*(m_x*m_y - m_z*m_w);
    rot_mat[2] = 2*(m_x*m_z + m_y*m_w);
    rot_mat[3] = 2*(m_x*m_y + m_z*m_w);
    rot_mat[4] = 1-2*(m_x*m_x + m_z*m_z);
    rot_mat[5] = 2*(m_y*m_z - m_x*m_w);
    rot_mat[6] = 2*(m_x*m_z - m_y*m_w);
    rot_mat[7] = 2*(m_y*m_z + m_x*m_w);
    rot_mat[8] = 1-2*(m_x*m_x + m_y*m_y);
    return rot_mat;
}

float SIGN(float x) { return (x >= 0.0f) ? +1.0f : -1.0f; }
Quaternion Quaternion::fromMatrix(float *m)
{
	// Quaternion
	float w = (m[0] + m[4] + m[8] + 1.0f) / 4.0f;
	float x = (m[0] - m[4] - m[8] + 1.0f) / 4.0f;
	float y = (-m[0] + m[4] - m[8] + 1.0f) / 4.0f;
	float z = (-m[0] - m[4] + m[8] + 1.0f) / 4.0f;

	w = sqrt(w);
	x = sqrt(x);
	y = sqrt(y);
	z = sqrt(z);

	if (w >= x && w >= y && w >= z)
	{
		w *= +1.0f;
		x *= SIGN(m[7] - m[5]);
		y *= SIGN(m[2] - m[6]);
		z *= SIGN(m[3] - m[1]);
	}
	else if (x >= w && x >= y && x >= z)
	{
		w *= SIGN(m[7] - m[5]);
		x *= +1.0f;
		y *= SIGN(m[3] + m[1]);
		z *= SIGN(m[2] + m[6]);
	}
	else if (y >= w && y >= x && y >= z)
	{
		w *= SIGN(m[2] - m[6]);
		x *= SIGN(m[3] + m[1]);
		y *= +1.0f;
		z *= SIGN(m[7] + m[5]);
	}
	else if (z >= w && z >= x && z >= y)
	{
		w *= SIGN(m[3] - m[1]);
		x *= SIGN(m[6] + m[2]);
		y *= SIGN(m[7] + m[5]);
		z *= +1.0f;
	}
	// Output quaternion
	Quaternion quat(x, y, z, w);
	float r = quat.length();
	quat /= r;
	return quat;
}

float Quaternion::length()
{
	return (float)(sqrt((double)m_x*(double)m_x + (sqrt((double)(m_y*m_y + m_z*m_z + m_w*m_w)))));
}

void Quaternion::normalized()
{
	*this /= length();
}

void Quaternion::conjugate()
{
	m_x = -m_x;
	m_y = -m_y;
	m_z = -m_z;
}

void Quaternion::operator+=(const Quaternion& quat)
{
    m_x += quat.m_x;
    m_y += quat.m_y;
    m_z += quat.m_z;
    m_w += quat.m_w;
}

void Quaternion::operator*=(const Quaternion& quat)
{
    float w = m_w * quat.m_w - m_x * quat.m_x - m_y * quat.m_y - m_z * quat.m_z;
	float x = m_w * quat.m_x + m_x * quat.m_w + m_y * quat.m_z - m_z * quat.m_y;
	float y = m_w * quat.m_y - m_x * quat.m_z + m_y * quat.m_w + m_z * quat.m_x;
	float z = m_w * quat.m_z + m_x * quat.m_y - m_y * quat.m_x + m_z * quat.m_w;
	m_w = w;
	m_x = x;
	m_y = y;
	m_z = z;
}

void Quaternion::operator/=(float scale)
{
	m_x /= scale;
	m_y /= scale;
	m_z /= scale;
	m_w /= scale;
}

QVector3D Quaternion::rotatedVector(const QVector3D &v)
{
	Quaternion V = fromVector3D(v);
	Quaternion conj(*this);
	conj.conjugate();
	Quaternion t = *this * V;
	Quaternion temp = t * conj;
	return (*this * V * conj).toVector3D();
}

Quaternion operator+(Quaternion const& quat, Quaternion const& quat2)
{
	Quaternion copie(quat);
	copie += quat2;
	return copie;
}

Quaternion operator*(Quaternion const& quat, Quaternion const& quat2)
{
	Quaternion copie(quat);
	copie *= quat2;
	return copie;
}