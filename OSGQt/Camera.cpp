#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Camera>
#include <osgGA/GUIEventHandler>
#include <osg/GraphicsContext>
#include "osg/MatrixTransform"

#ifdef _DEBUG
#pragma comment(lib,"osgViewerd.lib")
#pragma comment(lib,"osgDBd.lib")
#else
#pragma comment(lib,"osgViewer.lib")
#pragma comment(lib,"osgDB.lib")
#endif

class ChangeWindowSize : public osgGA::GUIEventHandler
{
public:
	ChangeWindowSize(){ first = false; }
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		if (!first)
		{
			osgViewer::Viewer * viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
			osgViewer::Viewer::Windows ws;
			first = true;
			viewer->getWindows(ws);
			if (!ws.empty())
			{
				osgViewer::Viewer::Windows::iterator iter = ws.begin();
				for (; iter != ws.end(); iter++)
				{
					(*iter)->setWindowRectangle(10, 10, 1200, 900);
					(*iter)->setWindowDecoration(false);
				}
			}
		}
			return false;		
	}
private:
	bool first;
};

void CreateCamera(osgViewer::Viewer* viewer)
{
	if (! viewer)
	{
		return;
	}
	osg::GraphicsContext::WindowingSystemInterface *ws = osg::GraphicsContext::getWindowingSystemInterface();
	if (! ws)
	{
		return;
	}

	unsigned int width;
	unsigned int heigh;
	ws->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0), width, heigh);

	osg::ref_ptr<osg::GraphicsContext::Traits>traits = new osg::GraphicsContext::Traits;
	traits->x = 0;
	traits->y = 0;
	traits->width = width;
	traits->height = heigh;
	traits->windowDecoration = false;	
	traits->doubleBuffer = true;
	traits->sharedContext = 0;

	osg::ref_ptr<osg::GraphicsContext>gc = osg::GraphicsContext::createGraphicsContext(traits.get());
	if (! gc->valid())
	{
		return;
	}
	/*gc->setClearColor(osg::Vec4f(0.5, 0.5, 0.5,1.0));
	gc->setClearMask(GL_COLOR_BUFFER_BIT);*/
	osg::ref_ptr<osg::Camera>camera = new osg::Camera;
//	camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera->setGraphicsContext(gc);	
	camera->setViewport(0, 0, width, heigh);
	viewer->addSlave(camera.get());

	osg::ref_ptr<osg::Camera>leftCamera = new osg::Camera;
//	leftCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
	leftCamera->setGraphicsContext(gc);
	leftCamera->setViewport(0, 0, width / 4, heigh/4);
//	viewer->addSlave(leftCamera.get(),osg::Matrix(),osg::Matrix::rotate(osg::DegreesToRadians(-90.0),0.0,1.0,0.0));
	viewer->addSlave(leftCamera.get(), osg::Matrix::scale(osg::Vec3f(4.0f,4.0f,4.0f)), osg::Matrix::rotate(osg::DegreesToRadians(-90.0), 0.0, 1.0, 0.0));
	return;
}

int main()
{
	osg::ref_ptr<osgViewer::Viewer>viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group>group = new osg::Group;
	osg::ref_ptr<osg::Node>node = osgDB::readNodeFile("D:\\3rdParty\\Data\\ceep.ive");
	if (! node)
	{
		printf("mode load failed \n");
		exit(-1);
	}
	group->addChild(node);
	/*osg::ref_ptr<osg::Camera>camera = new osg::Camera;
	camera->setViewport(0, 0, 200, 200);
	camera->addChild(node);
	group->addChild(camera);*/
	CreateCamera(viewer.get());	
	viewer->setSceneData(group.get());
	viewer->addEventHandler(new ChangeWindowSize);
	viewer->run();
	return 0;
}