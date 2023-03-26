#pragma once
#include "Tween.h"
#include <functional>

class UIntTween :
    public Tween
{
public:
	typedef std::function<void(int)> SetterCallback;

	// creates a new uint tween with a value to tween, target value to reach, how long to run for, delay, what method to use for tweening and whether to remove tween from tween manager on end 
	UIntTween(unsigned int* valueToTween, unsigned int startValue, unsigned int targetValue, double tweenDuration = 1.0, double tweenDelay = 0.0, bool deleteOnEnd = true, Method method = Method::Linear);

	// creates a new uint tween with setter callback for a value to tween, a target value to reach, how long to run for, delay, what method to use for tweening and whether to remove tween from tween manager on end 
	// setters are callbacks which return nothing but take a value as a parameter
	UIntTween(SetterCallback setterCallback, unsigned int startValue, unsigned int targetValue, double tweenDuration = 1.0, double tweenDelay = 0.0, bool deleteOnEnd = true, Method method = Method::Linear);

	// actually updates the value stored by the tween object
	void Update(double deltaTime);
	void Start();
private:
	// The target value to reach at end of time
	unsigned int _targetValue;

	// the value that the tween started from
	unsigned int _startValue;

	// - if not using getters and setters -

	// the actual value to tween each update
	unsigned int* _valueToTween;

	// - if using setter -
	SetterCallback _setterCallback;
};

