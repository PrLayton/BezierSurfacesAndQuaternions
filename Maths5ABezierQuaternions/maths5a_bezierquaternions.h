#ifndef MATHS5A_BEZIERQUATERNIONS_H
#define MATHS5A_BEZIERQUATERNIONS_H

#include <QtWidgets/QMainWindow>
#include "ui_maths5a_bezierquaternions.h"

class Maths5ABezierQuaternions : public QMainWindow
{
	Q_OBJECT

public:
	Maths5ABezierQuaternions(QWidget *parent = 0);
	~Maths5ABezierQuaternions();

private:
	Ui::Maths5ABezierQuaternionsClass ui;
};

#endif // MATHS5A_BEZIERQUATERNIONS_H
