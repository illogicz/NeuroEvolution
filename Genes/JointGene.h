#pragma once

#include "BaseGene.h"


class JointGene : public BaseGene
{

public:

	JointGene copy(JointGene &g)
	{
		BaseGene::copy(BaseGene(g));
		g.m_maxForce = copyValue(m_maxForce, 0,100);
		g.m_maxTorque = copyValue(m_maxTorque, 0,100);
		g.m_lowerAngle = copyValue(m_maxForce, 0,3.14);
		g.m_upperAngle = copyValue(m_maxTorque, 0,-3.14);

	}

private:

	float m_lowerAngle;
	float m_upperAngle;
	float m_maxForce;
	float m_maxTorque;

};