/*
 * Program 3 base code - includes modifications to shape and initGeom in preparation to load
 * multi shape objects 
 * CPE 471 Cal Poly Z. Wood + S. Sueda + I. Dunn
 */

/***********************
 SHADER MANAGER INSTRUCTIONS
 
 HOW TO ADD A SHADER:
 1) Create a #define in ShaderManager.h that will be used to identify your shader
 2) Add an init function in ShaderManager.cpp and put your initialization code there
 - be sure to add a prototype of this function in ShaderManager.h
 3) Call your init function from initShaders in ShaderManager.cpp and save it to the
 respective location in shaderMap. See example
 
 HOW TO USE A SHADER IN THE RENDER LOOP
 1) first, call shaderManager.setCurrentShader(int name) to set the current shader
 2) To retrieve the current shader, call shaderManager.getCurrentShader()
 3) Use the return value of getCurrentShader() to render
 ***********************/
 
/***********************
 SPLINE INSTRUCTIONS

 1) Create a spline object, or an array of splines (for a more complex path)
 2) Initialize the splines. I did this in initGeom in this example. There are 
	two constructors for it, for order 2 and order 3 splines. The first uses
	a beginning, intermediate control point, and ending. In the case of Bezier splines, 
	the path is influenced by, but does NOT necessarily touch, the control point. 
	There is a second constructor, for order 3 splines. These have two control points. 
	Use these to create S-curves. The constructor also takes a duration of time that the 
	path should take to be completed. This is in seconds. 
 3) Call update(frametime) with the time between the frames being rendered. 
	3a) Call isDone() and switch to the next part of the path if you are using multiple 
	    paths or something like that. 
 4) Call getPosition() to get the vec3 of where the current calculated position is. 
 ***********************/

#include <chrono>
#include <vector>
#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "Time.h"
#include "physics/PhysicsObject.h"
#include "physics/ColliderSphere.h"
#include "physics/ColliderMesh.h"
#include "Constants.h"
#include "Spider.h"
#include "ShaderManager.h"
#include "Spline.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

