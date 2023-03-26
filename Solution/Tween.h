#pragma once
#include <iostream>


// This is a base class of objects that interpolate from one value to another.
class Tween
{
public:
	// The interpolation method of the current tween
	enum Method {
		Linear
	};

	// The state of a tween
	enum State {
		// The tween is updating every frame
		Active,
		// The tween isn't updating every frame
		InActive
	};

	// Returns the state of the current tween
	State GetState();

	// whether or not to remove the tween from tween manager when it is finished running. Default is true
	bool deleteOnEnd = true;

	// ID of the current tween under tween manager. ONLY TWEEN MANAGER SHOULD SET THIS VALUE
	std::uint64_t ID = 0;

	// starts the current tween from starting value
	virtual void Start() = 0;

	void Resume();

	// stops/pauses the current tween
	void Stop();

	// actually updates the value stored by the tween object
	virtual void Update(double deltaTime) = 0;
protected:
	
	// whether or not to use setter callback for setting values 
	bool useSetter = false;

	// the current state of this tween
	State _state = InActive;

	// The interpolation method of the current tween
	Method _method = Linear;

	// How long the tween should run for in seconds, after the delay
	double _duration = 0;

	// How long to wait in seconds before a tween should run
	double _delay = 0;

	// How many seconds have passed since tween was started (including delay)
	double _progressedSeconds = 0;
};
