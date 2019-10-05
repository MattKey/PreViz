#pragma once

#include <glm/glm.hpp>

#include "Collider.h"
#include "ColliderMesh.h"
#include "PhysicsObject.h"
#include "BoundingBox.h"

using namespace glm;

class ColliderSphere : public Collider
{
public:
    ColliderSphere(float radius);

    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, Collider *col);
    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, ColliderMesh *col);
    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, ColliderSphere *col);
    virtual float getRadius(vec3 scale);

    float radius;
};