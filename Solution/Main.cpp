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


/* -- Notes --
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
* 
* 
* --- Known bugs/limitations ---
* Only 2D
* 
* I believe that camera rotation doesn't rotate that nicely. I think it rotates the entire scene around (0,0) which produces a weird effect.
	A fix for this would involve moving the origin of rotation to be the axis of the camera position i think
*
* I don't include support for geometry shaders.
* 
* The stb_image library gives warning about like converting from a 4 byte to 8 byte number or something idk. Anyway I just ignore them because they're not a big deal.
*/

// defualt fragment shader
const char* defaultFragShaderPath = "FragmentShaders/Default.frag";
// default vertex shader
const char* defaultVertShaderPath = "VertexShaders/Default.vert";
const char* defaultTexture = "Textures/ZazaWolf.jpg";

const int defaultWindowWidth = 800;
const int defaultWindowHeight = 800;
const bool wireframeMode = false; // whether or not wireframe mode is activated (only show outline of primitives) and no fill

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
	// ensures openGL doesn't fuck with images that don't have dimensions divisible by 4 
	// Explanation: https://stackoverflow.com/questions/11042027/glpixelstoreigl-unpack-alignment-1-disadvantages
	// Issues when this function isn't called: https://stackoverflow.com/questions/11042027/glpixelstoreigl-unpack-alignment-1-disadvantages
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

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

	scene->AddEntity("rect", sprite);

	// create rect entity
	std::shared_ptr<Entity> rect = std::make_shared<Entity>();

	rect->transform.size = glm::vec3(100.0f, 100.f, 0.0f);
	rect->transform.position.x = 50.0f;
	rect->transform.position.y = 50.0f;
	rect->transform.rotation.z = -22.5f;

	// create a new sprite renderer
	std::shared_ptr<RectangleRenderer> rectRenderer = std::make_shared<RectangleRenderer>();
	//rectRenderer->color = glm::vec3(1.0f, 0.0f, 0.0f); // red
	//rectRenderer->color = glm::vec3(0.0f, 1.0f, 0.0f); // green
	//rectRenderer->color = glm::vec3(0.0f, 0.0f, 1.0f); // blue

	// add to entity
	rect->AddComponent(Entity::ComponentType::RectangleRenderer, rectRenderer);

	scene->AddEntity("rect", rect);



	//unsigned int listenerId = scene->AddListener(Scene::EventType::Frame_End,EventListener(func));

	//std::cout << "Created a listener with id " << listenerId << std::endl;

	// set a breakpoint here if you need to check variables before they go into main loop
	std::cout << "checkpoint" << std::endl;

	// main loop that finishes when window is closed.
	while (!glfwWindowShouldClose(mainWindow))
	{
		// --- Update current scene ---

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

