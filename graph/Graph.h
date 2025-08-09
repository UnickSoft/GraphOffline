#pragma once

#include <string>
#include <vector>
#include "IGraph.h"
#include "YString.h"
#include <memory>
#include <algorithm>
#include <variant>
#include <functional>
#include <unordered_map>

extern bool bDebug;

/**
 * Main Graph class.
 */
class Graph: public virtual IGraph
{
public:

    struct Node;
    struct Edge;

    using EdgePtr    = std::shared_ptr<Edge>;
    using WeightType = std::variant<IntWeightType, FloatWeightType>;

    // Node sturct
    struct Node
    {
        String id;
        ObjectId privateId;
        bool fake;
        IndexType index;

        Node(const String& id, IndexType privateId, bool fake, IndexType index)
        {
            this->id = id;
            this->privateId = privateId;
            this->fake = fake;
            this->index = index;
        }

        const std::vector<Node*>& GetTargets() const
        {
            return targets;
        }

        ObjectId GetEdge(IndexType index) const
        {
            return edges[index];
        }

        void RemoveEdge(ObjectId edgeId)
        {
            auto removeEdgesPosition = std::find(edges.begin(), edges.end(), edgeId);

            if (removeEdgesPosition != edges.end())
            {
                IndexType index = IndexType(removeEdgesPosition - edges.begin());
                edges.erase(edges.begin() +  index);
                targets.erase(targets.begin() + index);
            }
        }

        void AddToTargets(Node* node, ObjectId edgeId)
        {
            auto findNode = std::find(targets.begin(), targets.end(), node);
            if (findNode == targets.end())
            {
                targets.push_back(node);
                edges.push_back(edgeId);
            }
        }

    protected:
        std::vector<Node*> targets;
        std::vector<ObjectId> edges;
    };

    class NodePtr : public std::shared_ptr<Node>
    {
    public:
        NodePtr () : std::shared_ptr<Node>() {}
        explicit NodePtr (Node* node) : std::shared_ptr<Node>(node) {}
        // Operator to compare pointers.
        bool operator == (const Graph::Node* node2) const
        {
            return this->get() == node2;
        }
    };
    
    using IdToNode = std::unordered_map<ObjectId, NodePtr>;

    // Edge struct
    struct Edge
    {
        String id;
        NodePtr source;
        NodePtr target;
        bool  direct;
        WeightType weight;
        ObjectId privateId;

        Edge(const String& id, NodePtr source, NodePtr target, bool direct, const WeightType& weight, ObjectId privateId)
        {
            this->id = id;
            this->source = source;
            this->target = target;
            this->direct = direct;
            this->weight = weight;
            this->privateId = privateId;
        }

        template<typename WeightTypeTmpl> WeightTypeTmpl GetWeight() const
        {
            if (std::holds_alternative<WeightTypeTmpl>(weight))
            {
                auto* pointer = std::get_if<WeightTypeTmpl>(&weight);
                if (pointer)
                  return *pointer;
            }

            if (std::is_same<WeightTypeTmpl, IntWeightType>::value)
            {
                auto* pointer = std::get_if<FloatWeightType>(&weight);
                if (pointer)
                  return *pointer;
            }
            else
            {
                auto* pointer = std::get_if<IntWeightType>(&weight);
                if (pointer)
                  return *pointer;
            }

            return WeightTypeTmpl();
        }
    };

    Graph(void);
    virtual ~Graph(void);

    // Load from GraphML format.
    bool LoadFromGraphML(const char * pBuffer, uint32_t bufferSize);

    // Clear Graph.
    void Clear();

    // IGraph
    // Get Nodes count.
    virtual IndexType GetNodesCount() const override;
    // Get Edges count.
    virtual IndexType GetEdgesCount() const override;
    // Get node of this graph.
    virtual ObjectId GetNode(IndexType index) const override;
    // Get node of this graph.
    virtual ObjectId GetNode(const char* nodeId) const override;
    // Get connected graph count.
    virtual IndexType GetConnectedNodes(ObjectId source) const override;
    // Get connected graph for this graph.
    virtual ObjectId GetConnectedNode(ObjectId source, IndexType index) const override;
    // Get connected vertex index.
    virtual IndexType GetConnectedNodeIndex(ObjectId source, IndexType index) const override;
    // Is edge exists.
    virtual bool AreNodesConnected(ObjectId source, ObjectId target) const override;
    // Return graph string Id.
    virtual bool GetNodeStrId(ObjectId node, char* outBuffer, IndexType bufferSize) const override;
    // Return Edge str id
    virtual bool GetEdgeStrId(ObjectId edge, char* outBuffer, IndexType bufferSize) const override;
    // Is edge exists in input graph.
    virtual bool IsEgdeExists(ObjectId source, ObjectId target, bool onlyInSourceGraph = true) const override;
    // Get weight real type
    virtual EdgeWeightType GetEdgeWeightType() const override;
    // Create copy of graph.
    virtual IGraph* MakeBaseCopy(GraphCopyType type) const override;
    // Is graph directed or not.
    virtual bool HasDirected() const override;
    virtual bool HasUndirected() const override;
    // Run DFS and call callbacks.
    virtual void ProcessDFS(IEnumStrategy* pEnumStrategy, ObjectId startedNode) const override;
    // Remove edge from Graph. For undirected Graph it removes source -> target or target -> source
    virtual void RemoveEdge(ObjectId source, ObjectId target) override;
    // How many nodes are source for this node.
    virtual IndexType GetSourceNodesNumber(ObjectId source) override;

