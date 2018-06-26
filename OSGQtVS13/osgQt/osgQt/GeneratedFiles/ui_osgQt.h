/********************************************************************************
** Form generated from reading UI file 'osgQt.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OSGQT_H
#define UI_OSGQT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_osgQtClass
{
public:

    void setupUi(QWidget *osgQtClass)
    {
        if (osgQtClass->objectName().isEmpty())
            osgQtClass->setObjectName(QStringLiteral("osgQtClass"));
        osgQtClass->resize(600, 400);

        retranslateUi(osgQtClass);

        QMetaObject::connectSlotsByName(osgQtClass);
    } // setupUi

    void retranslateUi(QWidget *osgQtClass)
    {
        osgQtClass->setWindowTitle(QApplication::translate("osgQtClass", "osgQt", 0));
    } // retranslateUi

};

namespace Ui {
    class osgQtClass: public Ui_osgQtClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OSGQT_H
