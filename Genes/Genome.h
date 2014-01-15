#pragma once

#include <vector>
#include "Genes/BaseGene.h"


class Genome
{

public:

	Genome mate(Genome &s);

private:

	std::vector<BaseGene> m_genes;



};