    // Add edge
    virtual bool AddEdge(ObjectId source, ObjectId target, bool direct, const FloatWeightType& weight) override;

    // Add node
    virtual ObjectId AddNode(bool fake) override;

    // Return graph struct
    virtual const char* PrintGraph() const override;

    // Find Node by Id
    NodePtr FindNode(const String& id) const;

    // Fake node
    bool IsFakeNode(ObjectId source) override;

    // Has multi graph
    bool IsMultiGraph() const override;

    ObjectId GetEdge(ObjectId source, ObjectId target) const override;

    // Remove node and all connected edges.
    void RemoveNode(ObjectId source) override;

    WeightType* GetEdgeWeight(ObjectId source, ObjectId target, const IndexType & index = 0) const;

    template<typename WeightTypeTmpl> WeightTypeTmpl GetEdgeWeight(ObjectId source, ObjectId target, const IndexType & index = 0) const
    {
        auto edge = FindEdge(source, target, index);
        return edge ? edge->GetWeight<WeightTypeTmpl>() : WeightTypeTmpl();
    }

    // Create copy of graph.
    virtual Graph* MakeGraphCopy(GraphCopyType type, const std::function<Graph*()> & createFunction = std::function<Graph*()>()) const;

    // Get edge of this graph.
    ObjectId GetEdge(IndexType index) const override;

    // Edge connected nodes
    NodePair GetEdgeData(IndexType index) const override;

    // Edge connected nodes
    NodePair GetEdgeData(ObjectId objectId) const override;

public:

    ObjectId AddNode(const String& idNode, IndexType privateId, bool fake);
    EdgePtr AddEdge(const String& id, IndexType sourceId, IndexType targetId, bool direct, const WeightType& weight, IndexType privateId);

protected:

    // Create own instance
    virtual Graph* CreateGraph() const;

    // Find Node by id in vector.
    template <typename T> T FindObject(const String& id, const std::vector<T>& nodes)  const;
    template <typename T> T FindObject(ObjectId objectId, const std::vector<T>& nodes) const;
    template <typename T> T FindObject(ObjectId objectId, const std::unordered_map<ObjectId, T>& search) const;

    // Find element in vector.
    template <typename T> bool Has(const std::vector<T>& vector, const T& value) const;
    template <typename T1, typename T2> bool Has(const std::vector<T1>& vector, const T2& value) const;

    // Is valid Object id.
    bool IsValidNodeId(ObjectId id, NodePtr& ptr) const;

    bool IsValidEdgeId(ObjectId id, EdgePtr& ptr) const;

    // Is edge exists.
    EdgePtr FindEdge(ObjectId source, ObjectId target, const IndexType & index = 0) const;

    // Simple make copy.
    Graph* MakeGraphCopy(const std::function<Graph*()> & createFunction) const;
    // Make current graph undirected.
    Graph* MakeGraphUndirected(const std::function<Graph*()> & createFunction) const;
    // Inverse Graph
    Graph* MakeGraphInverse(const std::function<Graph*()> & createFunction) const;
    // Remove self loop.
    Graph* MakeGraphRemoveSelfLoop(const std::function<Graph*()> & createFunction) const;
    // Remove negative edges
    virtual Graph* MakeGraphRemoveNegative(const std::function<Graph*()> & createFunction) const;
    // Convert current graph to its complement
    Graph* MakeGraphComplement(const std::function<Graph*()> & createFunction) const;

    bool IsDouble(double value);

    // Run DFS and call callbacks.
    void _ProcessDFS(IEnumStrategy* pEnumStrategy, Node* node) const;

    // Remove edge from Graph.
    void RemoveEdge(EdgePtr edge);

    EdgePtr GetEdgeById(ObjectId edgeId);

    const EdgePtr GetEdgeById(ObjectId edgeId) const;

    typedef std::vector<NodePtr> NodePtrVector;
    typedef std::vector<EdgePtr> EdgePtrVector;

    IndexType GetNextId();

    void CopyPropertiesTo(Graph* pGraph) const;

    // Update some state after private creation.
    void ForceUpdateStates();

    // List of graph.
    NodePtrVector m_nodes;
    EdgePtrVector m_edges;
    IdToNode m_idToNode;

    // ATTANTION: If you add new fields please update CopyProperties.
    EdgeWeightType m_weightType;
    static IndexType m_autoIncIndex;

    bool m_hasDirected;
    bool m_hasUndirected;
    bool m_isMultigraph;
};
