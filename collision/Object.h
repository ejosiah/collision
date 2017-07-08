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
		mat4 model = mat4_cast(orientation);
		model = translate(model, position);
		shader.sendComputed(camera, model);
		shape->draw(shader);
	}

	virtual void draw(const Camera& camera) const {
		draw(*shader, camera);
	}

	virtual void draw(Shader& shader, GlmCam& camera) const {
		mat4 model = translate(mat4(1), position);
		camera.model = model * mat4_cast(orientation);
		shader.sendComputed(camera);
		shape->draw(shader);
	}

	virtual void draw(GlmCam& camera) const {
		draw(*shader, camera);
	}

	virtual void move(const vec3& amount) {
		position += amount * 0.00011f;
	}

	virtual void rotate(const vec3& amount) {
		rotate(amount.x * 0.01f, amount.y * 0.01f , amount.z * 0.01f);
	}

	virtual void rotate(float pitch, float yaw, float roll){
		orientation *= Orientation({ radians(pitch), radians(yaw), radians(roll) });
	}

	virtual void update(float dt) {
		this->dt = dt;
	}

	void setShader(Shader& shader) {
		this->shader = &shader;
	}

protected:
	Shader* shader;
	Shape* shape;
	vec3 position;
	Orientation orientation;
	float angle = 30;
	float dt;

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