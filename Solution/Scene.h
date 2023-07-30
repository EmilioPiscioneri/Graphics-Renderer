#pragma once
#include <glad/glad.h>
#include <glfw3.h>
#include <iostream>
#include <map>
#include <vector>

#include "Entity.h"
#include "OrthoCamera.h"
#include "EventListener.h"
#include "TweenManager.h"
#include "Collider.h"

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

	// When you want to change an entity's name, call this function first and then change the private _name value of entity class. 
	// This updates the stored maps of entities with the new name
	void UpdateEntityName(std::shared_ptr<Entity> entity, std::string newName);

	// This updates the stored map of entity colliders with the new collider using entity's name
	void UpdateEntityCollider(std::string entityName, std::shared_ptr<Collider> newCollider);

	// linearly searches through all entities in scene to find the highest zIndex. 
	// Only call this when the entity with the highest index has been removed or changed
	void UpdateHighestZIndex();

	// default is on. If this value is true then will update mainCamera's far plane to match the highest zIndex otherwise entities won't be in rendered space.
	// When off, scene won't do anything when highestZIndex is changed
	bool autoUpdateFarPlane = true;

	// Returns the highest zIndex that an entity has, in current scene.
	// This is NOT an expensive operation as highest zIndex is perpetually tracked
	unsigned int GetHighestZIndex();


	// sets the highest zIndex that an entity has, in current scene.
	void SetHighestZIndex(unsigned int newHighest);

	// main camera in scene. This should NEVER be nullptr
	std::shared_ptr<OrthoCamera> mainCamera;

	// This tween manager manages all tweens for the current scene
	TweenManager tweenManager = TweenManager(this);
	
	// how many pixels (global coords) there are per metre in scene. Used for physics calculations
	float pixelsPerMetre = 100.0f;

	// This is the gravitational acceleration (in metres per second per second -> m/s^2) of the scene.
	// The default is the approximate gravitational acceleration on earth's surface.
	float gravity = 9.807f;

	// update the scene
	void Update();

	// run this whenever the viewport of the window changes
	void UpdateViewport(float width, float height);

	// a delay between each frame you can set. In seconds
	float frameUpdateDelay = 0.0f;
	bool framePaused = false;
	

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
	
	// this is a map of all colliders which are attached to entities in the current scene
	std::map < std::string, std::shared_ptr<Collider>> _entityColliders;

	// highest zIndex that an entity has, in current scene
	unsigned int _highestZIndex = 0;

	// -- I use these two vectors below in order to draw the scene back to front so blending can be done properly yknow --

	// vector of each z index of each transparent entity in scene sorted in ascending order. 
	// The indexes in this vector correlate to _sortedZIndexes
	std::vector< unsigned int> _sortedZIndexes;

	// vector of each transparent entity in scene sorted by z index in ascending order. 
	// The indexes in this vector correlate to _sortedTransparentEntities
	std::vector<std::shared_ptr<Entity>> _sortedTransparentEntities;


	// dictionary(map) of all opaque entities in a scene, indexed by name
	std::map<std::string, std::shared_ptr<Entity>> _opaqueEntities;

	// dictionary(map) of all entities (with any kind of transparency) in a scene, indexed by name
	std::map<std::string, std::shared_ptr<Entity>> _transparentEntities;

	// dictionary (map) of dynamic array (vectors) of event listeners (objects with function callbacks) which are indexed by event type.
	// There shouldn't be too much overhead with the vectors for each event type but like what do I know I'm 16 yknow
	std::map < EventType, std::vector<EventListener>> _eventListeners;

	// this is incremented each time an event listener is added. It is used to set the id of each added event listener
	// No one is using more than 2^32 - 1 (4,294,967,295) event listeners
	unsigned int amntOfEventListenersCreated = 0;


	GLFWwindow* _window;
	// callback for when window is resized
	
	// Deals with any keyboard inputs that are pending being dealt with
	void processKeyboardInputs();

	// intialises _eventListeners with an empty vector for each different event type
	void IntialiseListenerMap();
	
	// setup scene 
	void Initialise();

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

	// Run update function on a collision component based on type
	void UpdateCollisionComponent(Entity::ComponentType type, std::shared_ptr<Component> component);

	//when the last frame occurred in seconds (relative to how long program has been running for)
	double lastFrameTime;
};

