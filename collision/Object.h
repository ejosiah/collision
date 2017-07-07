#pragma once

#include <ncl/gl/primitives.h>
#include <ncl/gl/Shader.h>
#include <glm/glm.hpp>
#include <ncl/gl/orientation.h>
#include <ncl/gl/Camera.h>

using namespace std;
using namespace ncl::gl;
using namespace glm;

class Object {
public:
	
	virtual void init() {

	}

	virtual void draw(Shader& shader, const Camera& camera) const {
		mat4 model = translate(mat4_cast(orientation), position);
		shader.sendComputed(camera, model);
		shape->draw(shader);
	}

	virtual void draw(const Camera& camera) const {
		draw(*shader, camera);
	}

	virtual void move(vec3& amount) {
		position += amount;
	}

	virtual void rotate(float yaw, float pitch, float roll){
		orientation *= Orientation({ radians(pitch), radians(yaw), radians(roll) });
	}

	virtual void update(float dt) {

	}

	void setShader(Shader& shader) {
		this->shader = &shader;
	}

protected:
	Shader* shader;
	Shape* shape;
	vec3 position;
	Orientation orientation;

};

class SphereObject : public Object {
public:
	SphereObject(float r = 0.5f) {
		shape = new Sphere(r, 50, 50);
		shape->material().ambient = shape->material().diffuse = randomColor();
	}
};

class TeaPotObject : public Object {
public:
	TeaPotObject() {
		shape = new Teapot(16);
		shape->material().ambient = shape->material().diffuse = randomColor();
	}
};