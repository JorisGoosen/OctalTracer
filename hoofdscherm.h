#ifndef HOOFDSCHERM_H
#define HOOFDSCHERM_H

#include <QMainWindow>
#include "commonfunctions.h"

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

private:
    Ui::HoofdScherm *ui;
    void keyPressEvent( QKeyEvent* e );


};


#endif // HOOFDSCHERM_H
