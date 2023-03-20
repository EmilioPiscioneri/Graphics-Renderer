#include "Scene.h"
#include "SpriteRenderer.h"
#include "RectangleRenderer.h"
#include "EllipseRenderer.h"


Scene::Scene(GLFWwindow* window, float width, float height, glm::vec3 backgroundColor, std::shared_ptr<OrthoCamera> camera)
{
	this->_window = window;
	this->width = width;
	this->height = height;
	this->backgroundColor = backgroundColor;
	// If didn't provide a camera
	if (camera == nullptr)
		// create a default camera
		mainCamera = std::make_shared<OrthoCamera>(width, height);
	else
		// else use given camera
		mainCamera = camera;
	// setup scene
	Initialise();
}

Scene::~Scene()
{
	// cleanup scene
}

void Scene::AddEntity(std::string name, std::shared_ptr<Entity> entity)
{
	if (entity == nullptr)
		throw std::exception("ERROR: Tried to add a nullptr to scene");

	// Make sure the desired name hasn't been taken, if so keep adding "1" until a valid name is found
	name = GetValidNameForMap<std::shared_ptr<Entity>>(name, _entities);
	// set the entity's name to the valid name
	entity->name = name;
	// add to map of entities
	_entities.insert(std::pair<std::string, std::shared_ptr<Entity>>(name, entity));
}

void Scene::RemoveEntity(std::string name)
{
	// if the entity exists the remove it
	if (ItemExistsInMap<std::shared_ptr<Entity>>(name, _entities))
		_entities.erase(name);
}

std::shared_ptr<Entity> Scene::GetEntity(std::string name)
{
	// if entity exists
	if (ItemExistsInMap<std::shared_ptr<Entity>>(name, _entities))
		// return it
		return _entities.at(name);
	else
		// else return null
		return nullptr;
}


void Scene::Update()
{
	// get current time in seconds since start of app
	double curTime = glfwGetTime();
	// set delta time to be current time - last frame time 
	deltaTime = curTime - lastFrameTime;
	// set last frame time to current time to setup next frame
	lastFrameTime = curTime;

	// -- frame begin --
	FireListener(EventType::Frame_Start);

	// set background colour
	glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f);
	// Make sure background is applied and reset z buffer to make depth testing work properly
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	
	// check for keyboard inputs
	processKeyboardInputs();
	// process any pending events that have been received and then returns immediately
	glfwPollEvents();



	// loop through each entity
	for (std::pair<std::string, std::shared_ptr<Entity>> entityIterator : _entities)
	{
		std::shared_ptr<Entity> iteratedEntity = entityIterator.second;

		// if the actual entity is enabled
		if(iteratedEntity->isActive)
			// loop through each component under entity
			for (std::pair<Entity::ComponentType, std::shared_ptr<Component>> componentIterator : iteratedEntity->GetComponents()) 
			{
				// update the iterated component
				UpdateComponent(componentIterator.first, componentIterator.second);
			}
	}

	
	// swap the front buffer with the back buffer to draw any changes
	glfwSwapBuffers(_window);

	// frame has ended
	FireListener(EventType::Frame_End);
}

void Scene::UpdateComponent(Entity::ComponentType type, std::shared_ptr<Component> component)
{
	// switch case thru different component types and apply according update
	switch (type)
	{
	case Entity::SpriteRenderer:
	{
		// render the sprite each frame 

		// cast component to renderer
		std::shared_ptr<SpriteRenderer> renderer = std::static_pointer_cast<SpriteRenderer>(component);
		// render to screen
		renderer->Draw(mainCamera);
		break; 
	}
	case Entity::RectangleRenderer:
	{
		// cast component to renderer
		std::shared_ptr<RectangleRenderer> renderer = std::static_pointer_cast<RectangleRenderer>(component);
		// render to screen
		renderer->Draw(mainCamera);
		break;
	}
	case Entity::EllipseRenderer:
	{
		// cast component to renderer
		std::shared_ptr<EllipseRenderer> renderer = std::static_pointer_cast<EllipseRenderer>(component);
		// render to screen
		renderer->Draw(mainCamera);
		break;
	}
	default: // do nothing
		break;
	}
}

void Scene::Initialise()
{
	// intialise last frame time to creation of scene
	lastFrameTime = glfwGetTime();
	// fill up _eventListeners with an empty vector of EventListeners for each event type
	IntialiseListenerMap();
}

unsigned int Scene::AddListener(EventType type, EventListener listenerToAdd)
{
	// set to current amount
	int listenerId = amntOfEventListenersCreated;
	// increment to setup next listener
	amntOfEventListenersCreated++;

	// set the name of the listener to add
	listenerToAdd.id = listenerId;
	// add the listener to vector of event listeners
	_eventListeners[type].push_back(listenerToAdd);

	return listenerId;
}

void Scene::RemoveListener(EventType type, unsigned int id)
{
	// try and find the listener under the _eventListeners map
	int listenerIndex = IndexOfListener(type, id);

	// if listener was found
	if (listenerIndex != -1)
		// remove it, idk why you use .begin() but you do. Some weird stuff where the parameter requires an iterator and begin is an empty has one essentially
		_eventListeners[type].erase(_eventListeners[type].begin()+ listenerIndex);

	
}

void Scene::FireListener(EventType type, EventInfo info)
{
	// reference to the listeners vector assiocated with event listener type
	std::vector<EventListener>& listenersVector = _eventListeners[type];
	
	// iterate through each event listener and fire them all
	for (EventListener listener : listenersVector)
	{
		// fire the listener's callback
		listener.callback(info);
	}
}

void Scene::UpdateViewport(float width, float height)
{
	// update projection matrix of camera whenever viewport changes
	mainCamera->UpdateProjection(width, height);
}


// handles any keyboard inputs
void Scene::processKeyboardInputs() {
	// on ESC pressed
	if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		// close window
		glfwSetWindowShouldClose(_window, true);
}

void Scene::IntialiseListenerMap()
{
	// loop through each enum. EvenType::length is the last value in the enum which is a placeholder
	for (int enumIndex = 0; enumIndex < EventType::Length; enumIndex++)
	{
		// cast index to type
		EventType iteratedType = (EventType) enumIndex;
		// create an empty vector under map of listeners for eahc type of event 
		_eventListeners[iteratedType] = std::vector<EventListener>();

	}
}

int Scene::IndexOfListener(EventType type, unsigned int id)
{
	// reference to the listeners vector assiocated with event listener type
	std::vector<EventListener>& listenersVector = _eventListeners[type];

	// Perform a linear search that checks the id with each event listeners id
	for (int index = 0; index < listenersVector.size(); index++)
	{
		EventListener listener = listenersVector[index];
		// if there is a match, return the index of the listener under the vector
		if (listener.id == id)
			return index;
	}

	// else, was not found, return -1
	return -1;
}

template<typename T>
bool Scene::ItemExistsInMap(std::string name, std::map<std::string, T>& inputMap)
{
	// return whether or not the item exists in map
	return (inputMap.find(name) != inputMap.end());
}

template<typename T>
std::string Scene::GetValidNameForMap(std::string inputName, std::map<std::string, T>& inputMap)
{
	// start with the input name and then keep adding "1" until you find a unique name
	std::string outputName = inputName;

	// while there is an element with the same name as desired one in map
	while (ItemExistsInMap<T>(outputName, inputMap))
	{
		outputName += "1"; // add 1 to the end of the name
	}

	return outputName;
}