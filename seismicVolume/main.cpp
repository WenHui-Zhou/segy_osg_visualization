#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg/Node>
#include <osg/Geode>
#include <osg/Geometry>
#include <osgViewer/ViewerEventHandlers>
#include <osgUtil/Optimizer>
#include <osg/CullFace>
#include <osg/LineWidth>
#include <osg/BoundingBox>
#include <osg/ComputeBoundsVisitor>
#include <osg/LOD>
#include <osgUtil/Simplifier>

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
	float tmp = step(0,intense);
	pixColor = vec4(1.0-intense/maxValue*tmp,1-intense/maxValue*(2*tmp-1),1.0-intense/maxValue*(tmp-1),1.0);
}
);

//fragment shader中的解释说明

//	if (intense>0)
//	{
//		pixColor = vec4(1.0-intense/maxValue, 1-intense/maxValue, 1.0,                1.0); //颜色在白色与蓝色间
//	}
//	else
//	{
//		pixColor = vec4(1.0,                  1+intense/maxValue, 1+intense/maxValue, 1.0); //颜色在白色与红色间
//	}

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


osg::ref_ptr<osg::Node> cretateBoundingBox(osg::Node * node)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
	geode->addDrawable(geom);
	osg::ComputeBoundsVisitor boundVisitor;
	node->accept(boundVisitor);
	osg::BoundingBox boundingBox = boundVisitor.getBoundingBox();

	float length = boundingBox.xMax() - boundingBox.xMin();
	float width = boundingBox.yMax() - boundingBox.yMin();
	float height = boundingBox.zMax() - boundingBox.zMin();

	osg::Vec3Array* vertices = new osg::Vec3Array;
	osg::Vec4Array* colors = new osg::Vec4Array;
	vertices->setName("Vertex");
	vertices->push_back(osg::Vec3f(boundingBox.xMin(),boundingBox.yMin(),boundingBox.zMin()));
	vertices->push_back(osg::Vec3f(boundingBox.xMax(),boundingBox.yMin(),boundingBox.zMin()));

	vertices->push_back(osg::Vec3f(boundingBox.xMax(),boundingBox.yMin(),boundingBox.zMin()));
	vertices->push_back(osg::Vec3f(boundingBox.xMax(),boundingBox.yMax(),boundingBox.zMin()));

	vertices->push_back(osg::Vec3f(boundingBox.xMax(),boundingBox.yMax(),boundingBox.zMin()));
	vertices->push_back(osg::Vec3f(boundingBox.xMin(),boundingBox.yMax(),boundingBox.zMin()));

	
	vertices->push_back(osg::Vec3f(boundingBox.xMin(),boundingBox.yMax(),boundingBox.zMin()));
	vertices->push_back(osg::Vec3f(boundingBox.xMin(),boundingBox.yMin(),boundingBox.zMin()));

	//上面
	vertices->push_back(osg::Vec3f(boundingBox.xMin(),boundingBox.yMin(),boundingBox.zMax()));
	vertices->push_back(osg::Vec3f(boundingBox.xMax(),boundingBox.yMin(),boundingBox.zMax()));

	vertices->push_back(osg::Vec3f(boundingBox.xMax(),boundingBox.yMin(),boundingBox.zMax()));
	vertices->push_back(osg::Vec3f(boundingBox.xMax(),boundingBox.yMax(),boundingBox.zMax()));

	vertices->push_back(osg::Vec3f(boundingBox.xMax(),boundingBox.yMax(),boundingBox.zMax()));
	vertices->push_back(osg::Vec3f(boundingBox.xMin(),boundingBox.yMax(),boundingBox.zMax()));

	
	vertices->push_back(osg::Vec3f(boundingBox.xMin(),boundingBox.yMax(),boundingBox.zMax()));
	vertices->push_back(osg::Vec3f(boundingBox.xMin(),boundingBox.yMin(),boundingBox.zMax()));

	//上下
	vertices->push_back(osg::Vec3f(boundingBox.xMin(),boundingBox.yMin(),boundingBox.zMin()));
	vertices->push_back(osg::Vec3f(boundingBox.xMin(),boundingBox.yMin(),boundingBox.zMax()));

	vertices->push_back(osg::Vec3f(boundingBox.xMax(),boundingBox.yMin(),boundingBox.zMin()));
	vertices->push_back(osg::Vec3f(boundingBox.xMax(),boundingBox.yMin(),boundingBox.zMax()));

	vertices->push_back(osg::Vec3f(boundingBox.xMax(),boundingBox.yMax(),boundingBox.zMin()));
	vertices->push_back(osg::Vec3f(boundingBox.xMax(),boundingBox.yMax(),boundingBox.zMax()));

	vertices->push_back(osg::Vec3f(boundingBox.xMin(),boundingBox.yMax(),boundingBox.zMin()));
	vertices->push_back(osg::Vec3f(boundingBox.xMin(),boundingBox.yMax(),boundingBox.zMax()));

	colors->setName("Color");
	colors->push_back(osg::Vec4f(1.0f,1.0f,1.0f,1.0f));

	    // defaults
    geom->setVertexArray(vertices);
	geom->setColorArray(colors, osg::Array::BIND_OVERALL);

    // set attributes
    geom->setVertexAttribArray(0, vertices, osg::Array::BIND_PER_VERTEX);
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,2));
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,2,2));
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,4,2));
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,6,2));
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,8,2));
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,10,2));
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,12,2));
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,14,2));
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,16,2));
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,18,2));
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,20,2));
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,22,2));
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset = geom->getOrCreateStateSet();
	osg::ref_ptr<osg::LineWidth> linewidth = new osg::LineWidth(2.0);
	stateset->setAttribute(linewidth);
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	return geode;
}

