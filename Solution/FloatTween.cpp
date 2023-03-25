#include "FloatTween.h"
#include <exception>


FloatTween::FloatTween(float* valueToTween, float startValue, float targetValue, double tweenDuration, bool deleteOnEnd, Method method)
{
	_targetValue = targetValue;
	_tweenDuration = tweenDuration;
	_startValue = startValue;
	_method = method;
	_valueToTween = valueToTween;
	this->deleteOnEnd = deleteOnEnd;
}

void FloatTween::Update(double deltaTime)
{
	// don't do anything if tween is inactive
	if (_state == State::InActive)
		return;

	// error check
	if (_valueToTween == nullptr)
	{
		Stop();
		throw std::exception("Tried to tween a value that has been set to nullptr");
	}

	// this is the time difference in seonds since last update. Default to delta time. If the timer has been superceeded then this value is capped
	double differenceSinceLastUpdate = deltaTime;

	// if we've reached the end
	if (_progressedSeconds + deltaTime >= _tweenDuration)
	{
		// stop tween from continuing in future
		Stop();
		// get the difference from progressed so far to target to account for difference to reach end
		differenceSinceLastUpdate = _tweenDuration - _progressedSeconds;
	}

	// add progressed time
	_progressedSeconds += differenceSinceLastUpdate;

	// do the start value plus difference from the start to end * percentage of how much of the tween has been completed
	*_valueToTween = _startValue + (float)((_targetValue - _startValue) * (_progressedSeconds / _tweenDuration));
	
}

void FloatTween::Start()
{
	*_valueToTween = _startValue;
	_progressedSeconds = 0;
	_state = State::Active;
}
