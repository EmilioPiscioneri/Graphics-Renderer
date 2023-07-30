#include <glad/glad.h>
#include <glfw3.h>
#include <iostream>
#include <glm/glm.hpp> // OpenGL maths: Include all GLM core / GLSL features
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>


// stuff i made
#include "SpriteRenderer.h"
#include "RectangleRenderer.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "EllipseRenderer.h"
#include "LineRenderer.h"
#include "RigidBody2D.h"
#include "FloatTween.h"
#include "Vec2Tween.h"
#include "Vec3Tween.h"
#include "BoxCollider.h"



/* -- Notes/explanation of design choices --
*  At school (during free time where I have no other work to do) I get an error about not being able to do any git operations due to a self signed certificate thing.
	Anyway I disabled ssl verification so keep that in mind. I'm not sure if this only is local to my school computer or not.
* 
* 
* Whenever soemthing is a colour I just use spelling of color because that's what most libraries will have
* Local coordinates are normalised as a value form -1 to 1 on x and y axis. These are the values of any loaded vertices.
* Global coordinates, assuming camera is not moved, start with bottom left of viewport as (0,0) and top right of viewport 
	as (width, height) of window in pixels
* Note that 2 local units equal 1 global unit and almost everything apart from vertices uses global units (like size and position)
* so global units need to be multiplied by 2 to get a ratio of 1:1 because ratio starts as 2:1 with left as local and right being global
* when you do global*2 or right hand side * 2 you get 2:(1*2) -> 2:2 = 1:1
* But that math is done by my code so it is not a worry when using my transform class which handles that
* 
* Rotations are clockwise. Rotations use radians because that's just standard for maths but also because it prevents a degrees to radians function running each
	time you want to do calculations on a transform
* 
* Event listeners are objects with an id because it is a more robust system where just in case you wanted to attach the same function to a specific event type you can.
* My original method was storing dictionary of function pointers where you can only attach a specific function to an event type once. This means if 
	you wanted to listen for frame start with the same function twice or more you couldn't.
* Hopefully this has only a little more overhead. Also I use std::function because it's so much more useful and I hope overhead isn't an issue
* 
* I use a lot of smart pointers because it is my understanding that they are not a bad practice and don't lead to much overhead. Also will save headaches
	if I kept forgetting to delete created objects on the heap. Last time I attempted to make a graphics engine I had a memory leak so yeah.
* 
* When I use templates for things like maps I am doing so in anticipation that the codebase may get bigger and from what I understand only compiler performance/speed is 
	impacted when using templates.
* 
* For rendering ellipses I chose to just use a rectangle with a circle shader that is stretched via size because it is always going to scale no matter what and you won't see
	the edges if it was just an ellipse with 12 vertices or seomthin.

* Whenever you're setting uniforms make sure to actually Use() the shaderProgram first and then assign uniforms
* 
* Components under entities are all derived from the Component class so an entity stores pointers to each component as a pointer to the base class. I then make use of generics
	when you want to get a component. Due to the components being stored as a base class the way I ended up making the get component function was by passing in the
	derived component's class through the generic parameter or whatever you call it (the one in <>) but also require you to pass in an enum of that exact derived class' name.
	I do this because I found it to be the best solution for c++. The idea is that each entity can only have an instance of one component type so they are indexed by the
	the derived component's class name as an enum. You then pass in the actual Class in the generic type parameter which casts the base component to pointer 
	of the derived componet. Hope that makes sense. Anyway point is I was stuck on how to actually implement this feature and this was the solution I came up with.
	May not be pretty but it is fairly simple and works

* The camera faces towards the negative z-axis. Keep this in mind if you ever want to specifically set something in front of another. If you have objects a and b and you want
	a to be in front of be you would set a to -1 (smaller number is closer to screen) and b to -2 on the z axis of the scene.
	HOWEVER, in order to avoid this I implemented a zIndex which is like a layering system where greater numbers appear on top and smaller numbers appear behind in the scene.
	It really just puts different values on the negative z axis in the scene. If you're familiar with CSS this won't be a foreign concept.
* 
* If an entity has any kinda transparency you MUST do entity.SetHasTransparency(true) so that the scene is rendered correctly
* 
* No two entities can share the same name under a scene
* 
* I implemented getters and setters for a few properties of classes solely because they need to update something else whenever the properties are changed.
	The ellipse renderer doesn't need a getter and setter for alpha because it always needs to be transparent due to smoothing effect taking advantage of alpha
	but I still keep it so the structure of renderer classes follows
* 
* I don't discard any fragments because https://stackoverflow.com/questions/8509051/is-discard-bad-for-program-performance-in-opengl.
	I just use 0.0 alpha because I set up the engine to be able to use blending. 
* 
* The higher your max zIndex for a scene is, the greater a number the far plane of scene camera must be

* If you turn autoUpdateFarPlane off for a scene, it will only render any entities where (scene.highestZIndex - entity.zIndex) <  mainCamera.farPlane
* 
* If you're using a default transform constructor beware that the default values are (0,0) or (0,0,0) so you have to actually set them to see a value on the screen
* 
* Due to c++ not having actual support for static classes like c# and it just seeming better to use a normal class, I just decided to make the TweenManager
	its own normal class.

* ZIndex is under entity.transform because zIndex is related to the positioning of an entity which is a transformation
* 
* I used std::function for tween setters because it allows you to pass in setters for object methods. This is just something that is really useful with the
	architecture of my current codebase and allows custom implementations of tweened values
* 
* Tweens have really big constructors because the initial values should only be set once. This is probably a bad practice but hey it's not awful in my opinion
* 
* I don't account for static to static collisions because they're meant to stay still so we don't really care if they're inside each other or whatever
* 
* When you wanna do a shared pointer of this you have to inherit public std::enable_shared_from_this< ClassName > and then call shared_from_this()
* 
* If you get errors from shared_from_this you likely didn't inherit its public values so make sure to do public std::enable_shared_from_this< ClassName > 
* 
* When non-static objects collide, any relative values they had for position will be removed. I have to add support for this which is not in scope rn
* 
* The physics engine thingy system is tied to time not frame rate. This causes issues with things like lag where an object can clip out of scene. Fixing such
	such issues is not something I will cover for a while
* 
* --- Known bugs/issues ---
* I believe that camera rotation doesn't rotate that nicely. I think it rotates the entire scene around (0,0) which produces a weird effect.
	A fix for this would involve moving the origin of rotation to be the axis of the camera position i think

* The stb_image library gives warning about like converting from a 4 byte to 8 byte number or something idk. Anyway I just ignore them because they're not a big deal.
* --- limitations ---
* Only 2D
*
* I don't include support for geometry shaders. Maybe later
*
* The ellipse renderer doesn't support rotations on the x and y axis because it requires me to learn quarternions. I'm still in pre-calculus (1&2 methods) so maybe when 
	I get to calculus I'll be willing to tackle that problem. For now I'm sticking to 2D rotations (rotation along the z axis). It also involves me dealing with the
	complex plane which I don't wanna do. I'm making a 2D renderer anyway not 3D. It's an orthographic view as well smh my head.
*/

