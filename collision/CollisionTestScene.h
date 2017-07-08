#pragma once
#include <ncl/gl/Scene.h>
#include <vector>
#include "Object.h"
#include <ncl/gl/UserCameraController.h>


using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;

class CollisionTestScene;

//class Mode {
//public:
//	virtual void init() = 0;
//	virtual string msg() = 0;
//	virtual void processInput() = 0;
//protected:
//	CollisionTestScene* scene;
//};

class SelectedObjectController : public _3DMotionEventHandler {
public:
	virtual void onMotion(const _3DMotionEvent& e) {
		string msg;
		msg = "translation[" + to_string(e.translation.x) + ", " + to_string(e.translation.y) + ", " + to_string(e.translation.z) + "]";
		logger.info(msg);
		msg = "rotation[" + to_string(e.rotation.x) + ", " + to_string(e.rotation.y) + ", " + to_string(e.rotation.z) + "]";
		logger.info(msg);
		if (_object) {
			_object->move(e.translation);
			_object->rotate(e.rotation);
		}
	};
	virtual void onNoMotion() {
	
	}

	void object(Object* obj) {
		_object = obj;
	}

	void clear() {
		_object = nullptr;
	}

private:
	Object* _object;
	Logger logger = Logger::get("Object Controller");
};


enum Mode { CREATE, CAMERA, SELECT };

class CollisionTestScene : public Scene {
public:
	CollisionTestScene() : Scene("Collision Test") {
		currentMode = CREATE;
		delete _motionEventHandler;
		_motionEventHandler = &objectController;
	}

	virtual void init() override {
		cameraController = CameraController{ Mesurements{ float(_width), float(_height) }, Camera::SPECTATOR };
		camera().lookAt({ 1.25, 1.0, 1 }, vec3(0), { 0, 1, 0 });
		cam.view = lookAt({ 0, 1, 1 }, vec3(0), { 0, 1, 0 });
		font = Font::Arial();

		auto object = new SphereObject;
		auto teapot = new TeaPotObject;
		//object->setShader(_shader);
		teapot->setShader(_shader);
	//	objects.push_back(teapot);
		light[0].transform = true;
		lightModel.colorMaterial = true;
	}

	virtual void display() override {
		displayText();
		for (auto object : objects) 
			object->draw(cam);
	}

	virtual void update(float dt) override {
		for (auto object : objects)
			object->update(dt);
	}

	virtual void resized() override {
		cameraController.updateAspectRation(aspectRatio);
		cam.projection = perspective(radians(60.0f), aspectRatio, 0.3f, 1000.0f);
		font->resize(_width, _height);
	}

	Camera& camera() {
		return const_cast<Camera&>(cameraController.getCamera());
	}

	void displayText() {
		stringstream ss;
		ss << "FPS: " << fps << endl;
		
		switch (currentMode) {
		case CREATE:
			ss << "Create Mode:" << endl;
			ss << "\t1 : Sphere";
			ss << "\t2 : Cube";
			ss << "\t3 : Cyninder";
			ss << "\t4 : Cone";
			ss << "\t5 : Teapot";
			break;
		}
		font->render(ss.str(), 10, _height - 20);
	}

	virtual void processInput(Keyboard& keyboard) override {
		Object* object = nullptr;
		switch (currentMode) {
		case CREATE:
			if (keyboard.NUM_1.pressed()) {
				object = new SphereObject;
			}
			else if (keyboard.NUM_5.pressed()) {
				object = new TeaPotObject;
			}
			break;
		}
		if (object) {
			object->init();
			object->setShader(_shader);
			objects.push_back(object);
			objectController.object(object);
		}
	}

private:
	vector<Object*> objects;
	Object* selectedObj;
	CameraController cameraController;
	Font* font;
	Mode currentMode;
	SelectedObjectController objectController;
};