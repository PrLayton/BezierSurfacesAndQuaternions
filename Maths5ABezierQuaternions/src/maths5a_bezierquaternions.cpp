#include "stdafx.h"
#include "maths5a_bezierquaternions.h"

Math5A_Bezier::Math5A_Bezier(QWidget *parent)
	: QMainWindow(parent), ui(Ui::math5a_bezierForm())
{
	ui.setupUi(this);
	glScene = new GLWidget(this);
	ui.centralLayout->addWidget(glScene);

	// Créer les raccourcis clavier
	// Ctrl + Q pour quitter
	QShortcut *shortcut = new QShortcut(QKeySequence("Ctrl+Q"), this);
	QObject::connect(shortcut, SIGNAL(activated()), this, SLOT(quit()));
	// Ctrl + R pour reset le caméra
	shortcut = new QShortcut(QKeySequence("Ctrl+R"), this);
	QObject::connect(shortcut, SIGNAL(activated()), glScene, SLOT(resetCamera()));
	// Ctrl + D pour réinitialsier les données
	shortcut = new QShortcut(QKeySequence("Ctrl+D"), this);
	QObject::connect(shortcut, SIGNAL(activated()), this, SLOT(resetData()));

	// Signal depuis le GLWidget
	connect(glScene, SIGNAL(labelChanged()), this, SLOT(updateLabelTimer()));
	connect(glScene, SIGNAL(mouseMoved()), this, SLOT(updateStatus()));
	connect(glScene, SIGNAL(rotationReset()), this, SLOT(resetRotation()));

	// Connect signals aux éléments de l'UI
	connect(ui.rbRandom, SIGNAL(clicked()), this, SLOT(setModeGenerationPoints()));
	connect(ui.rbAdjust, SIGNAL(clicked()), this, SLOT(setModeGenerationPoints()));
	connect(ui.cbShowWireframe, SIGNAL(stateChanged(int)), glScene, SLOT(setWireframe(int)));
	connect(ui.cbShowPoints, SIGNAL(stateChanged(int)), glScene, SLOT(setShowPts(int)));
	connect(ui.cbShowGrid, SIGNAL(stateChanged(int)), glScene, SLOT(setGrid(int)));
	connect(ui.bResetData, SIGNAL(clicked()), this, SLOT(resetData()));
	connect(ui.bResetCam, SIGNAL(clicked()), glScene, SLOT(resetCamera()));
	connect(ui.bQuit, SIGNAL(clicked()), this, SLOT(quit()));

	// Bézier
	connect(ui.spinHori, SIGNAL(valueChanged(int)), glScene, SLOT(setDegreeU(int)));
	connect(ui.spinVerti, SIGNAL(valueChanged(int)), glScene, SLOT(setDegreeV(int)));
	connect(ui.spinPrecision, SIGNAL(valueChanged(int)), glScene, SLOT(setPrecision(int)));
	connect(ui.spinX, SIGNAL(valueChanged(double)), this, SLOT(setRotation()));
	connect(ui.spinY, SIGNAL(valueChanged(double)), this, SLOT(setRotation()));
	connect(ui.spinZ, SIGNAL(valueChanged(double)), this, SLOT(setRotation()));
}

// Mettre à jour le mode de génération des points de contrôle
void Math5A_Bezier::setModeGenerationPoints()
{
	if (ui.rbRandom->isChecked())
	{
		glScene->setModeGeneration(1);
	}
	else if (ui.rbAdjust->isChecked())
		glScene->setModeGeneration(2);
}

// Mettre à jour la rotation des points de contrôle
void Math5A_Bezier::setRotation()
{
	double rotX = ui.spinX->value();
	double rotY = ui.spinY->value();
	double rotZ = ui.spinZ->value();
	glScene->doRotation(QVector3D(rotX, rotY, rotZ));
}

// Remise la rotation UI à 0
void Math5A_Bezier::resetRotation()
{
	ui.spinX->setValue(0);
	ui.spinY->setValue(0);
	ui.spinZ->setValue(0);
}

// Remise à défaut le mode de génération
void Math5A_Bezier::resetData()
{
	ui.spinHori->setValue(0);
	ui.spinVerti->setValue(0);
	resetRotation();
	glScene->resetData();
}

// Quitter
void Math5A_Bezier::updateLabelTimer()
{
	ui.laTimeCalcSurface->setText(glScene->labelTimer);
}

// Mettre à jour la position de la souris et des polygones dans le status bar
void Math5A_Bezier::updateStatus()
{
	ui.statusBar->showMessage((QString("Screen (%1,%2) - World (%3,%4)")
		.arg(glScene->mouse.x())
		.arg(glScene->mouse.y())
		.arg(glScene->mouseWorld.x())
		.arg(glScene->mouseWorld.y())
	));
}

// Quitter
void Math5A_Bezier::quit()
{
	qApp->quit();
}

Math5A_Bezier::~Math5A_Bezier()
{
	delete[] glScene;
}