#include "MaxIndependentSet.h"
#include "IAlgorithmFactory.h"
#include <list>
#include <string.h>
#include <algorithm>
#include "Logger.h"

MaxIndependentSet::MaxIndependentSet () {

}

MaxIndependentSet::~MaxIndependentSet () {
    
}

bool MaxIndependentSet::Calculate() {
    
    // this->m_pGraph is not saved in ProcessConsoleParams stack -> no need for delete

    // make a copy of the graph's complement graph
    GraphPtr pGraph = GraphPtr(m_pGraph->MakeBaseCopy(GTC_COMPLEMENT));
    if (!pGraph)
    {
        return false;
    }

    LOG_INFO(pGraph->PrintGraph());

    // run MaxClique algorithm on the complement graph
    AlgorithmPtr maxCliqueAlgo = AlgorithmPtr(m_pAlgorithmFactory->CreateAlgorithm("mc", pGraph.get()));
    
    /**
     * By default, MaxClique algorithm will have the following values:
     * m_num_threads = 1
     * m_param_lower_bound = 0
     * m_param_upper_bound = UINT_MAX
     * m_param_algorithm_type = Exact (Algorithm { Heuristic, Hybrid, Exact })
     */

    maxCliqueAlgo->Calculate();
    maxCliqueAlgo->UnitTest();

    IntWeightType resultCount = maxCliqueAlgo->GetResultCount();

    for (int idx = 1; idx < resultCount; idx++) {
        this->m_maxIndependentSet.emplace_back(maxCliqueAlgo->GetResult(idx).nodeId);
    }

    return true;
}


/**
 * Used to loop over the result using GetResult
 */
IndexType MaxIndependentSet::GetResultCount() const {
    return this->m_maxIndependentSet.size() + 1;
}

// Get result of index. Algorithms can have complex result.
/**
 * Index 0 will return number of nodes in the MIS found by the algorithm
 * Index 1 - <number of nodes> will get the corresponding node id.
 */
AlgorithmResult MaxIndependentSet::GetResult(IndexType index) const {
    AlgorithmResult res;
    if (index == 0) {
        res.type = ART_INT;
        res.nValue = m_maxIndependentSet.size();
    } else if (index < m_maxIndependentSet.size() + 1 && index > 0) {
        res.type = ART_NODE_ID;
        res.nodeId = m_maxIndependentSet[index - 1];
    }
    return res;
}
