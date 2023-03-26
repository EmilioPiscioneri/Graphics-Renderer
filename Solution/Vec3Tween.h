#pragma once
#include "Tween.h"
#include <glm/glm.hpp> // OpenGL maths: Include all GLM core / GLSL features
#include <functional>

class Vec3Tween :
    public Tween
{
public:
	typedef std::function<void(glm::vec3)> SetterCallback;

	// creates a new vec2 tween with a value to tween, target value to reach, how long to run for, delay, what method to use for tweening and whether to remove tween from tween manager on end 
	Vec3Tween(glm::vec3* valueToTween, glm::vec3 startValue, glm::vec3 targetValue, double tweenDuration = 1.0, double tweenDelay = 0.0, bool deleteOnEnd = true, Method method = Method::Linear);

	// creates a new vec2 tween with setter callback for a value to tween, a target value to reach, how long to run for, delay, what method to use for tweening and whether to remove tween from tween manager on end 
	// setters are callbacks which return nothing but take a value as a parameter
	Vec3Tween(SetterCallback setterCallback, glm::vec3 startValue, glm::vec3 targetValue, double tweenDuration = 1.0, double tweenDelay = 0.0, bool deleteOnEnd = true, Method method = Method::Linear);

	// actually updates the value stored by the tween object
	void Update(double deltaTime);
	void Start();
private:
	// The target value to reach at end of time
	glm::vec3 _targetValue;

	// the value that the tween started from
	glm::vec3 _startValue;

	// - if not using getters and setters -

	// the actual value to tween each update
	glm::vec3* _valueToTween;

	// - if using setter -
	SetterCallback _setterCallback;
};


