#pragma once

#include <iostream>
#include <glad/glad.h>
#include <time.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "WindowManager.h"
#include "GLTextureWriter.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

#include "stb_image.h"


using namespace std;
using namespace glm;

class Spider
{
public:
	Spider();
	~Spider();

	shared_ptr<Shape> sphere;
	float time; // frame time for animation

	/* * * * * * * * * * * * * * * * * * * * * * 
	 *                   NOTE                  *
	 *                   (pos)    (neg)        *
	 *         x-axis is right and left        *
	 *         y-axis is up and down           *
	 *         z-axis is back and front        *
	 *                                         *
	 * * * * * * * * * * * * * * * * * * * * * */

	
	float size = 0.1;
	vec3 location = vec3(0);

	// ----------- body variables -------------- //
	vec3 bodyPosition = vec3(0, 0, 1);
	vec3 bodyScale = vec3(1, 0.7, 1);

	// ----------- head variables -------------- //
	vec3 headPosition = vec3(0, 0, -0.2);
	float headRadius = 0.7; // radius of head from top-down view
	float headHeight = headRadius / 2; // distance from center of head to top

	// ----------- eye variables ------------- //
	float eyeOffset = 0.1; // offset from center horizontal line of spider head to center of eyes
	vec3 headToEyePosition = vec3(0, eyeOffset, -headRadius);
	float eyeSize = 0.025;
	float eyeDistances = eyeSize / 2; // distance between each eye

	// ----------- mouth variables ------------- //
	vec3 headToMouthPosition = vec3(0, 0, -headRadius);
	float mouthRadius = eyeSize / 2;
	float mouthWidth = 4 * eyeSize;
	float fangHeight = mouthWidth / 2.0;
	vec3 mouthLineScale = vec3(mouthWidth, mouthRadius, mouthRadius); // top line of mouth
	vec3 mouthFangScale = vec3(mouthRadius, fangHeight, mouthRadius);

	// ----------- leg variables ------------- //
	vec3 sphereToLegScale = vec3(0.05, 0.05, 1);
	float legBendAngle = M_PI_2; // angle of leg joint
	vec3 legOrigin = vec3(-1, 0, 0); // center point where left legs come out of


	// ----------- Functions -------------- //
	void initialize(shared_ptr<Shape> sphere);
	void draw(shared_ptr<Program> prog, shared_ptr<MatrixStack> M);
	void drawEyes(shared_ptr<Program> prog, shared_ptr<MatrixStack> M);
	void drawMouth(shared_ptr<Program> prog, shared_ptr<MatrixStack> M);
	void drawLegs(shared_ptr<Program> prog, shared_ptr<MatrixStack> M);
	void drawLeg(shared_ptr<Program> prog, shared_ptr<MatrixStack> M,
		vec3 rotations, vec3 translate);
	void drawPart(shared_ptr<Program> prog, shared_ptr<MatrixStack> M, vec3 translation, vec3 scale);
	void drawPart(shared_ptr<Program> prog, shared_ptr<MatrixStack> M, vec3 scale);

	vec3 defaultLegRotation(int legNum);
	vec3 defaultLegAnimation(int legNum);
};