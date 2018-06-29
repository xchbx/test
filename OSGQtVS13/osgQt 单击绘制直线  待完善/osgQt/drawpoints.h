#ifndef DRAWPOINTS_H
#define DRAWPOINTS_H

#include <QObject>
#include <QMessageBox>
#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osg/LineWidth>
#include <osgUtil/SmoothingVisitor>

#pragma comment(lib,"osgUtild.lib")

class DrawPoints : public QObject, public osgGA::GUIEventHandler
{
	Q_OBJECT

public:
	DrawPoints(QObject *parent);
	bool handle(const osgGA::GUIEventAdapter&, osgGA::GUIActionAdapter&);
	~DrawPoints();

signals:
	void ttt(int x, int y, osgGA::GUIActionAdapter& aa);

private slots:
void ShowMsg(int x, int y, osgGA::GUIActionAdapter& aa);

private:
	
};

#endif // DRAWPOINTS_H
