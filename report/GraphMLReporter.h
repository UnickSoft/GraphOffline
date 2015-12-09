/**
 * Class return report in GraphML.
 *
 *  <?xml version="1.0" encoding="UTF-8"?>
 *   <graphml xmlns="http://graphml.graphdrawing.org/xmlns"  
 *       xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
 *       xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns
 *        http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd">
 *     <graph id="G" edgedefault="undirected" result="5">
 *       <node id="n0"/>
 *       <node id="n1"/>
 *       <node id="n2"/>
 *       <edge source="n0" target="n2"/>
 *       <edge source="n1" target="n2"/>
 *     </graph>
 *   </graphml>
 *
 * This XML describe heighlight graph. result="5" is result of algorithm.
 */

#pragma once

#include "IReporter.h"

class GraphMLReporter : public IReporter
{
public:
    
    /**
     * @return report in GraphML format.
     */
    virtual IndexType GetReport(const IAlgorithmResult* pAlgorithm, const IGraph* pGraph,
                                char* buffer, IndexType bufferSize);
    
    // release it.
    virtual void Release ()
    {
        delete this;
    }
};

