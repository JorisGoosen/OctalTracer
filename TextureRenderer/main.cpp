#include "hoofdscherm.h"
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{


    QApplication a(argc, argv);
    HoofdScherm w;
    w.show();

	int EindWaarde = a.exec();

	//std::cin.get();

	return EindWaarde;

}
