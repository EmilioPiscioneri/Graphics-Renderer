#pragma once
#include <glad/glad.h>
#include <glfw3.h>
#include <iostream>
#include <map>
#include "Entity.h"
#include "OrthoCamera.h"
#include "EventListener.h"
#include <vector>

// Create a new scene to render entities.
// Note that you must call the UpdateViewport function of this scene whenever the viewport is updated
class Scene
{
public:
	float width, height;

	// colour of background of scene
	glm::vec3 backgroundColor;

	// Create a new scene with initial width and height and camera. If camera is nullptr will create a default camera
	Scene(GLFWwindow* window, float width, float height, glm::vec3 backgroundColor = glm::vec3(0.0204f, 0.265f, 0.340f), std::shared_ptr<OrthoCamera> camera = nullptr);
	~Scene();

	// amount of time passed since the beginning of the last frame to the beginning of current one
	double deltaTime = 0;

	// Add an entity to scene, giving it a name and providing smart pointer to the entity. 
	// "1" is added to name if it already exists
	void AddEntity(std::string name, std::shared_ptr<Entity> entity);

	// Remove an entity from the scene using name
	void RemoveEntity(std::string name);

	// Retrieve an entity frome scene based on name. Returns nullptr if not found
	std::shared_ptr<Entity> GetEntity(std::string name);

	// If an entity's HasTransparency boolean changes after it has been added to scene this function MUST be called. Pass in the new transparency in the last newTransparency parameter
	void UpdateEntityTransparency(std::shared_ptr<Entity> entity);

	// main camera in scene
	std::shared_ptr<OrthoCamera> mainCamera;

	// update the scene
	void Update();

	// run this whenever the viewport of the window changes
	void UpdateViewport(float width, float height);

	// setup scene 
	void Initialise();

	// Type of an event
	// I am using pascal case with underscores because I'm not sure on enum naming conventions and it looks alright to me.
	enum EventType {
		Frame_Start, // fired when frame begins, before any calculations are made (apart from delta time)
		Frame_End, // fired when everything for updating/rendering frame has been completed
		Length, // NOT AN EVENT - Cast this value to an integer to get how many event types there are
	};

	// Add an event listener (object with function callback) based on event type to scene.
	// Returns: id of EventListener under scene
	unsigned int AddListener(EventType type, EventListener listenerToAdd);

	// Removes an event listener based on type it is attached to and id of the listener
	void RemoveListener(EventType type, unsigned int id);
	
	// Fires all event listeners of a specific type and supply info along with it
	void FireListener(EventType type, EventInfo info = EventInfo());

	
private:
	// private variables come after public because they need to access some public values
	
	// dictionary(map) of all opaque entities in a scene, indexed by name
	std::map<std::string, std::shared_ptr<Entity>> _opaqueEntities;

	// dictionary(map) of all entities (with any kind of transparency) in a scene, indexed by name
	std::map<std::string, std::shared_ptr<Entity>> _transparentEntities;

	// dictionary (map) of dynamic array (vectors) of event listeners (objects with function callbacks) which are indexed by event type.
	// There shouldn't be too much overhead with the vectors for each event type but like what do I know I'm 16 yknow
	std::map < EventType, std::vector<EventListener>> _eventListeners;

	// this is incremented each time an event listener is added. It is used to set the id of each added event listener
	// No one is using more than 2^32 - 1 (4,294,967,295) fuckin event listeners
	unsigned int amntOfEventListenersCreated = 0;


	GLFWwindow* _window;
	// callback for when window is resized
	
	// Deals with any keyboard inputs that are pending being dealt with
	void processKeyboardInputs();

	// intialises _eventListeners with an empty vector for each different event type
	void IntialiseListenerMap();

	//unsigned int _maxUInt = 4294967295; // 2^32 - 1

	// Checks if a listener of type id exists under that event type and returns the index under associated vector if so. 
	// If not found, returns -1
	int IndexOfListener(EventType type, unsigned int id);

	// whether an item of specific name exists in a map (passed as pointer)
	template <typename T>
	static bool ItemExistsInMap(std::string name, std::map<std::string, T>& inputMap);



	// Checks if desired name doesn't exist and if not, keeps adding "1" to name until there is an available unused name. 
	std::string GetValidName(std::string inputName);
	// Run update function on a component based on type
	void UpdateComponent(Entity::ComponentType type, std::shared_ptr<Component> component);
	//when the last frame occurred in seconds (relative to how long program has been running for)
	double lastFrameTime;
};

