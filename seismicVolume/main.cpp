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

#define GLSL400(src) "#version 400\n" #src

const char* vertSource1 = GLSL400 (
uniform mat4 ModelViewProjectionMatrix;
uniform float maxValue;
layout(location = 0) in vec4 Vertex;
layout(location = 1) in float intense;
out vec4 pixColor;
void main(void)
{
	gl_Position = ModelViewProjectionMatrix * Vertex;
	if (intense>0)
	{
		pixColor = vec4(1-intense/maxValue,1-intense/maxValue,1.0,1.0);
	}
	else
	{
		pixColor = vec4(1.0,1+intense/maxValue,1+intense/maxValue,1.0);
	}
}
);

const char* fragSource1 = GLSL400(
	in vec4 pixColor;
void main(void)
{
	gl_FragColor = pixColor;
}
);

struct ModelViewProjectionMatrixCallback:public osg::Uniform::Callback{
	ModelViewProjectionMatrixCallback(osg::Camera* camera):_camera(camera){}
	virtual void operator()(osg::Uniform* uniform, osg::NodeVisitor* nv) {
		osg::Matrixd viewMatrix = _camera->getViewMatrix();
		osg::Matrixd modelMatrix = osg::computeLocalToWorld(nv->getNodePath());
		osg::Matrixd modelViewProjectionMatrix = modelMatrix * viewMatrix * _camera->getProjectionMatrix();
		uniform->set(modelViewProjectionMatrix);
	}

	osg::Camera* _camera;
};



segy RunSegy()
{
	segy sgy;
//	sgy.ReadTraceHeader("shot.segy");
	sgy.ReadTraceHeader("LineE.sgy");
//	sgy.ReadTraceHeader("pg_lm.segy");
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
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	segy sgy = RunSegy();
	sgy.readFaceData();
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Camera> camera = viewer->getCamera();
	//设置输入shader 中的数据
	geom->setVertexAttribArray(0,sgy.data,osg::Array::BIND_PER_VERTEX);
	geom->setVertexAttribArray(1,sgy.intensity,osg::Array::BIND_PER_VERTEX);

	sgy.setUnitGeom(geom);          									//设计需要绘制的图元
	//定义shader
	osg::ref_ptr<osg::Shader> vertShader = new osg::Shader(osg::Shader::VERTEX,vertSource1);
	osg::ref_ptr<osg::Shader> fragShader = new osg::Shader(osg::Shader::FRAGMENT,fragSource1);
	osg::ref_ptr<osg::Program> Program = new osg::Program;
	Program->addShader(vertShader.get());
	Program->addShader(fragShader.get());

	osg::DisplaySettings* displaySettings = new osg::DisplaySettings;
	viewer->setDisplaySettings(displaySettings);
	geom->setUseDisplayList(false);
	osg::StateSet* stateset = root->getOrCreateStateSet();
	stateset->setAttributeAndModes(Program.get(),osg::StateAttribute::ON); //设置shader属性
	// add uniforms
	osg::Uniform* modelViewProjectionMatrix = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "ModelViewProjectionMatrix");
	modelViewProjectionMatrix->setUpdateCallback(new ModelViewProjectionMatrixCallback(camera));
	stateset->addUniform(modelViewProjectionMatrix);
	stateset->addUniform(new osg::Uniform("maxValue",sgy.maxValue));
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
