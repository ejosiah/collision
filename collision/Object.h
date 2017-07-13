#pragma once

#include <ncl/gl/primitives.h>
#include <ncl/gl/Shader.h>
#include <glm/glm.hpp>
#include <ncl/gl/orientation.h>
#include <ncl/gl/Camera.h>
#include <ncl/gl/UserCameraController.h>

using namespace std;
using namespace ncl::gl;
using namespace glm;

class Object {
public:
	
	virtual void init() {

	}

	virtual void draw(Shader& shader, const CameraController& camera) const {
		mat4 model = translate(mat4(1), _position);
		model = camera.getCamera().getMode() == Camera::ORBIT ? camera.modelTrans() * model : model;
		model = model * mat4_cast(orientation);
		shader.sendComputed(camera.getCamera(), model);
		shape->draw(shader);
	}

	virtual void draw(const CameraController& camera) const {
		draw(*shader, camera);
	}

	virtual void draw(Shader& shader, GlmCam& camera) const {
		mat4 model = translate(mat4(1), _position);
		camera.model = model * mat4_cast(orientation);
		shader.sendComputed(camera);
		shape->draw(shader);
	}

	virtual void draw(GlmCam& camera) const {
		draw(*shader, camera);
	}

	virtual void move(const vec3& amount) {
		_position += amount * 0.00011f;
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

	vec3 position() const { return _position;  }

protected:
	Shader* shader;
	Shape* shape;
	vec3 _position;
	Orientation orientation;
	float angle = 30;
	float dt;

};

class SphereObject : public Object {
public:
	SphereObject(float r = 0.5f) {
		shape = new Sphere(r);
	}
};

class TeaPotObject : public Object {
public:
	TeaPotObject() {
		shape = new Teapot(16);
	}
};

class CylinderObject : public Object {
public:
	CylinderObject() {
		shape = new Cylinder;
	}
};

class ConeObject : public Object {
public:
	ConeObject() {
		shape = new Cone;
	}
};

class TriangleObject : public Object {
public:
	TriangleObject() {
		Mesh mesh;
		mesh.positions.push_back({ -0.5, 0, 0 });
		mesh.positions.push_back({ 0.5, 0, 0 });
		mesh.positions.push_back({ 0, 1, 0 });
		vec4 color = randomColor();
		mesh.colors = vector<vec4>(3, color);
		mesh.normals = vector<vec3>(3, { 0, 1, 0 });
		shape = new ProvidedMesh(mesh);
	}
};