#ifndef HOOFDSCHERM_H
#define HOOFDSCHERM_H

#include <QMainWindow>
#include "commonfunctions.h"
#include <QLCDNumber>
#include <iostream>
#include "mijnglwidget.h"

namespace Ui {
class HoofdScherm;
}

class HoofdScherm : public QMainWindow
{
    Q_OBJECT

public:
    explicit HoofdScherm(QWidget *parent = 0);
    ~HoofdScherm();

    static HoofdScherm * Instance () { return theHoofdScherm; }
    static HoofdScherm * theHoofdScherm;

	void setFPS(double fps);
	void setShaderInUse(QString shaderName);
private:
    Ui::HoofdScherm *ui;
    void keyPressEvent( QKeyEvent* e );


};


#endif // HOOFDSCHERM_H
