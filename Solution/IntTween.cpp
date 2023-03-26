#include "IntTween.h"
#include <exception>

IntTween::IntTween(int* valueToTween, int startValue, int targetValue, double tweenDuration, double tweenDelay, bool deleteOnEnd, Method method)
{
	useSetter = false;
	_targetValue = targetValue;
	_duration = tweenDuration;
	_delay = tweenDelay;
	_startValue = startValue;
	_method = method;
	_valueToTween = valueToTween;
	this->deleteOnEnd = deleteOnEnd;
}

IntTween::IntTween(SetterCallback setterCallback, int startValue, int targetValue, double tweenDuration, double tweenDelay, bool deleteOnEnd, Method method)
{
	useSetter = true;
	_valueToTween = nullptr;
	_setterCallback = setterCallback;
	_duration = tweenDuration;
	_delay = tweenDelay;
	_targetValue = targetValue;
	_startValue = startValue;
	_method = method;
	this->deleteOnEnd = deleteOnEnd;
}

void IntTween::Update(double deltaTime)
{
	// don't do anything if tween is inactive
	if (_state == State::InActive)
		return;

	// error check
	if (!useSetter && _valueToTween == nullptr)
	{
		Stop();
		throw std::exception("Tried to tween a value that has been set to nullptr");
	}

	// this is the time difference in seonds since last update. Default to delta time. If the timer has been superceeded then this value is capped
	double differenceSinceLastUpdate = deltaTime;

	// if we've reached the end
	if (_progressedSeconds + deltaTime >= _duration + _delay)
	{
		// stop tween from continuing in future
		Stop();
		// get the difference from progressed so far to target to account for difference to reach end
		differenceSinceLastUpdate = (_duration + _delay) - _progressedSeconds;
	}

	// add progressed time
	_progressedSeconds += differenceSinceLastUpdate;

	// if the amount of time passed has surpassed the delay time
	if (_progressedSeconds > _delay) {
		// how much time has passed since the actual start of the tween
		double timeSinceStart = _progressedSeconds - _delay;

		// do the start value plus difference from the start to end * percentage of how much of the tween has been completed
		int newValue = _startValue + (int)((_targetValue - _startValue) * (timeSinceStart / _duration));

		// if not using setters
		if (!useSetter)
			*_valueToTween = newValue;
		else  // else, use setter
			_setterCallback(newValue);
	}


}

void IntTween::Start()
{
	// if not using setter
	if (!useSetter)
		*_valueToTween = _startValue;
	else
		_setterCallback(_startValue);

	_progressedSeconds = 0;
	_state = State::Active;
}