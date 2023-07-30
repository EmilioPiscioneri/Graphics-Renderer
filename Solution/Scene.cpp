#include "Scene.h"
#include "SpriteRenderer.h"
#include "RectangleRenderer.h"
#include "EllipseRenderer.h"
#include "LineRenderer.h"
#include "RigidBody2D.h"
#include "BoxCollider.h"




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
	// set the entity's name to the valid name, set false to not do recursion loop
	entity->SetName(name, false);

	// the z index of the entity
	unsigned int entityZIndex = entity->transform.GetZIndex();

	// check if the added entity has a zIndex higher than the current highest
	if (entityZIndex > _highestZIndex)
		// set the highest to the new found highest
		SetHighestZIndex(entity->transform.GetZIndex());

	// ----- deal with colliders ----

	if (entity->ComponentExists(Entity::BoxCollider))
		// add collider
		_entityColliders.insert(std::pair <std::string, std::shared_ptr<Collider>>(name, entity->GetComponent<Collider>(Entity::BoxCollider)));

	


	// ----- deal with transparency ----

	// if it has transparecny and add to appropriate entities map
	if (entity->GetHasTransparency())
	{
		_transparentEntities.insert(std::pair<std::string, std::shared_ptr<Entity>>(name, entity));

		// --- add the entity to the list of sorted zIndexes if it is transparent ---

		// Get the lower bound iterator of the zIndex. This ends up being the index that the value is added to in vector
		std::vector<unsigned int>::iterator lowerBound = std::lower_bound(_sortedZIndexes.begin(), _sortedZIndexes.end(), entityZIndex);
		// doing lowerBound - vector.begin() gets the index that it would be adding to
		unsigned int indexToAddTo = (unsigned int) (lowerBound - _sortedZIndexes.begin());

		// Now that you have the lowerBound iterator you can use it to just add the zIndex and entity into our two vectors and it will be ordered
		_sortedZIndexes.insert(lowerBound, entityZIndex); // first add the z index
		// then add the entity. Currently there is a mismatch where the lowerBound iterator is for a vector of type unsigned int but to convert it to vector of
		// entity shared pointer, just do beginning of vector + desired index
		_sortedTransparentEntities.insert(_sortedTransparentEntities.begin() + indexToAddTo, entity);

	}
	else // else is opaque
		_opaqueEntities.insert(std::pair<std::string, std::shared_ptr<Entity>>(name, entity));
}

