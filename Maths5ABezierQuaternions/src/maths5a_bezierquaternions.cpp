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
	QObject::connect(shortcut, SIGNAL(activated()), glScene, SLOT(resetData()));

	// Signal pour mettre à jour le lablel du Timer
	connect(glScene, SIGNAL(labelChanged()), this, SLOT(updateLabelTimer()));

	// Connect signals aux éléments de l'UI
	connect(ui.rbRandom, SIGNAL(clicked()), this, SLOT(setModeGenerationPoints()));
	connect(ui.rbAdjust, SIGNAL(clicked()), this, SLOT(setModeGenerationPoints()));
	connect(ui.cbShowWireframe, SIGNAL(stateChanged(int)), glScene, SLOT(setWireframe(int)));
	connect(ui.cbShowPoints, SIGNAL(stateChanged(int)), glScene, SLOT(setShowPts(int)));
	connect(ui.cbShowGrid, SIGNAL(stateChanged(int)), glScene, SLOT(setGrid(int)));
	connect(ui.bResetData, SIGNAL(clicked()), glScene, SLOT(resetData()));
	connect(ui.bResetCam, SIGNAL(clicked()), glScene, SLOT(resetCamera()));
	connect(ui.bQuit, SIGNAL(clicked()), this, SLOT(quit()));
	connect(glScene, SIGNAL(mouseMoved()), this, SLOT(updateStatus()));

	// Bézier
	connect(ui.spinHori, SIGNAL(valueChanged(int)), glScene, SLOT(setDegreeU(int)));
	connect(ui.spinVerti, SIGNAL(valueChanged(int)), glScene, SLOT(setDegreeV(int)));
	connect(ui.spinPrecision, SIGNAL(valueChanged(int)), glScene, SLOT(setPrecision(int)));
}

// Mettre à jour le mode d'envelope
void Math5A_Bezier::setModeGenerationPoints()
{
	if (ui.rbRandom->isChecked())
	{
		glScene->setModeGeneration(1);
	}
	else if (ui.rbAdjust->isChecked())
		glScene->setModeGeneration(2);
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