// defualt fragment shader
const char* defaultFragShaderPath = "FragmentShaders/Default.frag";
// default vertex shader
const char* defaultVertShaderPath = "VertexShaders/Default.vert";
const char* defaultTexture = "Textures/ZazaWolf.jpg";

const int defaultWindowWidth = 800;
const int defaultWindowHeight = 800;
const bool wireframeMode = false; // whether or not wireframe mode is activated (only show outline of primitives) and no fill
const unsigned int antiAliasingSamples = 4; // how many samples openGL's anti aliasing functionality uses (MSAA). More samples per pixel means more chance an object will appear smoother cos more hit points

// scene gets intialised in main function
std::unique_ptr<Scene> scene;

// declared functions
static void windowReSizeCallback(GLFWwindow* window, int width, int height);

void func(EventInfo e) {
	std::cout << "Fired an event" << std::endl;
}

// main code
int main() {
	// initialise glfw
	glfwInit();
	// Configure glfw so it knows we are using version 3 of opengl (3.3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, antiAliasingSamples); // turn on anti aliasing
	// Core profile removes backwards compatability because we won't use those other functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create window pointer object
	GLFWwindow* mainWindow = glfwCreateWindow(defaultWindowWidth, defaultWindowHeight, "Main window", NULL, NULL);
	// check if it successfuly created
	if (mainWindow == NULL)
	{
		std::cout << "ERROR: Failed to create GLFW window" << std::endl;
		// cleanup project
		glfwTerminate();
		return -1;
	}
	// idk makes this window the main one or somethin
	glfwMakeContextCurrent(mainWindow);

	// assign the opengl function that uses function pointers to do stuff?
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "ERROR: Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// intialise a new scene
	scene = std::make_unique<Scene>(mainWindow, defaultWindowWidth, defaultWindowHeight);

	// attach callback for when window is resized
	glfwSetFramebufferSizeCallback(mainWindow, windowReSizeCallback);

	// --- config ---
	// enable depth testing to ensure opengl takes into account depth when rendering
	glEnable(GL_DEPTH_TEST);
	// ensures openGL doesn't mess with images that don't have dimensions divisible by 4
	// Explanation: https://stackoverflow.com/questions/11042027/glpixelstoreigl-unpack-alignment-1-disadvantages
	// Issues when this function isn't called: https://stackoverflow.com/questions/11042027/glpixelstoreigl-unpack-alignment-1-disadvantages
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// enable transparency (alpha channel)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendEquation(GL_FUNC_ADD);
	//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);


	// - Shaders

	// create a shader porgram using path
	ShaderProgram* shaderProgram = ResourceManager::LoadShaderProgram("program", defaultVertShaderPath, defaultFragShaderPath);

	// --- textures ---

	Texture2D* zazaTexture = ResourceManager::LoadTexture("ZazaWolf", defaultTexture, false);

	// wireframe mode
	if(wireframeMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// create rect entity
	std::shared_ptr<Entity> rect = std::make_shared<Entity>();

	rect->transform.offsetSize = glm::vec3(100.0f, 100.f, 0.0f);
	rect->transform.offsetPosition.x = 250.0f;
	rect->transform.offsetPosition.y = 250.0f;
	rect->transform.SetZIndex(2);

	rect->transform.rotation.z = glm::radians(-22.5f);

	// create a new rect renderer
	std::shared_ptr<RectangleRenderer> rectRenderer = std::make_shared<RectangleRenderer>();
	//rectRenderer->color = glm::vec3(1.0f, 0.0f, 0.0f); // red
	//rectRenderer->color = glm::vec3(0.0f, 1.0f, 0.0f); // green
	rectRenderer->color = glm::vec3(0.0f, 0.0f, 1.0f); // blue
	rectRenderer->SetAlpha(0.9f);

	// add to entity
	rect->AddComponent(Entity::RectangleRenderer, rectRenderer);

	scene->AddEntity("rect", rect);

	// create rect2 entity
	std::shared_ptr<Entity> rect2 = std::make_shared<Entity>();

	rect2->transform.offsetSize = glm::vec3(100.0f, 100.f, 0.0f);
	rect2->transform.offsetPosition.x = 500.0f;
	rect2->transform.offsetPosition.y = 250.0f;
	rect2->transform.SetZIndex(4);

	rect2->transform.rotation.z = glm::radians(22.5f);

	// create a new rect renderer
	std::shared_ptr<RectangleRenderer> rectRenderer2 = std::make_shared<RectangleRenderer>();
	//rectRenderer->color = glm::vec3(1.0f, 0.0f, 0.0f); // red
	rectRenderer2->color = glm::vec3(0.0f, 1.0f, 0.0f); // green
	//rectRenderer2->color = glm::vec3(0.0f, 0.0f, 1.0f); // blue
	rectRenderer2->SetAlpha(1.0f);

	// add to entity
	rect2->AddComponent(Entity::RectangleRenderer, rectRenderer2);

	scene->AddEntity("rect2", rect2);


	// create sprite entity
	std::shared_ptr<Entity> sprite = std::make_shared<Entity>(); 

	sprite->transform.offsetSize = glm::vec3(114.234, 73.267f, 0.0f);
	sprite->transform.offsetPosition.x = 200.0f;
	sprite->transform.offsetPosition.y = 200.0f;
	sprite->transform.SetZIndex(10);
	//sprite->transform.rotation.z = glm::radians(45.0f);
	
	
	// create a new sprite renderer
	std::shared_ptr<SpriteRenderer> spriteRenderer = std::make_shared<SpriteRenderer>(zazaTexture);
	//spriteRenderer->color = glm::vec3(1.0f, 0.0f, 0.0f); // red
	//renderer->color = glm::vec3(0.0f, 1.0f, 0.0f); // green
	//renderer->color = glm::vec3(0.0f, 0.0f, 1.0f); // blue
	//spriteRenderer->SetAlpha(0.7f);

	// add to entity
	sprite->AddComponent(Entity::SpriteRenderer, spriteRenderer);
	//rect.AddComponent(Entity::ComponentType::SpriteRenderer, std::make_shared<SpriteRenderer>());

	// add a rigid body 2D
	std::shared_ptr<RigidBody2D> spriteRigidBody = std::make_shared<RigidBody2D>();
	// launch angle (anti clockwise)
	constexpr float launchAngle = glm::radians(56.3f);
	// how many metres to launch by
	float launchSize = 6.0f;

	// math is simply just basic unit circle stuff * magnitude
	glm::vec2 velocity = glm::vec2(cos(launchAngle), sin(launchAngle)) * launchSize;

	spriteRigidBody->velocity = velocity;
	spriteRigidBody->isStatic = true;
	spriteRigidBody->gravityScale = 1.0f;
	//spriteRigidBody->linearDrag = 0.7f; // exaggerated drag
	
	// add to entity
	sprite->AddComponent(Entity::RigidBody2D, spriteRigidBody);

	std::shared_ptr<BoxCollider> spriteCollider = std::make_shared<BoxCollider>();
	sprite->AddComponent(Entity::BoxCollider, spriteCollider); // attach to entity
	//spriteCollider->size = glm::vec2(2.0f,2.0f);
	//spriteCollider->position = glm::vec2(-50.0f, -50.0f);

	spriteRigidBody->SetAttachedCollider(spriteCollider); // attach the collider
	


	

	// -- DEBUGGING -- create 4 lines that connect each bounding box of the collider
	/*auto colliderVertices = spriteCollider->GetBoundingVertices(scene->mainCamera);
	auto colLine1 = std::make_shared<Entity>(Transform(glm::vec2(0.0f,0.0f),glm::vec3(1.0f,1.0f,0.0f)));
	colLine1->transform.SetZIndex(15);
	auto colLineRenderer1 = std::make_shared<LineRenderer>(colliderVertices[0], colliderVertices[1]);
	colLine1->AddComponent(Entity::LineRenderer, colLineRenderer1);
	scene->AddEntity("colLine1", colLine1);
	auto colLine2 = std::make_shared<Entity>(Transform(glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f)));
	colLine2->transform.SetZIndex(15);
	auto colLineRenderer2 = std::make_shared<LineRenderer>(colliderVertices[1], colliderVertices[3]);
	colLine2->AddComponent(Entity::LineRenderer, colLineRenderer2);
	scene->AddEntity("colLine2", colLine2);
	auto colLine3 = std::make_shared<Entity>(Transform(glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f)));
	colLine3->transform.SetZIndex(15);
	auto colLineRenderer3 = std::make_shared<LineRenderer>(colliderVertices[3], colliderVertices[2]);
	colLine3->AddComponent(Entity::LineRenderer, colLineRenderer3);
	scene->AddEntity("colLine3", colLine3);
	auto colLine4 = std::make_shared<Entity>(Transform(glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f)));
	colLine4->transform.SetZIndex(15);
	auto colLineRenderer4 = std::make_shared<LineRenderer>(colliderVertices[2], colliderVertices[0]);
	colLine4->AddComponent(Entity::LineRenderer, colLineRenderer4);
	scene->AddEntity("colLine4", colLine4);*/
	// -- DEBUGGING END --
	

	scene->AddEntity("sprite", sprite);

	// create sprite entity
	std::shared_ptr<Entity> sprite2 = std::make_shared<Entity>();

	sprite2->transform.offsetSize = glm::vec3(200.0f, 100.f, 0.0f);
	sprite2->transform.offsetPosition.x = 500.0f;
	sprite2->transform.offsetPosition.y = 300.0f;
	sprite2->transform.SetZIndex(11);
	//sprite->transform.rotation.z = glm::radians(45.0f);


	// create a new sprite renderer
	std::shared_ptr<SpriteRenderer> spriteRenderer2 = std::make_shared<SpriteRenderer>(zazaTexture);
	// add to entity
	sprite2->AddComponent(Entity::SpriteRenderer, spriteRenderer2);

	// add a rigid body 2D
	std::shared_ptr<RigidBody2D> spriteRigidBody2 = std::make_shared<RigidBody2D>();
	
	// launch angle (anti clockwise)
	constexpr float launchAngle2 = glm::radians(180-80.3f);
	// how many metres to launch by
	float launchSize2 = 2.0f;

	// math is simply just basic unit circle stuff * magnitude
	glm::vec2 velocity2 = glm::vec2(cos(launchAngle2), sin(launchAngle2)) * launchSize2;

	spriteRigidBody2->velocity = velocity2;
	spriteRigidBody2->isStatic = true;
	spriteRigidBody2->gravityScale = 1.0f;

	// add to entity
	sprite2->AddComponent(Entity::RigidBody2D, spriteRigidBody2);

	std::shared_ptr<BoxCollider> spriteCollider2 = std::make_shared<BoxCollider>();
	sprite2->AddComponent(Entity::BoxCollider, spriteCollider2); // attach to entity
	//spriteCollider->size = glm::vec2(2.0f,2.0f);
	//spriteCollider->position = glm::vec2(-50.0f, -50.0f);

	spriteRigidBody2->SetAttachedCollider(spriteCollider2); // attach the collider


	scene->AddEntity("sprite2", sprite2);


	//sprite->RemoveComponent(Entity::BoxCollider); // detatch collider

	//spriteRigidBody->SetAttachedCollider(nullptr); // detatch the collider


	

	// create ellipse entity
	std::shared_ptr<Entity> ellipse = std::make_shared<Entity>();

	//ellipse->transform.type = Transform::Sticky;

	//ellipse->transform.offsetSize = glm::vec3(400.0f, 200.f, 0.0f);
	ellipse->transform.relativeSize = glm::vec2(0.5f, 0.25f);
	//ellipse->transform.offsetPosition.x = 200.0f;
	//ellipse->transform.offsetPosition.y = 300.0f;
	ellipse->transform.relativePosition.x = 0.25f;
	ellipse->transform.relativePosition.y = 0.375f;
	ellipse->transform.SetZIndex(3);

	//ellipse->transform.rotation.z = -22.5f;

	// create a new sprite renderer
	std::shared_ptr<EllipseRenderer> ellipseRenderer = std::make_shared<EllipseRenderer>();
	//ellipseRenderer->color = glm::vec3(1.0f, 0.0f, 0.0f); // red
	//ellipseRenderer->color = glm::vec3(0.0f, 1.0f, 0.0f); // green
	//ellipseRenderer->color = glm::vec3(0.0f, 0.0f, 1.0f); // blue
	ellipseRenderer->color = glm::vec3(1.0f, 1.0f, 0.0f); // yellow
	ellipseRenderer->SetAlpha(0.6f);

	// add to entity
	ellipse->AddComponent(Entity::EllipseRenderer, ellipseRenderer);

	scene->AddEntity("ellipse", ellipse);

	// Create a line entity

	std::shared_ptr<Entity> line = std::make_shared<Entity>();
	// actually make it visible
	line->transform.offsetSize = glm::vec3(1.0f, 1.0f, 0.0f);

	glm::vec2 point1 = glm::vec2(25.0f, 25.0f);
	glm::vec2 point2 = glm::vec2(100.0f, 200.0f);
	float lineThickness = 4.0f;

	std::shared_ptr<LineRenderer> lineRenderer = std::make_shared<LineRenderer>(point1, point2, lineThickness);

	//lineRenderer->color = glm::vec3(1.0f,0.0f,0.0f); // red
	//lineRenderer->color = glm::vec3(0.0f, 1.0f, 0.0f); // green
	//lineRenderer->color = glm::vec3(0.0f, 0.0f, 1.0f); // blue

	line->transform.SetZIndex(5);
	//lineRenderer->SetAlpha(0.9f);

	// add to entity
	line->AddComponent(Entity::LineRenderer, lineRenderer);

	scene->AddEntity("line", line);

	//unsigned int listenerId = scene->AddListener(Scene::EventType::Frame_End,EventListener(func));

	//std::cout << "Created a listener with id " << listenerId << std::endl;

	
	// testing tween funcitonality 
	
	/*
	std::shared_ptr<FloatTween> rotationZTween = std::make_shared<FloatTween>(&ellipse->transform.rotation.z, 0.0f, glm::radians(360.0f), 2.5f);
	scene->tweenManager.AddTween(rotationZTween, false);
	rotationZTween->Start();

	std::shared_ptr<Vec2Tween> tweenPos = std::make_shared<Vec2Tween>(&ellipse->transform.offsetPosition, glm::vec2(0.0f,0.0f), glm::vec2(400.0f, -400.0f), 2.5f);
	scene->tweenManager.AddTween(tweenPos);

	// callback to point1 setter
	Vec2Tween::SetterCallback p1setter = std::bind(&LineRenderer::SetPoint1,lineRenderer, std::placeholders::_1);

	std::shared_ptr<Vec2Tween> linePos1Tween = std::make_shared<Vec2Tween>(p1setter, point1, glm::vec2(400.0f, 700.0f), 2.5f);
	scene->tweenManager.AddTween(linePos1Tween);

	// callback to point2 setter
	Vec2Tween::SetterCallback p2setter = std::bind(&LineRenderer::SetPoint2, lineRenderer, std::placeholders::_1);

	std::shared_ptr<Vec2Tween> linePos2Tween = std::make_shared<Vec2Tween>(p2setter, point2, glm::vec2(700.0f, 700.0f), 1.3f, 2.5f);
	scene->tweenManager.AddTween(linePos2Tween);
	//*/
	
	// after this many seconds since window start, launch sprite
	double launchTime = 4.0f;
	bool launched = false;

	// after this many seconds stop tracking sprite to mouse
	double mouseStickTime = 4.0f;
	bool sticking = true;

	// set a breakpoint here if you need to check variables before they go into main loop
	std::cout << "checkpoint" << std::endl;


	// temp camera movement config
	float camSpeed = 500.0f; // how many global coords cam moves per second

	// main loop that finishes when window is closed.
	while (!glfwWindowShouldClose(mainWindow))
	{
		// --- Update current scene ---
		
		// temp camera movement code

		float deltaTime = (float)scene->deltaTime;

		if (sticking && glfwGetTime() > mouseStickTime)
		{
			sticking = false;
		}
		else if(sticking) {
			// attach sprite to mouse
			double xPos = 0.0f;
			double yPos = 0.0f;
			glfwGetCursorPos(mainWindow, &xPos, &yPos);
			yPos = scene->mainCamera->height - yPos; // convert from top-left based to bottom-left
			sprite->transform.offsetPosition = glm::vec2((float)xPos, (float)yPos);
		}

		if (!launched && glfwGetTime() >= launchTime)
		{
			launched = true;
			spriteRigidBody->isStatic = false; // turn on
			spriteRigidBody2->isStatic = false; // turn on

		}

		

		// if W pressed
		if (glfwGetKey(mainWindow, GLFW_KEY_W) == GLFW_PRESS)
			scene->mainCamera->position.y += camSpeed * deltaTime;
		// if A pressed
		if (glfwGetKey(mainWindow, GLFW_KEY_A) == GLFW_PRESS)
			scene->mainCamera->position.x -= camSpeed * deltaTime;
		// if S pressed
		if (glfwGetKey(mainWindow, GLFW_KEY_S) == GLFW_PRESS)
			scene->mainCamera->position.y -= camSpeed * deltaTime;
		// if D pressed
		if (glfwGetKey(mainWindow, GLFW_KEY_D) == GLFW_PRESS)
			scene->mainCamera->position.x += camSpeed * deltaTime;

		

		// rotate ellipse (revolutions are every 2*pi seconds)
		//ellipse->transform.rotation.z = (float)glfwGetTime();
		scene->Update();
		
		// print rigid body velocity
		//std::cout << "("<< spriteRigidBody->velocity.x << ", " << spriteRigidBody->velocity.y << ")" << std::endl;

		//float timeSinceStart = (float)glfwGetTime(); // time since start of window
		//shaderProgram.setFloat("sinTime", sin(timeSinceStart) / 2.0f + 0.5f); // normalise sin(time since start of application) to be a value between 0-1 based
		

		

	}

	

	// cleanup 
	glfwTerminate();
	return 0;
}

void windowReSizeCallback(GLFWwindow* window, int width, int height) {
	// update projection using new width and height
	scene->mainCamera->UpdateProjection((float) width, (float)height);
	// Set size and location of rendering window. first 2 params is location of lower left corner of window. Last two are size of window
	glViewport(0, 0, width, height);
}

