#pragma once
#include "Tween.h"
class UIntTween :
    public Tween
{
public:
	// creates a new uint tween with a value to tween, target value to reach, how long to run for, what method to use for tweening and whether to remove tween from tween manager on end 
	UIntTween(unsigned int* valueToTween, unsigned int startValue, unsigned int targetValue, double tweenDuration = 1.0, bool deleteOnEnd = true, Method method = Method::Linear);

	// actually updates the value stored by the tween object
	void Update(double deltaTime);
	void Start();
private:
	// The target value to reach at end of time
	unsigned int _targetValue;

	// the value that the tween started from
	unsigned int _startValue;

	// the actual value to tween each update
	unsigned int* _valueToTween;
};

