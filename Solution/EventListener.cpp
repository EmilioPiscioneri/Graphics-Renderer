#include "EventListener.h"


EventListener::EventListener(EventListenerCallback callback)
{
	// fucking compiler wouldn't shut up that it's not intialised although IT IS
	id = 4294967295;
	// asign callback, very simple
	this->callback = callback;
}
