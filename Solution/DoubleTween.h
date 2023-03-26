#pragma once
#include "Tween.h"
#include <functional>

class DoubleTween :
    public Tween
{
public:
	typedef std::function<void(double)> SetterCallback;

	// creates a new double tween with a value to tween, target value to reach, how long to run for, delay, what method to use for tweening and whether to remove tween from tween manager on end 
	DoubleTween(double* valueToTween, double startValue, double targetValue, double tweenDuration = 1.0, double tweenDelay = 0.0, bool deleteOnEnd = true, Method method = Method::Linear);

	// creates a new double tween with setter callback for a value to tween, a target value to reach, how long to run for, delay, what method to use for tweening and whether to remove tween from tween manager on end 
	// setters are callbacks which return nothing but take a value as a parameter
	DoubleTween(SetterCallback setterCallback, double startValue, double targetValue, double tweenDuration = 1.0, double tweenDelay = 0.0, bool deleteOnEnd = true, Method method = Method::Linear);

	// actually updates the value stored by the tween object
	void Update(double deltaTime);
	void Start();
private:
	// The target value to reach at end of time
	double _targetValue;

	// the value that the tween started from
	double _startValue;

	// - if not using getters and setters -

	// the actual value to tween each update
	double* _valueToTween;

	// - if using setter -
	SetterCallback _setterCallback;
};

