#include "hoofdscherm.h"
#include "ui_hoofdscherm.h"


HoofdScherm * HoofdScherm::theHoofdScherm = NULL;

HoofdScherm::HoofdScherm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HoofdScherm)
{
    /*QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setSamples(8);
    QSurfaceFormat::setDefaultFormat(format);*/

    InitRandSeed();

    theHoofdScherm = this;
    ui->setupUi(this);
	showMaximized();
	//showFullScreen();

	connect(ui->pushShaderSwitch, &QPushButton::clicked, ui->glScherm, &MijnGLWidget::shaderButtonPressed);

}

HoofdScherm::~HoofdScherm()
{
    delete ui;
}

void HoofdScherm::keyPressEvent( QKeyEvent* e )
{
    ui->glScherm->keyPressEvent(e);
}

void HoofdScherm::setFPS(double fps)
{
	ui->labelFPS->setText(QString::fromStdString(std::to_string(fps)));
}

void HoofdScherm::setShaderInUse(QString shaderName)
{
	ui->pushShaderSwitch->setText(shaderName);
}
