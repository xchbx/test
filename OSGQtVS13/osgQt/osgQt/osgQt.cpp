#include "osgQt.h"
#include "QtOsgView.h"
#include <QHBoxLayout>
#include <QMainWindow>

osgQt::osgQt(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	QHBoxLayout *layout = new QHBoxLayout;
	this->setLayout(layout);
	QMainWindow* mw = new QMainWindow(this);
	layout->addWidget(mw);
	mw->setWindowFlags(Qt::Widget);

	osg::ref_ptr<osg::Node>  loadedModel = osgDB::readNodeFile("glider.osg");
	QtOsgView * ViewerWindow = new QtOsgView(mw);
	ViewerWindow->setCameraManipulator(new osgGA::TrackballManipulator);
	ViewerWindow->addEventHandler(new osgViewer::WindowSizeHandler);
	ViewerWindow->addEventHandler(new osgViewer::StatsHandler);
	ViewerWindow->setSceneData(loadedModel.get());

	mw->setCentralWidget(ViewerWindow);
}

osgQt::~osgQt()
{

}
