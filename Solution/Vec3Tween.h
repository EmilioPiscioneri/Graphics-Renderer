#pragma once
#include "Tween.h"
#include <glm/glm.hpp> // OpenGL maths: Include all GLM core / GLSL features

class Vec3Tween :
    public Tween
{
public:
	// creates a new vec3 tween with a value to tween, target value to reach, how long to run for, what method to use for tweening and whether to remove tween from tween manager on end 
	Vec3Tween(glm::vec3* valueToTween, glm::vec3 startValue, glm::vec3 targetValue, double tweenDuration = 1.0, bool deleteOnEnd = true, Method method = Method::Linear);

	// actually updates the value stored by the tween object
	void Update(double deltaTime);
	void Start();
private:
	// The target value to reach at end of time
	glm::vec3 _targetValue;

	// the value that the tween started from
	glm::vec3 _startValue;

	// the actual value to tween each update
	glm::vec3* _valueToTween;
};


