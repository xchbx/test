#include "drawpoints.h"

DrawPoints::DrawPoints(QObject *parent)
	: QObject(parent)
{
	connect(this, &DrawPoints::ttt, this, &DrawPoints::ShowMsg);
}

bool DrawPoints::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	switch (ea.getEventType())
	{
	case osgGA::GUIEventAdapter::PUSH:
		if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		{
			int x = ea.getX();
			int y = ea.getY();
			emit ttt(x,y,aa);
		}
	default:
		break;
	}
	return false;
}

void DrawPoints::ShowMsg(int x, int y, osgGA::GUIActionAdapter& aa)
{
	osgViewer::Viewer* pViewer = dynamic_cast<osgViewer::Viewer*>(&aa);
	osg::ref_ptr<osg::MatrixTransform>mt = new osg::MatrixTransform;
	if (pViewer == NULL)
	{
		osg::Vec3d(0, 0, 0);
	}
	osg::ref_ptr<osg::Geode>geod = new osg::Geode;
	osg::ref_ptr<osg::Geometry>geometry = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array>normal = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array>color = new osg::Vec4Array;
	osg::ref_ptr<osg::Vec3Array>point = new osg::Vec3Array;
	osg::ref_ptr<osg::LineWidth>lw = new osg::LineWidth;
	osg::ref_ptr<osg::Node>node = osgDB::readNodeFile("glider.osg");

	printf("%d     %d\n", x, y);
	point->push_back(osg::Vec3(x, 200, y));
	point->push_back(osg::Vec3(0, 200, 0));
//	normal->push_back(osg::Vec3(0, 0, 1.0));
	color->push_back(osg::Vec4(1.0, 0.0, 0.0, 1.0));

	geometry->setVertexArray(point.get());
//	geometry->setNormalArray(normal.get());
//	geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
	geometry->setColorArray(color.get());
	geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	osgUtil::SmoothingVisitor::smooth(*(geometry.get()));
	lw->setWidth(4.0);

	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 2));
	geod->addDrawable(geometry.get());
	geod->getOrCreateStateSet()->setAttributeAndModes(lw, osg::StateAttribute::ON);
	geod->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	mt->addChild(geod.get());
	
	mt->addChild(node);
	mt->setMatrix(/*osg::Matrix::scale(10.0, 10.0, 10.0)**/osg::Matrix::translate(osg::Vec3(x, y, 2)));
	pViewer->setSceneData(mt.get());
}

DrawPoints::~DrawPoints()
{

}
