#pragma once
#include <ncl/gl/Scene.h>
#include <vector>
#include "Object.h"
#include <ncl/gl/UserCameraController.h>


using namespace std;
using namespace glm;
using namespace ncl::gl;

class CollisionTestScene : public Scene {
public:
	CollisionTestScene() : Scene("Collision Test") {

	}

	virtual void init() override {
		cameraController = CameraController{ Mesurements{ float(_width), float(_height) }, Camera::SPECTATOR };
		camera().lookAt({ 0, 1.0, 1 }, vec3(0), { 0, 1, 0 });
		font = Font::Arial();

		auto object = new SphereObject;
		auto teapot = new TeaPotObject;
		//object->setShader(_shader);
		teapot->setShader(_shader);
		objects.push_back(teapot);

		addKeyListener([&](const Key& key) {
			cameraController.processUserInput();
		});

		lightModel.colorMaterial = true;
	}

	virtual void display() override {
		displayText();
		for (auto object : objects) 
			object->draw(cameraController.getCamera());
	}

	virtual void update(float dt) override {
		for (auto object : objects)
			object->update(dt);
	}

	virtual void resized() override {
		cameraController.updateAspectRation(aspectRatio);
		font->resize(_width, _height);
	}

	Camera& camera() {
		return const_cast<Camera&>(cameraController.getCamera());
	}

	void displayText() {
		stringstream ss;
		ss << "FPS: " << fps;
		font->render(ss.str(), 10, _height - 20);
	}

private:
	vector<Object*> objects;
	CameraController cameraController;
	Font* font;
};