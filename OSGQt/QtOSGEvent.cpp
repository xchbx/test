//version VS2013  Qt 5.7.0   OSG 3.4.0
#include <osgDB/ReadFile>
#include <osgViewer/Viewer> 
#include <osgViewer/ViewerEventHandlers> 
#include <osgGA/StateSetManipulator>

#ifdef _DEBUG
#pragma comment(lib,"osgGAd.lib")
#else
#pragma comment(lib,"osgGA.lib")
#endif
int main()
{
	osgViewer::Viewer viewer; viewer.setSceneData(osgDB::readNodeFile("glider.osg")); //添加状态事件
	viewer.addEventHandler(new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet())); //窗口大小变化事件
	viewer.addEventHandler(new osgViewer::WindowSizeHandler); //添加一些常用状态设置 
	viewer.addEventHandler(new osgViewer::StatsHandler); 
	viewer.realize(); 
	viewer.run();
	return 0;
}