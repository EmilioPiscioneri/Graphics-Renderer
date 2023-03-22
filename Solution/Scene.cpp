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
	
	// set its attached scene to this cene
	entity->parentScene = this;

	// Make sure the desired name hasn't been taken, if so keep adding "1" until a valid name is found
	name = GetValidName(name);
	// set the entity's name to the valid name
	entity->name = name;

	// check if the added entity has a zIndex higher than the current highest
	if (entity->transform.GetZIndex() > highestZIndex)
		// set the highest to the new found highest
		highestZIndex = entity->transform.GetZIndex();

	// if it has transparecny and add to appropriate entities map
	if(entity->GetHasTransparency())
		_transparentEntities.insert(std::pair<std::string, std::shared_ptr<Entity>>(name, entity));
	else // else is opaque
		_opaqueEntities.insert(std::pair<std::string, std::shared_ptr<Entity>>(name, entity));
}

void Scene::RemoveEntity(std::string name)
{

	// if the entity exists in opaue entites then remove it
	if (ItemExistsInMap<std::shared_ptr<Entity>>(name, _opaqueEntities)) {
		// get the zIndex of the removed entity
		unsigned int removedEntityZIndex = _opaqueEntities.at(name)->transform.GetZIndex();

		_opaqueEntities.erase(name);

		// check if the found entity has a zIndex that is being removed which is the current highest
		if (removedEntityZIndex == highestZIndex)
			// update the highest to see if there is a lower value now
			UpdateHighestZIndex();
	}

	// else if it is in transparent entites remove it
	else if (ItemExistsInMap<std::shared_ptr<Entity>>(name, _transparentEntities))
	{
		// get the zIndex of the removed entity
		unsigned int removedEntityZIndex = _transparentEntities.at(name)->transform.GetZIndex();

		_transparentEntities.erase(name);

		// check if the found entity has a zIndex that is being removed which is the current highest
		if (removedEntityZIndex == highestZIndex)
			// update the highest to see if there is a lower value now
			UpdateHighestZIndex();
	}
}

std::shared_ptr<Entity> Scene::GetEntity(std::string name)
{
	// if entity exists in opaque map
	if (ItemExistsInMap<std::shared_ptr<Entity>>(name, _opaqueEntities))
		// return it
		return _opaqueEntities.at(name);
	// or it is in transparent map
	else if(ItemExistsInMap<std::shared_ptr<Entity>>(name, _transparentEntities))
		return _transparentEntities.at(name);
	else
		// else return null
		return nullptr;
}

void Scene::UpdateEntityTransparency(std::shared_ptr<Entity> entity)
{
	// error checks
	if (entity == nullptr)
		throw std::exception("Why did you just try to update the transparency of nullptr");

	if(entity->parentScene != this)
		throw std::exception("Why did you just try to update the transparency of an entity that doesn't belong to this scene?");

	// remove the entity from wherever it is
	RemoveEntity(entity->name);
	// add it back, the add function handles transparency stuff for you/me/us/idk what perspective I'm supposed to write from 
	AddEntity(entity->name, entity);
	
}

void Scene::UpdateHighestZIndex()
{
	// first loop through each opaque entity
	for (std::pair<std::string, std::shared_ptr<Entity>> entityIterator : _opaqueEntities)
	{
		std::shared_ptr<Entity> iteratedEntity = entityIterator.second;

		// check if the iterated entity has an index higher than the current highest
		if (iteratedEntity->transform.GetZIndex() > highestZIndex)
			// set to new highest
			highestZIndex = iteratedEntity->transform.GetZIndex();
	}

	// Now you loop through all entities with transparency 
	for (std::pair<std::string, std::shared_ptr<Entity>> entityIterator : _transparentEntities)
	{
		std::shared_ptr<Entity> iteratedEntity = entityIterator.second;

		// check if the iterated entity has an index higher than the current highest
		if (iteratedEntity->transform.GetZIndex() > highestZIndex)
			// set to new highest
			highestZIndex = iteratedEntity->transform.GetZIndex();
	}

}

// last element is taken as pivot
int Partition(std::vector<std::shared_ptr<Entity>>& v, int start, int end) {

	int pivot = end;
	int j = start;
	for (int i = start; i < end; ++i) {
		if (v[i]->transform.GetZIndex() < v[pivot]->transform.GetZIndex()) {
			swap(v[i], v[j]);
			++j;
		}
	}
	swap(v[j], v[pivot]);
	return j;

}

void Quicksort(std::vector<std::shared_ptr<Entity>>& v, int start, int end) {

	if (start < end) {
		int pivot = Partition(v, start, end);
		Quicksort(v, start, pivot - 1);
		Quicksort(v, pivot + 1, end);
	}

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



	// first loop through each opaque entity
	for (std::pair<std::string, std::shared_ptr<Entity>> entityIterator : _opaqueEntities)
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
	
	// Look, I'm not gonna lie I got kinda lazy with the below implementation of drawing objects fromt the back of the scene first then moving towards the front.
	// I'm tired, school has been annoying, homework and tests just pile up and this is my pitty story as to why I didn't just keep an orderer map on entities
	// sorted by index whether it be descending or ascending. I plan to fix it up in V 0.3 of the renderer. It's in the trello ok. I'm complaining to the air rn.

	
	// Just do a quick sort and order the indexes of each transparent entity. I did not copy-paste the code from https://slaystudy.com/c-vector-quicksort/
	// I'm not gonna lie I'm like barely even focusing and so tired this is a rare time that I'll do this nasty code. It's simply a placeholder ok.


	std::vector<std::shared_ptr<Entity>> sortedTransparentEntities;
	
	// fill it in
	for (std::pair<std::string, std::shared_ptr<Entity>> entityIterator : _transparentEntities)
	{
		std::shared_ptr<Entity> iteratedEntity = entityIterator.second;
		sortedTransparentEntities.push_back(iteratedEntity); // add to array
	}

	// quicksort
	Quicksort(sortedTransparentEntities, 0, sortedTransparentEntities.size() - 1);

	// Now you loop through all entities with transparency 
	for ( std::shared_ptr<Entity> entity : sortedTransparentEntities)
	{
		std::shared_ptr<Entity> iteratedEntity = entity;

		// if the actual entity is enabled
		if (iteratedEntity->isActive)
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
		// remove it, idk why you use .begin() but you do. Some weird shit where the parameter requires an iterator and begin is an empty has one essentially
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

std::string Scene::GetValidName(std::string inputName)
{
	// start with the input name and then keep adding "1" until you find a unique name
	std::string outputName = inputName;

	// While the name is in either opaque or transparent entity maps. I do this because I'm pretty sure maps are faster.
	while (ItemExistsInMap <std::shared_ptr<Entity>> (outputName, _opaqueEntities) || ItemExistsInMap<std::shared_ptr<Entity>>(outputName,_transparentEntities))
	{
		outputName += "1"; // add 1 to the end of the name
	}

	return outputName;
}