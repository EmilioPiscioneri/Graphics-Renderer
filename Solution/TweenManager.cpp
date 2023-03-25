#include "TweenManager.h"
#include "Scene.h"

TweenManager::TweenManager(Scene* parentScene)
{
	_parentScene = parentScene;
}

std::uint64_t TweenManager::AddTween(std::shared_ptr<Tween> tweenToAdd, bool startOnAdd)
{
	// get the current nextID value and increment for next tween that is added
	std::int64_t tweenID = nextID++;
	
	// set ID of tween
	tweenToAdd->ID = tweenID;

	// add the tween
	_tweens.insert(std::pair<std::int64_t, std::shared_ptr<Tween>>(tweenID, tweenToAdd));
	
	// self explanatory
	if (startOnAdd)
		tweenToAdd->Start();

	return tweenID;
}

void TweenManager::RemoveTween(std::uint64_t ID)
{
	// if exists
	if (TweenExists(ID))
		// remove
		_tweens.erase(ID);
}

void TweenManager::UpdateAll()
{
	// time since last frame began being rendered
	double deltaTime = _parentScene->deltaTime;
	// loop through each stored tween
	for (std::pair<std::int64_t, std::shared_ptr<Tween>> iterator : _tweens)
	{
		// update the tween
		iterator.second->Update(deltaTime);
	}
}

bool TweenManager::TweenExists(std::uint64_t ID)
{
	// return whether or not the tween was found
	return (_tweens.find(ID) != _tweens.end());
}
