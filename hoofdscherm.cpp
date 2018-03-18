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

}

HoofdScherm::~HoofdScherm()
{
    delete ui;
}

void HoofdScherm::keyPressEvent( QKeyEvent* e )
{
    ui->glScherm->keyPressEvent(e);
}
