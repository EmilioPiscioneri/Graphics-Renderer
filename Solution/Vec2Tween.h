#pragma once
#include "Tween.h"
#include <glm/glm.hpp> // OpenGL maths: Include all GLM core / GLSL features
#include <functional>

class Vec2Tween :
    public Tween
{
public:
	typedef std::function<float()> GetterCallback;
	typedef std::function<void(float)> SetterCallback;


	// creates a new vec2 tween with a value to tween, target value to reach, how long to run for, what method to use for tweening and whether to remove tween from tween manager on end 
	Vec2Tween(glm::vec2* valueToTween, glm::vec2 startValue, glm::vec2 targetValue, double tweenDuration = 1.0, bool deleteOnEnd = true, Method method = Method::Linear);

	// creates a new vec2 tween with a getters and setters for a value to tween, a target value to reach, how long to run for, what method to use for tweening and whether to remove tween from tween manager on end 
	// getters are callbacks which return only a value
	// setters are callbacks which return nothing but take a value as a parameter
	Vec2Tween(GetterCallback getterCallback, SetterCallback setterCallback, glm::vec2 startValue, glm::vec2 targetValue, double tweenDuration = 1.0, bool deleteOnEnd = true, Method method = Method::Linear);


	// actually updates the value stored by the tween object
	void Update(double deltaTime);
	void Start();
private:
	

	// The target value to reach at end of time
	glm::vec2 _targetValue;

	// the value that the tween started from
	glm::vec2 _startValue;

	// - if not using getters and setters -

	// the actual value to tween each update
	glm::vec2* _valueToTween;
	
	// - if using getters and setters -
};

