#pragma once
#include "EventInfo.h"
#include <functional>
// this is an object which stores a reference to a function pointer and an id to itself under a scene.
class EventListener
{
public:
	// uint id of event. No one is having more than 4,294,967,295 event listeners just saying.
	// Defaulted to 4,294,967,295 (2^32-1) when it is not initialised
	unsigned int id = 4294967295;

	// an event listener callback is a function/lambda/callable object pointer that has no return and takes an event info object as its parameter
	typedef std::function<void(EventInfo)> EventListenerCallback;

	// function pointer that is stored under the object
	EventListenerCallback callback;

	// Create a new event listener object that stores a pointer to function callback inside it
	EventListener(EventListenerCallback callback);
};

