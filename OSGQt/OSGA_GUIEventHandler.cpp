#pragma comment(lib, "osgd.lib")  
#pragma comment(lib, "osgDBd.lib")  
#pragma comment(lib, "osgViewerd.lib")  

#include  "osgViewer/Viewer"  
#include  "osgDB/ReadFile"  
#include <osgGA/GUIEventHandler>
#include <iostream>
#include <osgGA/StateSetManipulator>
#include <osgViewer/ViewerEventHandlers>

class Test :public osgGA::GUIEventHandler
{
public:
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		switch (ea.getEventType())
		{
		case osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON:
			std::cout << "event" << std::endl;
			break;
		case osgGA::GUIEventAdapter::KEYDOWN:
			_keyMap[ea.getKey()] = true;
			if ((_keyMap['w'] || _keyMap['W']) && (_keyMap['a']|| _keyMap['A']))
			{
				std::cout << "key w and a pressed" << std::endl;
			}
			if (ea.getKey()==osgGA::GUIEventAdapter::KEY_Alt_L)
			{
				std::cout << "key Alt Left pressed" << std::endl;
			}
			break;
		default:
			break;
		}
		return false;
	}
private:
	std::map<int, bool>_keyMap;
};

int main()
{
	osg::ref_ptr<osgViewer::Viewer>viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Node>node = osgDB::readNodeFile("cow.osg");
	viewer->setSceneData(node.get());
	viewer->addEventHandler(new osgViewer::WindowSizeHandler);  //窗口大小变化事件
	viewer->addEventHandler(new Test);
	viewer->addEventHandler(new osgGA::StateSetManipulator(viewer->getCamera()->getOrCreateStateSet())); 
	viewer->addEventHandler(new osgViewer::StatsHandler); //添加一些常用状态设置
	viewer->run();
	return 0;
}