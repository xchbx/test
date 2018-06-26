#include "osgQt.h"
#include <QtWidgets/QApplication>
#include "QtOsgView.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	osgQt w;
	w.show();
	return a.exec();
}

