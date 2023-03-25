#pragma once
#include "Tween.h"
class DoubleTween :
    public Tween
{
public:
	// creates a new double tween with a value to tween, target value to reach, how long to run for, what method to use for tweening and whether to remove tween from tween manager on end 
	DoubleTween(double* valueToTween, double startValue, double targetValue, double tweenDuration = 1.0, bool deleteOnEnd = true, Method method = Method::Linear);

	// actually updates the value stored by the tween object
	void Update(double deltaTime);
	void Start();
private:
	// The target value to reach at end of time
	double _targetValue;

	// the value that the tween started from
	double _startValue;

	// the actual value to tween each update
	double* _valueToTween;
};

