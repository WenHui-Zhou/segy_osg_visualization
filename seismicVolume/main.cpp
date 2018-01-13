#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg/Node>
#include <osg/Geode>
#include <osg/Geometry>
#include <osgViewer/ViewerEventHandlers>
#include <osgUtil/Optimizer>

#include <iostream>
#include <cstring>
#include "segy.h"
#include <windows.h>

using namespace std;

segy RunSegy()
{
	segy sgy;
//	sgy.ReadTraceHeader("shot.segy");
//	sgy.ReadTraceHeader("LineE.sgy");
	sgy.ReadTraceHeader("pg_lm.segy");
	sgy.ReadAllTrace();
//	sgy.PrintTextHeader();
//	sgy.PrintBinaryHeader();
//	printf(".........................\n");
//	sgy.PrintTraceHeader();
	return sgy;
}

int main( int argc, char** argv )
{
	osg::ref_ptr<osg::Group> root = new osg::Group;
//	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
//	geode->addDrawable(geom);
	segy sgy = RunSegy();
	sgy.drawAllTrace();													//读取需绘制的数据，颜色映射
	geom->setVertexArray(sgy.data);
	geom->setColorArray(sgy.color);
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);				//颜色绑定对应的一个点
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	sgy.setUnitGeom(geom);          									//设计需要绘制的图元
	osg::DisplaySettings* displaySettings = new osg::DisplaySettings;
	viewer->setDisplaySettings(displaySettings);
	geom->setUseDisplayList(false);
	osg::ref_ptr<osg::StateSet> stateset = root->getOrCreateStateSet();
	// turn lights off
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
	root->addChild(geom.get());
	osgUtil::Optimizer optimzer;
	optimzer.optimize(root);
	viewer->setSceneData(root.get());
	viewer->realize();
	viewer->run();
}