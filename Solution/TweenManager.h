#pragma once
#include <map>
#include "Tween.h"

class Scene;

// A class that manages tweens, objects that interpolate values from one value to another over time
class TweenManager
{
public:
	TweenManager(Scene* parentScene);

	// adds a tween to tween manager and optional start tween on add parameter. Returns the id of the tween
	std::uint64_t AddTween(std::shared_ptr<Tween> tweenToAdd, bool startOnAdd = true);

	// removes a tween using ID
	void RemoveTween(std::uint64_t ID);

	// updates all tweens under the manager using parentScene's delta time
	void UpdateAll();

private:
	// parent scene of the tween manager
	Scene* _parentScene;

	// maps of pointers to all tweens managed by the curerent manager. Indexed by tween id
	std::map<std::uint64_t, std::shared_ptr<Tween>> _tweens;

	// The next available id for a tween managed by this tween manager to use
	std::uint64_t nextID = 0;

	// Returns whether a tween exists in tween manager
	bool TweenExists(std::uint64_t ID);
};

