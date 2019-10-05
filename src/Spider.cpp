#include "Spider.h"
#include "Constants.h"


using namespace std;
using namespace glm;



Spider::Spider()
{
}


Spider::~Spider()
{
}

void Spider::initialize(shared_ptr<Shape> sphere)
{
	this->sphere = sphere;
}

void Spider::draw(shared_ptr<Program> prog, shared_ptr<MatrixStack> M)
{
	M->pushMatrix();
		M->translate(location);
		M->scale(size);

		drawPart(prog, M, bodyPosition, bodyScale); // body

		M->pushMatrix(); // head
			M->translate(headPosition);
			drawEyes(prog, M);
			drawMouth(prog, M);
			drawPart(prog, M, vec3(headRadius, headHeight, headRadius));
		M->popMatrix();

		drawLegs(prog, M);
	M->popMatrix();
}

void Spider::drawEyes(shared_ptr<Program> prog, shared_ptr<MatrixStack> M)
{
	M->pushMatrix();
	M->translate(headToEyePosition);
	for (int i = 0; i < 4; ++i) {
		float x = i * 2 * eyeSize - eyeSize * 3;
		drawPart(prog, M, vec3(x, eyeSize, 0), vec3(eyeSize)); // top row
		drawPart(prog, M, vec3(x, -eyeSize, 0), vec3(eyeSize)); // bottom row
	}
	M->popMatrix();
}

void Spider::drawMouth(shared_ptr<Program> prog, shared_ptr<MatrixStack> M)
{
	M->pushMatrix();
	M->translate(headToMouthPosition);

	// top line of mouth
	drawPart(prog, M, mouthLineScale);

	// Fangs
	drawPart(prog, M, vec3(mouthWidth / 2 + 2 * mouthRadius, -fangHeight / 2 - mouthRadius, 0), mouthFangScale);
	drawPart(prog, M, vec3(-mouthWidth / 2 - 2 * mouthRadius, -fangHeight / 2 - mouthRadius, 0), mouthFangScale);

	M->popMatrix();
}

void Spider::drawLegs(shared_ptr<Program> prog, shared_ptr<MatrixStack> M)
{
	float adjustment = M_PI_4 / 2;
	float delta = -M_PI_4 / 2;
	vec3 rotations;
	M->pushMatrix();
	for (int i = 0; i < 4; ++i) {
		rotations = defaultLegRotation(i);
		drawLeg(prog, M, rotations, legOrigin); // left leg
		drawLeg(prog, M, vec3(rotations.x, -rotations.y, -rotations.z), -legOrigin); // right leg
	}
	M->popMatrix();
}

void Spider::drawLeg(shared_ptr<Program> prog, shared_ptr<MatrixStack> M,
	vec3 rotations, vec3 translate)
{
	M->pushMatrix();
		M->rotate(rotations.y, YAXIS);
		M->rotate(rotations.x, XAXIS);
		M->rotate(rotations.z, ZAXIS);
		M->translate(translate);
		M->pushMatrix();
			M->rotate(M_PI_2, YAXIS);
			M->rotate(M_PI_2, XAXIS);
			drawPart(prog, M, vec3(0, translate.x, 1), sphereToLegScale);
		M->popMatrix();
		M->rotate(legBendAngle, YAXIS);
		drawPart(prog, M, sphereToLegScale);
	M->popMatrix();
}

void Spider::drawPart(shared_ptr<Program> prog, shared_ptr<MatrixStack> M, vec3 translation, vec3 scale)
{
	M->pushMatrix();
	M->translate(translation);
	drawPart(prog, M, scale);
	M->popMatrix();
}

void Spider::drawPart(shared_ptr<Program> prog, shared_ptr<MatrixStack> M, vec3 scale)
{
	M->pushMatrix();
	M->scale(scale);
	glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	sphere->draw(prog);
	M->popMatrix();
}

/*
 * Returns the default angles to rotate the legs in relation to each other
 */
vec3 Spider::defaultLegRotation(int legNum)
{
	float adjustment = M_PI / 4;
	float delta = -M_PI / 8;
	float leg = (legNum % 2 - 0.5) * 2;
	float timeAdjust = time + delta * legNum;
	float distanceAdjust = 4.0;

	// default rotation formula
	float xRot = 0;
	float yRot = (1.0 / 8.0 - adjustment * legNum) * leg / distanceAdjust;
	float zRot = -1.0 / distanceAdjust;
	return vec3(xRot, yRot, zRot);
}

/* 
 * Returns the default angles to rotate the legs in relation to each other and time
 * Mainly just an example of moving the legs (Replace the use of defaultLegRotation to see)
 */
vec3 Spider::defaultLegAnimation(int legNum)
{
	float adjustment = M_PI / 8;
	float delta = -M_PI / 8;
	float leg = (legNum % 2 - 0.5) * 2;
	float timeAdjust = time + delta * legNum;
	float distanceAdjust = 4.0;

	// default rotation formula
	float xRot = 0;
	float yRot = (sin(timeAdjust) / 8 - adjustment * legNum) * leg / distanceAdjust;
	float zRot = -abs(cos(timeAdjust)) / distanceAdjust;
	return vec3(xRot, yRot, zRot);
}
