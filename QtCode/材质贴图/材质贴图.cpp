
#include <osg/Group>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/Texture2D>
#include <osgViewer/Viewer>
#include <osgDB/readfile>
 
#include <osg/PositionAttitudeTransform>
#include <osg/TexEnv>
#include <osg/Texture2D>
#include <iostream>
int main()
{
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osgViewer::Viewer myViewer;
 
	//Declear Sphere instance,the constructor takes an osg::vec3 to define
	//center and a float to define the radius.
	//Then we declear the ShapDrawable instance that derive from Drawable .We should 
	//initalize it with the shape we create above .
	osg::ref_ptr<osg::Geode> unitSphere = new osg::Geode;
	osg::ref_ptr<osg::Sphere> sphere = new osg::Sphere(osg::Vec3(0,0,0), 1.0f);
	osg::ref_ptr<osg::ShapeDrawable> shapeDrawable = new osg::ShapeDrawable(sphere.get());
	unitSphere->addDrawable(shapeDrawable.get());
	osg::ref_ptr<osg::PositionAttitudeTransform> sphereForm = new osg::PositionAttitudeTransform;
	sphereForm->setPosition(osg::Vec3(2.5,0.0,0.0));
	sphereForm->addChild(unitSphere.get());
	root->addChild(sphereForm.get());
	
	//load image from the file
	osg::ref_ptr<osg::Texture2D> earthTexture = new osg::Texture2D;
	earthTexture->setDataVariance(osg::Object::DYNAMIC);
	osg::ref_ptr<osg::Image> earthImage = osgDB::readImageFile("D:\\OSG\\osg2.9\\OpenSceneGraph2.9.5\\data\\Images\\land_shallow_topo_2048.jpg");
	if (!earthImage.get())
	{
		std::cout<<"load texture failed !"<<std::endl;
		return -1;
	}
 
	//assign the texture from the image file
	earthTexture->setImage(earthImage.get());
 
	//set the stateset for decal texture
	osg::ref_ptr<osg::TexEnv> texenv = new osg::TexEnv;
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	texenv->setMode(osg::TexEnv::DECAL);
	stateset->setTextureAttributeAndModes(0,earthTexture.get(),osg::StateAttribute::ON);
	stateset->setTextureAttribute(0,texenv.get());
 
	//realize
	root->setStateSet(stateset.get());
	myViewer.setSceneData(root.get());
	myViewer.realize();
	myViewer.run();
}