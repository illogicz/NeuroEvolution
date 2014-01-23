#include "sContainer.h"
#include "sBody.h"

b2AABB sContainer::getAABB()
{
	b2AABB aabb;
	aabb.lowerBound.Set(100000,100000);
	aabb.upperBound.Set(-100000,-100000);


	for(set<sObject*>::iterator i = m_children.begin(); i != m_children.end(); ++i){
		if((*i)->m_type == BODY_OBJECT){
			sBody* body = (sBody*)(*i);
			aabb.Combine(aabb,body->getAABB());
		}
	}	
	return aabb;
}