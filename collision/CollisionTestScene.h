#pragma once
#include <algorithm>
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

enum Mode { CREATE, CAMERA, SELECT, VIEW, NONE };

enum CamView { FRONT, LEFT, TOP, PERSPECTIVE };

class CollisionTestScene;

class SelectedObjectController : public _3DMotionEventHandler {
public:
	SelectedObjectController(CollisionTestScene& scene) :scene(scene) {}

	virtual void onMotion(const _3DMotionEvent& e) override;

	virtual void onNoMotion() override {
	
	}

	void set(Object* obj) {
		_object = obj;
	}

	void clear() {
		_object = nullptr;
	}

private:
	CollisionTestScene& scene;
	Object* _object;
	Logger logger = Logger::get("Object Controller");
};

class CollisionTestScene : public Scene {
	friend class SelectedObjectController;
public:
	CollisionTestScene() : Scene("Collision Test") {
		currentMode = VIEW;
		currentView = PERSPECTIVE;
		delete _motionEventHandler;
		objectController = new SelectedObjectController(*this);
		_motionEventHandler = objectController;
		_requireMouse = true;
		_useImplictShaderLoad = true;
	}

	virtual void init() override {
		cam.view = views[currentView];
		font = Font::Arial();

		for (int i = 0; i < 4; i++) {
			viewState[i] = views[i];
		}

		light[0].transform = true;
		lightModel.colorMaterial = true;

		_mouseClickListners.push_back([&](Mouse& mouse) {
			if (mouse.left.status == Mouse::Button::PRESSED) {
				float winZ = 0;
				float x = mouse.pos.x;
				float y = _height - mouse.pos.y;
				glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
				vec3 point = unProject(vec3(x, y, winZ), cam.view, cam.projection, vec4(0, 0, _width, _height));
				stringstream ss;
				ss << "selected point: (" << point.x << ", " << point.y << ", " << point.z << ")";
				logger.info(ss.str());

				float minDist = 1000;
				auto itr = find_if(objects.begin(), objects.end(), [&](Object* obj) {
					float dist = distance(obj->position(), point);
					return dist < 1 && dist < minDist;
				});

				if (itr != objects.end()) {
					currentMode = SELECT;
					selectedObj = *itr;
					objectController->set(selectedObj);
				}
				else {
					currentMode = VIEW;
					selectedObj = nullptr;
					objectController->clear();
				}
			}
		});

		shader("lighting").use();
		shader("lighting").sendUniform1f("edgeWidth", 0.015f);
		shader("lighting").sendUniform1f("pctExtend", 0.25f);
		shader("lighting").sendUniform4f("lineColor", 0.0f, 1.0f, 1.0f, 1.0f);
		sendLights("lighting");
		shader("lighting").send(lightModel);
	}

	virtual void display() override {
		displayText();
		shader("lighting").send(lightModel);
		for (auto object : objects) {
			shader("lighting").send("isSelected", false);
			if (object == selectedObj) shader("lighting").send("isSelected", true);
			object->draw(cam);
		}
	}

	virtual void update(float dt) override {
		for (auto object : objects) {
			object->update(dt);
		}
	}

	virtual void resized() override {
		cam.projection = perspective(radians(60.0f), aspectRatio, 0.3f, 1000.0f);
		font->resize(_width, _height);
	}

	void displayText() {
		stringstream ss;
		ss << "FPS: " << fps << endl;
		
		switch (currentMode) {
		case NONE:
			ss << "v: View, c: create" << endl;
		case CREATE:
			ss << "Create Mode:" << endl;
			ss << "\t1 : Triangle";
			ss << "\t2 : Sphere";
			ss << "\t3 : Cube";
			ss << "\t4 : Cyninder";
			ss << "\t5 : Cone";
			ss << "\t6 : Teapot";
			break;
		case VIEW:
			ss << "View Mode:" << endl;
			ss << "\t1 : top view";
			ss << "\t2 : Font view";
			ss << "\t3 : Left";
			ss << "\t4 : Perspective";
			ss << "\t5 : Reset view";
			break;
		case SELECT:
			ss << "Object select Mode:" << endl;
			break;
		default:
			break;
		}

		font->render(ss.str(), 10, _height - 20);
	}

	virtual void processInput(Keyboard& keyboard) override {
		if (keyboard.V.pressed()) {
			currentMode = VIEW;
		}
		else if (keyboard.C.pressed()) {
			currentMode = CREATE;
		}

		Object* object = nullptr;
		switch (currentMode) {
		case CREATE:
			if (keyboard.NUM_1.pressed()) {
				object = new TriangleObject;
			}
			else if (keyboard.NUM_3.pressed()) {
				object = new SphereObject;
			}
			else if (keyboard.NUM_6.pressed()) {
				object = new TeaPotObject;
			}
			else if (keyboard.NUM_4.pressed()) {
				object = new CylinderObject;
			}
			else if (keyboard.NUM_5.pressed()) {
				object = new ConeObject;
			}
			break;
		case VIEW:
			viewState[currentView] = cam.view;
			if (keyboard.NUM_1.pressed()) {
				currentView = TOP;
			}
			else if (keyboard.NUM_2.pressed()) {
				currentView = FRONT;
			}
			else if (keyboard.NUM_3.pressed()) {
				currentView = LEFT;
			}
			else if (keyboard.NUM_4.pressed()) {
				currentView = PERSPECTIVE;
			}
			cam.view = viewState[currentView];
			if (keyboard.NUM_5.pressed()) {
				cam.view = views[currentView];
			}
			break;
		case SELECT:
			if (keyboard.D.pressed()) {
				auto itr = find(objects.begin(), objects.end(), selectedObj);
				if (itr != objects.end()) {
					objects.erase(itr);
					delete selectedObj;
					selectedObj = nullptr;
					currentMode = VIEW;
				}
			}
			break;

		}
		if (object) {
			object->init();
			object->setShader(shader("lighting"));
			objects.push_back(object);
			currentMode = VIEW;
		}
	}

private:
	vector<Object*> objects;
	Object* selectedObj;
	Font* font;
	Mode currentMode;
	CamView currentView;
	SelectedObjectController* objectController;
	Logger logger = Logger::get("Collision Scene");
	mat4 views[4] = {
		lookAt({ 0, 0, 3 }, vec3(0), { 0, 1, 0 }),
		lookAt({ 3, 0, 0 }, vec3(0), { 0, 1, 0 }),
		lookAt({ 0, 3, 0 }, vec3(0), { 0, 0, 1 }),
		lookAt({ 0, 1.25, 3 }, vec3(0), { 0, 1, 0 }),
	} ;
	mat4 viewState[4];
};

void SelectedObjectController::onMotion(const _3DMotionEvent& e) {
	
	vec3 translation = e.translation;
	vec3 rotation = e.rotation;

	if (scene.currentView == FRONT) {
		translation.z = 0;
	}
	if (scene.currentView == LEFT) {
		translation.x = 0;
	}
	if (scene.currentView == TOP) {
		translation.y = 0;
	}

	if (_object && scene.currentMode == SELECT) {
		_object->move(translation);
		_object->rotate(rotation);
	}
	else if (scene.currentMode == VIEW && scene.currentView == PERSPECTIVE) {
		mat4 model = translate(mat4(1), translation * 0.00011f);
		model = rotate(model, radians(rotation.z *  0.01f), { 0, 0, 1 });
		model = rotate(model, radians(rotation.y * 0.01f), { 0, 1, 0 });
		model = rotate(model, radians(rotation.x * 0.01f), { 1, 0, 1 });
		scene.cam.view = scene.cam.view * model;
	}
};