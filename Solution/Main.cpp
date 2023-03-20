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


/* -- Notes/explanation of design choices --
*  At school (during free time where I have no other work to do) I get an error about not being able to do any git operations due to a self signed certificate thing.
	Anyway I disabled ssl verification so keep that in mind. I'm not sure if this only is local to my school computer or not.
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
* Rotations are clockwise
* 
* Event listeners are objects with an id because it is a more robust system where just in case you wanted to attach the same function to a specific event type you can.
* My original method was storing dictionary of function pointers where you can only attach a specific function to an event type once. This means if 
	you wanted to listen for frame start with the same function twice or more you couldn't.
* Hopefully this has only a little more overhead.
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
* 
* --- Known bugs/issues ---
* I believe that camera rotation doesn't rotate that nicely. I think it rotates the entire scene around (0,0) which produces a weird effect.
	A fix for this would involve moving the origin of rotation to be the axis of the camera position i think

* The stb_image library gives warning about like converting from a 4 byte to 8 byte number or something idk. Anyway I just ignore them because they're not a big deal.
* 


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
	
	// create sprite entity
	std::shared_ptr<Entity> sprite = std::make_shared<Entity>(); 

	sprite->transform.size = glm::vec3(400.0f, 400.f, 0.0f);
	sprite->transform.position.x = 200.0f;
	sprite->transform.position.y = 200.0f;
	sprite->transform.rotation.z = 45.0f;
	
	// create a new sprite renderer
	std::shared_ptr<SpriteRenderer> spriteRenderer = std::make_shared<SpriteRenderer>(zazaTexture);
	spriteRenderer->color = glm::vec3(1.0f, 0.0f, 0.0f); // red
	//renderer->color = glm::vec3(0.0f, 1.0f, 0.0f); // green
	//renderer->color = glm::vec3(0.0f, 0.0f, 1.0f); // blue
	
	// add to entity
	sprite->AddComponent(Entity::ComponentType::SpriteRenderer, spriteRenderer);
	//rect.AddComponent(Entity::ComponentType::SpriteRenderer, std::make_shared<SpriteRenderer>());

	scene->AddEntity("sprite", sprite);

	// create rect entity
	std::shared_ptr<Entity> rect = std::make_shared<Entity>();

	rect->transform.size = glm::vec3(100.0f, 100.f, 0.0f);
	rect->transform.position.x = 50.0f;
	rect->transform.position.y = 50.0f;
	//rect->transform.position.z = 1.0f;
	rect->transform.rotation.z = -22.5f;

	// create a new sprite renderer
	std::shared_ptr<RectangleRenderer> rectRenderer = std::make_shared<RectangleRenderer>();
	//rectRenderer->color = glm::vec3(1.0f, 0.0f, 0.0f); // red
	//rectRenderer->color = glm::vec3(0.0f, 1.0f, 0.0f); // green
	//rectRenderer->color = glm::vec3(0.0f, 0.0f, 1.0f); // blue

	// add to entity
	rect->AddComponent(Entity::ComponentType::RectangleRenderer, rectRenderer);

	scene->AddEntity("rect", rect);

	// create ellipse entity
	std::shared_ptr<Entity> ellipse = std::make_shared<Entity>();

	ellipse->transform.size = glm::vec3(400.0f, 200.f, 0.0f);
	ellipse->transform.position.x = 200.0f;
	ellipse->transform.position.y = 300.0f;
	ellipse->transform.rotation.z = 22.5f;

	// create a new sprite renderer
	std::shared_ptr<EllipseRenderer> ellipseRenderer = std::make_shared<EllipseRenderer>();
	//ellipseRenderer->color = glm::vec3(1.0f, 0.0f, 0.0f); // red
	ellipseRenderer->color = glm::vec3(0.0f, 1.0f, 0.0f); // green
	//ellipseRenderer->color = glm::vec3(0.0f, 0.0f, 1.0f); // blue
	//ellipseRenderer->color = glm::vec3(1.0f, 1.0f, 0.0f); // yellow

	// add to entity
	ellipse->AddComponent(Entity::ComponentType::EllipseRenderer, ellipseRenderer);

	scene->AddEntity("zellipse", ellipse);



	//unsigned int listenerId = scene->AddListener(Scene::EventType::Frame_End,EventListener(func));

	//std::cout << "Created a listener with id " << listenerId << std::endl;

	// set a breakpoint here if you need to check variables before they go into main loop
	std::cout << "checkpoint" << std::endl;

	// main loop that finishes when window is closed.
	while (!glfwWindowShouldClose(mainWindow))
	{
		// --- Update current scene ---

		ellipse->transform.rotation.z = glm::degrees((float)glfwGetTime());
		scene->Update();

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

