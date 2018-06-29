#ifndef OSGQT_H
#define OSGQT_H

#include <QtWidgets/QWidget>
#include "ui_osgQt.h"

class osgQt : public QWidget
{
	Q_OBJECT

public:
	osgQt(QWidget *parent = 0);
	~osgQt();

private:
	Ui::osgQtClass ui;
};

#endif // OSGQT_H

