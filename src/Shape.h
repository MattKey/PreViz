#pragma once
#ifndef _SHAPE_H_
#define _SHAPE_H_

#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>
#include <tiny_obj_loader/tiny_obj_loader.h>

class Program;

class Shape
{
public:
	Shape();
	virtual ~Shape();
	void createShape(tinyobj::shape_t & shape);
	void init();
	void measure();
	void draw(const std::shared_ptr<Program> prog) const;
	glm::vec3 min;
	glm::vec3 max;
	glm::vec3 center;
	glm::vec3 size;

	void findEdges();
	void calcNormals();
	void resize();
	void loadMesh(const std::string &meshName);
	std::vector<glm::vec3> getFace(int i, const glm::mat4 &M);
	int getNumFaces();
	glm::vec3 getVertex(int i, const glm::mat4 &M);
	int getNumVertices();
	std::vector<glm::vec3> getEdge(int i, const glm::mat4 &M);
	int getNumEdges();
	std::vector<unsigned int> edgeBuffer;
	
private:
	std::vector<unsigned int> eleBuf;
	std::vector<float> posBuf;
	std::vector<float> norBuf;
	std::vector<float> texBuf;
	std::vector<float> uvBuffer;
	unsigned int uvBufferID = 0;
	unsigned eleBufID;
	unsigned posBufID;
	unsigned norBufID;
	unsigned texBufID;
   unsigned vaoID;
};

#endif
