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

	// Signal pour mettre à jour les lablels des Timers
	connect(glScene, SIGNAL(labelChanged(int)), this, SLOT(updateLabels(int)));

	// Connect signals aux éléments de l'UI
	connect(ui.rbJarvis, SIGNAL(clicked()), this, SLOT(modeEnvelop()));
	connect(ui.rbGrahamScan, SIGNAL(clicked()), this, SLOT(modeEnvelop()));
	connect(ui.rbNoneEnv, SIGNAL(clicked()), this, SLOT(modeEnvelop()));
	connect(ui.cbShowEnvelop3D, SIGNAL(stateChanged(int)), glScene, SLOT(setEnvelop3D(int)));
	connect(ui.cbShowGrid, SIGNAL(stateChanged(int)), glScene, SLOT(setGrid(int)));
	connect(ui.bResetData, SIGNAL(clicked()), glScene, SLOT(resetData()));
	connect(ui.bResetCam, SIGNAL(clicked()), glScene, SLOT(resetCamera()));
	connect(ui.bQuit, SIGNAL(clicked()), this, SLOT(quit()));

	// Bézier
	connect(ui.spinHori, SIGNAL(valueChanged(int)), glScene, SLOT(setDegreeU(int)));
	connect(ui.spinVerti, SIGNAL(valueChanged(int)), glScene, SLOT(setDegreeV(int)));
	connect(ui.spinPrecision, SIGNAL(valueChanged(int)), glScene, SLOT(setPrecision(int)));
}

// Mettre à jour les labels des Timers
void Math5A_Bezier::updateLabels(int label)
{
	switch (label)
	{
	case 0:
		ui.laTimeJarvis->setText(glScene->labelTimer[label]);
		break;
	default:
		break;
	}
}

// Mettre à jour le mode d'envelope
void Math5A_Bezier::modeEnvelop() 
{
	if (ui.rbJarvis->isChecked())
	{
		glScene->changeModeEnvelop(1);
	}
	else if (ui.rbGrahamScan->isChecked())
	{
		glScene->changeModeEnvelop(2);
	}
	else
		glScene->changeModeEnvelop(0);
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