segy RunSegy()
{
	segy sgy;
//	sgy.ReadTraceHeader("shot.segy");
//	sgy.ReadTraceHeader("LineE.sgy");
	sgy.ReadTraceHeader("pg_lm.segy");
	sgy.ReadAllTrace();      //地震数据
//	sgy.ReadOneTrace(1,150); //读切片数据
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
	sgy.setUnitGeom(geom);   	       									//设计需要绘制的图元
	osg::ref_ptr<osg::Camera> camera = viewer->getCamera();
	//设置输入shader 中的数据
	geom->setVertexAttribArray(0,sgy.data,osg::Array::BIND_PER_VERTEX);
	geom->setVertexAttribArray(1,sgy.intensity,osg::Array::BIND_PER_VERTEX);
	//定义shader
	osg::ref_ptr<osg::Shader> vertShader = new osg::Shader(osg::Shader::VERTEX,vertSource1);
	osg::ref_ptr<osg::Shader> fragShader = new osg::Shader(osg::Shader::FRAGMENT,fragSource1);
	osg::ref_ptr<osg::Program> Program = new osg::Program;
	Program->addShader(vertShader.get());
	Program->addShader(fragShader.get());

	osg::DisplaySettings* displaySettings = new osg::DisplaySettings;
	viewer->setDisplaySettings(displaySettings);
	geom->setUseDisplayList(false);
	osg::StateSet* stateset = geom->getOrCreateStateSet();
	stateset->setAttributeAndModes(Program.get(),osg::StateAttribute::ON); //设置shader属性
	// add uniforms
	osg::Uniform* modelViewProjectionMatrix = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "ModelViewProjectionMatrix");
	modelViewProjectionMatrix->setUpdateCallback(new ModelViewProjectionMatrixCallback(camera));
	stateset->addUniform(modelViewProjectionMatrix);
	stateset->addUniform(new osg::Uniform("maxValue",sgy.maxValue));
	// turn lights off
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
	osg::ref_ptr<osg::Node> cowBoundingBox = cretateBoundingBox(geom);
	root->addChild(cowBoundingBox);
//	mt->addChild(cowBoundingBox);

	root->addChild(geom.get());
	osgUtil::Optimizer optimzer;
	optimzer.optimize(root);
	viewer->setSceneData(root.get());
	viewer->realize();
	viewer->run();
}
