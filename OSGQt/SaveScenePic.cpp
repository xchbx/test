#include <osgViewer/Viewer>
#include <osgViewer/GraphicsWindow>
#include <osg/Node>
#include <osg/Geode>
#include <osg/Group>
#include <osg/Camera>
#include <osg/Image>
#include <osg/BufferObject>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgUtil/Optimizer>
#include <iostream>
#include <osgGA/GUIEventHandler>
#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>

osg::ref_ptr<osg::Image> image_c = new osg::Image();   //全局对象  用于存储每帧图像数据

struct CaptureCallback :public osg::Camera::DrawCallback
{
	CaptureCallback(osg::ref_ptr<osg::Image> image)
	{
		image_ = image;
	}
	~CaptureCallback(){}

	virtual void operator()(const osg::Camera &camera) const
	{
		//得到窗口系统接口
		osg::ref_ptr<osg::GraphicsContext::WindowingSystemInterface> wsi = osg::GraphicsContext::getWindowingSystemInterface();
		unsigned int width, height;
		//得到分辨率
		wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0), width, height);

		//分配一个image
		image_->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);
		//读取像素信息抓图
		image_->readPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE);
	}

private:
	osg::ref_ptr<osg::Image> image_;

};

//抓图事件处理器
class ImageHandler :public osgGA::GUIEventHandler
{
public:
	ImageHandler()
	{

	}
	~ImageHandler(){}
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		osg::ref_ptr<osgViewer::Viewer> viewer = dynamic_cast<osgViewer::Viewer *>(&aa);

		if (viewer == NULL)
		{
			return false;
		}

		//定义一个静态变量
		static int _screenCaptureSequence = 0;
		switch (ea.getEventType())
		{
		case osgGA::GUIEventAdapter::KEYDOWN:
		{
			if (ea.getKey() == 'c' || ea.getKey() == 'C')
			{
				char filename[128];
				sprintf(filename, "ScreenShot%4d.bmp", _screenCaptureSequence);
				_screenCaptureSequence++;
				osgDB::writeImageFile(*(image_c.get()), filename);
			}
		}
			break;
		default:

			return false;//一定设为false不然不能旋转
		}
		return true;
	}
};

int main()
{
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");
	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(node.get());
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;

	//优化场景数据
	osgUtil::Optimizer optimizer;
	optimizer.optimize(root.get());

	viewer->getCamera()->setPostDrawCallback(new CaptureCallback(image_c.get()));		//写入图像数据
	viewer->setSceneData(root.get());

	viewer->addEventHandler(new ImageHandler());		//事件驱动  生成图片

	viewer->realize();
	viewer->run();
	return 1;
}