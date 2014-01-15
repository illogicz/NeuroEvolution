#include "sWorld.h"

void sWorld::SayGoodbye(b2Joint * joint)
{
	sJoint * j = (sJoint*)joint->GetUserData();
	
	if(j->isInWorld()){
		j->jointDestroyed();
		remove((sObject*)j);
	}
}