void Scene::RemoveEntity(std::string name)
{
	// if it has a collider stored under entity colliders in scene
	if (ItemExistsInMap<std::shared_ptr<Collider>>(name, _entityColliders))
		_entityColliders.erase(name); // remove it

	// if the entity exists in opaue entites then remove it
	if (ItemExistsInMap<std::shared_ptr<Entity>>(name, _opaqueEntities)) {
		// get the zIndex of the removed entity
		unsigned int removedEntityZIndex = _opaqueEntities.at(name)->transform.GetZIndex();

		_opaqueEntities.erase(name);

		// check if the found entity has a zIndex that is being removed which is the current highest
		if (removedEntityZIndex == _highestZIndex) 
			// update the highest to see if there is a lower value now
			UpdateHighestZIndex();
	}

	// else if it is in transparent entites remove it
	else if (ItemExistsInMap<std::shared_ptr<Entity>>(name, _transparentEntities))
	{
		std::shared_ptr<Entity> entityToRemove = _transparentEntities.at(name);

		// -- remove the entity from both the sortedZIndex and sortedTransparentEntities vectors --
		// find the entity's index
		std::vector<std::shared_ptr<Entity>>::iterator iteratorOfEntity = 
			std::find(_sortedTransparentEntities.begin(), _sortedTransparentEntities.end(), entityToRemove);
		// used to convert between the iterators of two different types
		unsigned int indexOfEntity = (unsigned int) (iteratorOfEntity - _sortedTransparentEntities.begin());

		// remove
		_sortedTransparentEntities.erase(iteratorOfEntity);
		_sortedZIndexes.erase(_sortedZIndexes.begin() + indexOfEntity);


		// get the zIndex of the removed entity
		unsigned int removedEntityZIndex = entityToRemove->transform.GetZIndex();

		_transparentEntities.erase(name);

		// check if the found entity has a zIndex that is being removed which is the current highest
		if (removedEntityZIndex == _highestZIndex)
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
	RemoveEntity(entity->GetName());
	// add it back, the add function handles transparency stuff for you/me/us/idk what perspective I'm supposed to write from 
	AddEntity(entity->GetName(), entity);
	
}

void Scene::UpdateEntityName(std::shared_ptr<Entity> entity, std::string newName)
{
	// error checks
	if (entity == nullptr)
		throw std::exception("Why did you just try to update the name of nullptr");

	if (entity->parentScene != this)
		throw std::exception("Why did you just try to update the name of an entity that doesn't belong to this scene?");

	// Remove the entity from the scene
	RemoveEntity(entity->GetName());

	// add the new name to the entity (don't need to do any validation because add entity function does for me/you/I'm conflicted on the pronoun to put here)
	AddEntity(newName, entity);
}

void Scene::UpdateEntityCollider(std::string entityName, std::shared_ptr<Collider> newCollider)
{
	// if not setting the collider to nullptr
	if (newCollider != nullptr)
		_entityColliders[entityName] = newCollider; // set collider
	// else if there is already a recorded entity and we're setting it to null
	else if (_entityColliders.find(entityName) != _entityColliders.end()) // redundant: && newCollider == nullptr 	
		_entityColliders.erase(entityName);
}

void Scene::UpdateHighestZIndex()
{
	// first loop through each opaque entity
	for (std::pair<std::string, std::shared_ptr<Entity>> entityIterator : _opaqueEntities)
	{
		std::shared_ptr<Entity> iteratedEntity = entityIterator.second;

		// check if the iterated entity has an index higher than the current highest
		if (iteratedEntity->transform.GetZIndex() > _highestZIndex)
			// set to new highest
			SetHighestZIndex(iteratedEntity->transform.GetZIndex());
	}

	// Now you check the last value in the vector of transparent entities sorted by zIndexes in ascending order. Also it needs to have at least 1 element
	if (_sortedZIndexes.size() != 0 && _sortedZIndexes[_sortedZIndexes.size() - 1] > _highestZIndex)
		SetHighestZIndex(_sortedZIndexes[_sortedZIndexes.size() - 1]);
	

}

unsigned int Scene::GetHighestZIndex()
{
	// simple
	return _highestZIndex;
}

void Scene::SetHighestZIndex(unsigned int newHighest)
{
	// set to new value
	_highestZIndex = newHighest;

	if (autoUpdateFarPlane)
		// then update the far plane to highest
		mainCamera->farPlane = (float) newHighest;

}


void Scene::Update()
{
	// get current time in seconds since start of app
	double curTime = glfwGetTime();
	
	
	if (!framePaused && frameUpdateDelay != 0) {
		// set delta time to be current time - last frame time 
		deltaTime = curTime - lastFrameTime;
		framePaused = true;
	}
	else if (frameUpdateDelay == 0)
	{
		// set delta time to be current time - last frame time 
		deltaTime = curTime - lastFrameTime;
	}

	if ((framePaused && curTime - lastFrameTime > frameUpdateDelay) || !framePaused) {
		if (frameUpdateDelay != 0) {
			std::cout << "Passed with deltaTime of " << deltaTime << std::endl;
		}
		//deltaTime -= frameUpdateDelay; // remove delay from deltatime
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
		// update tweens
		tweenManager.UpdateAll();

		// first loop through each opaque entity
		for (std::pair<std::string, std::shared_ptr<Entity>> entityIterator : _opaqueEntities)
		{
			std::shared_ptr<Entity> iteratedEntity = entityIterator.second;

			// if the actual entity is enabled
			if (iteratedEntity->isActive)
				// loop through each component under entity
				for (std::pair<Entity::ComponentType, std::shared_ptr<Component>> componentIterator : iteratedEntity->GetComponents())
				{
					// update the iterated component
					UpdateComponent(componentIterator.first, componentIterator.second);
				}
		}

		// Now you loop through all entities with transparency 
		for (std::shared_ptr<Entity> entity : _sortedTransparentEntities)
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
	case Entity::LineRenderer:
	{
		// cast component to renderer
		std::shared_ptr<LineRenderer> renderer = std::static_pointer_cast<LineRenderer>(component);
		// render to screen
		renderer->Draw(mainCamera);
		break;
	}
	case Entity::RigidBody2D: 
	{
		// cast component to rigid body
		std::shared_ptr<RigidBody2D> rigidBody = std::static_pointer_cast<RigidBody2D>(component);
		// calculate physics n that
		rigidBody->Update();
		break;
	}
	// --- colliders ---
	case Entity::BoxCollider:
	{
		// handle in a seperate function
		UpdateCollisionComponent(type, component);
		break;
	}
	// -- end of colliders ---
	default: // do nothing
		break;
	}
}

void Scene::UpdateCollisionComponent(Entity::ComponentType type, std::shared_ptr<Component> component)
{
	switch (type)
	{
	case Entity::BoxCollider: 
	{
		// cast component
		std::shared_ptr<BoxCollider> boxCollider = std::static_pointer_cast<BoxCollider>(component);

		// if the box collider is attached to a rigid body and that body doesn't have simulated off
		if(boxCollider->attachedRigidBody != nullptr && boxCollider->attachedRigidBody->isSimulated)
			// loop thru every other collider in scene
			for (std::pair<std::string, std::shared_ptr<Collider>> itertator : _entityColliders)
			{
				std::shared_ptr<Collider> otherCollider = itertator.second;
				// skip checking if the current collider is colliding with itself
				if (otherCollider != boxCollider && 
					// and check the iterated collider has a rigid body which is simulated (participates in physics calcs)
					otherCollider->attachedRigidBody != nullptr && otherCollider->attachedRigidBody->isSimulated
					// and since we don't wanna do static to static collision detection, check for that
					&& 
					// make sure both of the items aren't static as you will never do static to static collision detection
					(!(boxCollider->attachedRigidBody->isStatic && otherCollider->attachedRigidBody->isStatic)) 	)
				{
					// check for a collision and push out objects if so and also bounce those objects
					bool collision = boxCollider->CheckCollision(itertator.second, mainCamera, true, true);

					if (collision) {
						//frameUpdateDelay = 0.5;
					}

					// if there was a collision set the entity's colour to red, else white
					//if (collision) {
					//	boxCollider->GetParentEntity()->GetComponent<SpriteRenderer>(Entity::SpriteRenderer)->color = glm::vec3(1.0f, 0.0f, 0.0f);
					//	//otherCollider->GetParentEntity()->GetComponent<SpriteRenderer>(Entity::SpriteRenderer)->color = glm::vec3(1.0f, 0.0f, 0.0f);
					//}
					//else
					//{
					//	boxCollider->GetParentEntity()->GetComponent<SpriteRenderer>(Entity::SpriteRenderer)->color = glm::vec3(1.0f);
					//	//otherCollider->GetParentEntity()->GetComponent<SpriteRenderer>(Entity::SpriteRenderer)->color = glm::vec3(1.0f);
					//}

				}
			}
		break;
	}
	default:
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
	// if ESC pressed
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