TimeData Time;

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;
    
  	ShaderManager * shaderManager;

	// Shape to be used (from  file) - modify to support multiple
	shared_ptr<Shape> sphere;
	shared_ptr<Shape> cube;

	vector<shared_ptr<PhysicsObject>> physicsObjects;

	//hand
	vector<shared_ptr<Shape>> hand;

	//spider
	vector<shared_ptr<Shape>> spider;

	// 8 Eyes
	vector<shared_ptr<Shape>> eye1;
	vector<shared_ptr<Shape>> eye2;
	vector<shared_ptr<Shape>> eye3;
	vector<shared_ptr<Shape>> eye4;
	vector<shared_ptr<Shape>> eye5;
	vector<shared_ptr<Shape>> eye6;
	vector<shared_ptr<Shape>> eye7;
	vector<shared_ptr<Shape>> eye8;

	// spline vectors for "animation"
	vector<Spline> spiderPaths;
	vector<Spline> spiderRots;
	vector<Spline> handRots;
	vector<Spline> eyePaths;

	//position vectors
	glm::vec3 handPos = vec3(0, -0.3, -5);
	glm::vec3 eye1Pos = vec3(-0.02, 0.01, -0.2);
	glm::vec3 eye2Pos = vec3(-0.005, 0.01, -0.2);
	glm::vec3 eye3Pos = vec3(0.005, 0.01, -0.2);
	glm::vec3 eye4Pos = vec3(0.02, 0.01, -0.2);
	glm::vec3 eye5Pos = vec3(-0.02, 0, -0.2);
	glm::vec3 eye6Pos = vec3(-0.01, 0, -0.2);
	glm::vec3 eye7Pos = vec3(0.01, 0, -0.2);
	glm::vec3 eye8Pos = vec3(0.02, 0, -0.2);

	//rotations of spider
	float xspidRot = 0;				//X-axis
	float yspidRot = M_PI_2;		//Y-axis
	float zspidRot = M_PI_2;		//Z-axis

	//rotations of hand
	float xhandRot = M_PI_4/2;		//x-axis
	float yhandRot = 0;		//y-axis
	float zhandRot = 0;		//z-axis

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our triangle to OpenGL
	GLuint VertexBufferID;

	//example data that might be useful when trying to compute bounds on multi-shape
	vec3 gMin;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
		if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			 glfwGetCursorPos(window, &posX, &posY);
			 cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
		}
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.5,0.2,0.2, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

        // create the Instance of ShaderManager which will initialize all shaders in its constructor
		shaderManager = new ShaderManager(resourceDirectory);
	}

	void loadMultiPartObject(const std::string& resource, vector<shared_ptr<Shape>>* object)
	{
		// Initialize mesh
		// Load geometry
		// Some obj files contain material information.We'll ignore them for this assignment.
		vector<tinyobj::shape_t> TOshapes;
		vector<tinyobj::material_t> objMaterials;
		string errStr;
		shared_ptr<Shape> s;
		//load in the mesh and make the shape(s)
		bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resource).c_str());
		if (!rc) {
			cerr << errStr << endl;
		}
		else {
			for (int i = 0; i < TOshapes.size(); i++)
			{
				s = make_shared<Shape>();
				s->createShape(TOshapes[i]);
				s->measure();
				s->init();
				object->push_back(s);
			}
		}
	}

	void drawMultiPartObject(vector<shared_ptr<Shape>>* object, shared_ptr<Program>* program)
	{
		for (int i = 0; i < object->size(); i++)
			(*object)[i]->draw(*program);
	}

	void initTextures(const std::string& resourceDirectory)
	{

	}

	void initGeom(const std::string& resourceDirectory)
	{
		//loadMultiPartObject(resourceDirectory + "/models/hand_low_quality.obj", &hand);
		loadMultiPartObject(resourceDirectory + "/models/spider_low_quality.obj", &spider);
		loadMultiPartObject(resourceDirectory + "/models/hand_low_quality.obj", &hand);
		loadMultiPartObject(resourceDirectory + "/models/ico_sphere.obj", &eye1);
		loadMultiPartObject(resourceDirectory + "/models/ico_sphere.obj", &eye2);
		loadMultiPartObject(resourceDirectory + "/models/ico_sphere.obj", &eye3);
		loadMultiPartObject(resourceDirectory + "/models/ico_sphere.obj", &eye4);
		loadMultiPartObject(resourceDirectory + "/models/ico_sphere.obj", &eye5);
		loadMultiPartObject(resourceDirectory + "/models/ico_sphere.obj", &eye6);
		loadMultiPartObject(resourceDirectory + "/models/ico_sphere.obj", &eye7);
		loadMultiPartObject(resourceDirectory + "/models/ico_sphere.obj", &eye8);

		//read out information stored in the shape about its size - something like this...
		//then do something with that information.....
		//gMin.x = sphere->min.x;
		//gMin.y = sphere->min.y;

		//lower down
		spiderPaths.push_back(Spline(glm::vec3(0, 2, -2.5), glm::vec3(0, 1, -2.5), glm::vec3(0, 0.5, -2.5), 2));
		//slowly lower onto hand
		spiderPaths.push_back(Spline(glm::vec3(0, 0.5, -2.5), glm::vec3(0, 0.25, -2.5), glm::vec3(0, 0, -2.5), 1));
		//rotate onto hand
		spiderRots.push_back(Spline(glm::vec3(0, 0, 0), glm::vec3(-M_PI_4, 0, 0), glm::vec3(-M_PI_2, 0, 0), 1));
		//sit still
		spiderPaths.push_back(Spline(glm::vec3(0, 0, -2.5), glm::vec3(0, 0, -2.5), glm::vec3(0, 0, -2.5), 0.5));
		//stay still (but closer) for 1 second
		spiderPaths.push_back(Spline(glm::vec3(0, 0, -1.5), glm::vec3(0, 0, -1.5), glm::vec3(0, 0, -1.5), 1));
		//down and up for bite, 0.1seconds
		spiderPaths.push_back(Spline(glm::vec3(0, 0, -1.5), glm::vec3(0, -0.1, -1.5), glm::vec3(0, 0, -1.5), 0.1));
		//stay still for 3/4 of a second
		spiderPaths.push_back(Spline(glm::vec3(0, 0, -1.5), glm::vec3(0, 0, -1.5), glm::vec3(0, 0, -1.5), 0.75));
		//fall down in arc path for 1.5 seconds
		spiderPaths.push_back(Spline(glm::vec3(0, 0, -5), glm::vec3(0, 1, -4), glm::vec3(0, -1.5, -3), glm::vec3(0, -1.5, -3), 1.5));
		//rotation variable for rotation along x axis
		spiderRots.push_back(Spline(glm::vec3(-M_PI_2, 0, 0), glm::vec3(0, 0, 0), glm::vec3(M_PI_2, 0, 0), 1));
		//slow "zoom" in
		spiderPaths.push_back(Spline(glm::vec3(0, -0.25, -1), glm::vec3(0, -0.125, -0.625), glm::vec3(0, 0, -0.3), 2.5));
		//rotate hand for spider fall
		handRots.push_back(Spline(glm::vec3(M_PI_4/2, 0, 0), glm::vec3(M_PI_4, 0, 0), glm::vec3((M_PI_4/2)+M_PI_2, 0, 0), 1));
	
		// Eyes 1, 5, 6 move toward eye 2
		eyePaths.push_back(Spline(eye1Pos, eye2Pos, eye2Pos, 3));
		eyePaths.push_back(Spline(eye5Pos, eye2Pos, eye2Pos, 3));
		eyePaths.push_back(Spline(eye6Pos, eye2Pos, eye2Pos, 3));
		// Eyes 4, 7, 8 move toward eye 3 
		eyePaths.push_back(Spline(eye4Pos, eye3Pos, eye3Pos, 3));
		eyePaths.push_back(Spline(eye7Pos, eye3Pos, eye3Pos, 3));
		eyePaths.push_back(Spline(eye8Pos, eye3Pos, eye3Pos, 3));
	}
    
    mat4 SetProjectionMatrix(shared_ptr<Program> curShader) {
        int width, height;
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
        float aspect = width/(float)height;
        mat4 Projection = perspective(radians(50.0f), aspect, 0.1f, 100.0f);
        glUniformMatrix4fv(curShader->getUniform("P"), 1, GL_FALSE, value_ptr(Projection));
        return Projection;
    }
    
    void SetViewMatrix(shared_ptr<Program> curShader) {
        auto View = make_shared<MatrixStack>();
        View->pushMatrix();
        glUniformMatrix4fv(curShader->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
        View->popMatrix();
    }

	void render(float frametime) {
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderManager->setCurrentShader(SIMPLEPROG);
        renderSimpleProg(frametime);
	}
    
    void renderSimpleProg(float frametime) {
		shared_ptr<Program> spiderProg;
		shared_ptr<Program> eyeProg;
		shared_ptr<Program> handProg;

        auto Model = make_shared<MatrixStack>();

		shaderManager->setCurrentShader(HANDPROG);
		auto simple = shaderManager->getCurrentShader();

        	

			// Demo of Bezier Spline
			glm::vec3 spidPos;

			int current = 0;
			for (int i = 0; i < spiderPaths.size(); i++) {
				if (spiderPaths.at(i).isDone() && i < spiderPaths.size() - 1) {
					current = i + 1;
				}
			}
			spiderPaths.at(current).update(frametime);
			spidPos = spiderPaths.at(current).getPosition();

			if (current == 1) {		//rotate spider onto hand
				spiderRots.at(0).update(frametime);
				xspidRot = spiderRots.at(0).getPosition().x;
			}
			else if (current == 6) {	//rotate spider for fall
				spiderRots.at(1).update(frametime);
				xspidRot = spiderRots.at(1).getPosition().x;
			}
			else if (current == 7) {	//set spider rotations for upside down eye merge
				xspidRot = 0;
				yspidRot = -M_PI_2;
				zspidRot = M_PI;
			}

			if (current == 3 || current == 4 || current == 5) {	//zoom the hand appropriately
				handPos = vec3(0, -0.3, -4);
			}
			else if (current == 6) {		//zoom the hand for the bite
				handPos = vec3(0, -0.3, -7.5);
				handRots.at(0).update(frametime);
				xhandRot = handRots.at(0).getPosition().x;
			}

            // draw mesh
			if (current < 7) {		//only render the hand when necessary
				Model->pushMatrix();
				Model->loadIdentity();
				//"global" translate
				Model->translate(handPos);
				Model->rotate(xhandRot, XAXIS);
				Model->rotate(yhandRot, YAXIS);
				Model->rotate(zhandRot, ZAXIS);
				Model->scale(vec3(0.5, 0.5, 0.5));
				shaderManager->setCurrentShader(HANDPROG);
				simple = shaderManager->getCurrentShader();
				simple->bind();
	            SetProjectionMatrix(simple);
         		SetViewMatrix(simple);
				glUniformMatrix4fv(simple->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
				drawMultiPartObject(&hand, &simple);
				simple->unbind();
				Model->popMatrix();
			}
			
			// When the zooming to the spider is done. Show the frame where the 8 eyes merging
			if (spiderPaths.at(7).isDone() && !eyePaths.at(5).isDone()) {
				for (int i = 0; i < eyePaths.size(); i++) {
					eyePaths.at(i).update(frametime);
				}
				eye1Pos = eyePaths.at(0).getPosition();
				eye4Pos = eyePaths.at(1).getPosition();
				eye5Pos = eyePaths.at(2).getPosition();
				eye6Pos = eyePaths.at(3).getPosition();
				eye7Pos = eyePaths.at(4).getPosition();
				eye8Pos = eyePaths.at(5).getPosition();					
				
				// 8 eyes
				shaderManager->setCurrentShader(PUPILPROG);
				simple = shaderManager->getCurrentShader();
				simple->bind();
	            SetProjectionMatrix(simple);
         		SetViewMatrix(simple);
				Model->pushMatrix();
					Model->loadIdentity();
					Model->translate(eye1Pos);
					Model->scale(0.0035);
					glUniformMatrix4fv(simple->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
					drawMultiPartObject(&eye1, &simple);
				Model->popMatrix();
				
				Model->pushMatrix();
					Model->loadIdentity();
					Model->translate(eye2Pos);
					Model->scale(0.005);
					glUniformMatrix4fv(simple->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
					drawMultiPartObject(&eye2, &simple);
				Model->popMatrix();
				
				Model->pushMatrix();
					Model->loadIdentity();
					Model->translate(eye3Pos);
					Model->scale(0.005);
					glUniformMatrix4fv(simple->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
					drawMultiPartObject(&eye3, &simple);
				Model->popMatrix();
				
				Model->pushMatrix();
					Model->loadIdentity();
					Model->translate(eye4Pos);
					Model->scale(0.0035);
					glUniformMatrix4fv(simple->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
					drawMultiPartObject(&eye4, &simple);
				Model->popMatrix();
				
				Model->pushMatrix();
					Model->loadIdentity();
					Model->translate(eye5Pos);
					Model->scale(0.0035);
					glUniformMatrix4fv(simple->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
					drawMultiPartObject(&eye5, &simple);
				Model->popMatrix();
				
				Model->pushMatrix();
					Model->loadIdentity();
					Model->translate(eye6Pos);
					Model->scale(0.0035);
					glUniformMatrix4fv(simple->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
					drawMultiPartObject(&eye6, &simple);
				Model->popMatrix();

				Model->pushMatrix();
					Model->loadIdentity();
					Model->translate(eye7Pos);
					Model->scale(0.0035);
					glUniformMatrix4fv(simple->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
					drawMultiPartObject(&eye7, &simple);
				Model->popMatrix();

				Model->pushMatrix();
					Model->loadIdentity();
					Model->translate(eye8Pos);
					Model->scale(0.0035);
					glUniformMatrix4fv(simple->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
					drawMultiPartObject(&eye8, &simple);
				Model->popMatrix();
				simple->unbind();
			}

			// After the eyes merged together, we have 2 big eyes (eye2 + eye 3)
			else if (eyePaths.at(5).isDone()){
				glm::vec3 eye3Pos = vec3(0.008, 0.01, -0.2);
				glm::vec3 eye2Pos = vec3(-0.008, 0.01, -0.2);
				
				shaderManager->setCurrentShader(EYEPROG);
				simple = shaderManager->getCurrentShader();
				simple->bind();
	            SetProjectionMatrix(simple);
         		SetViewMatrix(simple);
				Model->pushMatrix();
					Model->loadIdentity();
					Model->translate(eye2Pos);
					Model->scale(0.01);
					glUniformMatrix4fv(simple->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
					drawMultiPartObject(&eye3, &simple);
				Model->popMatrix();

				Model->pushMatrix();
					Model->loadIdentity();
					Model->translate(eye3Pos);
					Model->scale(0.01);
					glUniformMatrix4fv(simple->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
					drawMultiPartObject(&eye3, &simple);
				Model->popMatrix();
				simple->unbind();

				// draw pupils
				shaderManager->setCurrentShader(PUPILPROG);
				simple = shaderManager->getCurrentShader();
				simple->bind();
	            SetProjectionMatrix(simple);
         		SetViewMatrix(simple);

				Model->pushMatrix();
					Model->loadIdentity();
					Model->translate(vec3(0.008, 0.01, -0.15));
					Model->scale(0.002);
					glUniformMatrix4fv(simple->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
					drawMultiPartObject(&eye3, &simple);
				Model->popMatrix();
				Model->pushMatrix();
					Model->loadIdentity();
					Model->translate(vec3(-0.008, 0.01, -0.15));
					Model->scale(0.002);
					glUniformMatrix4fv(simple->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
					drawMultiPartObject(&eye3, &simple);
				Model->popMatrix();
				simple->unbind();

			}

			// The spider should be shown in all frames
			shaderManager->setCurrentShader(SPIDERPROG);
			simple = shaderManager->getCurrentShader();
			simple->bind();
	        SetProjectionMatrix(simple);
         	SetViewMatrix(simple);
			Model->pushMatrix();
				Model->loadIdentity();
				//Model->translate(vec3(0, 0, -1));
				Model->translate(spidPos);			//move
				Model->scale(0.05);					//size
				Model->rotate(xspidRot, XAXIS);		//rotate along X
				Model->rotate(yspidRot, YAXIS);		//rotate along Y
				Model->rotate(zspidRot, ZAXIS);		//rotate along Z
				//spider.draw(simple, Model);
				glUniformMatrix4fv(simple->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
				drawMultiPartObject(&spider, &simple);
			Model->popMatrix();
			simple->unbind();

			for (auto obj : physicsObjects) {
				obj->draw(simple, Model);
			}
    }

	void updatePhysics(float dt) {
		for (int i = 0; i < physicsObjects.size(); i++) {
			for (int j = i + 1; j < physicsObjects.size(); j++) {
				physicsObjects[i]->checkCollision(physicsObjects[j].get());
			}
		}
		for (auto obj : physicsObjects) {
			obj->update();
		}
	}
};

int main(int argc, char *argv[])
{
	// Where the resources are loaded from
	std::string resourceDir = "../resources";

	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
	application->initGeom(resourceDir);
	//application->initPhysicsObjects();

	auto lastTime = chrono::high_resolution_clock::now();
	float accumulator = 0.0f;
	Time.physicsDeltaTime = 0.02f;

	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{

		// save current time for next frame
		auto nextLastTime = chrono::high_resolution_clock::now();

		// get time since last frame
		float deltaTime =
			chrono::duration_cast<std::chrono::microseconds>(
				chrono::high_resolution_clock::now() - lastTime)
				.count();

		// convert microseconds (weird) to seconds (less weird)
		deltaTime *= 0.000001;

		// reset lastTime so that we can calculate the deltaTime
		// on the next frame
		lastTime = nextLastTime;

		accumulator += deltaTime;
		while (accumulator >= Time.physicsDeltaTime) {
			application->updatePhysics(Time.physicsDeltaTime);
			accumulator -= Time.physicsDeltaTime;
		}

		// Render scene.
		application->render(deltaTime);

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}