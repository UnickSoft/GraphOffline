//
//  MaxClique.h
//  Graphoffline
//
//  Created by Anas Rchid on 06/04/2022
//
//

#ifndef MAX_CLIQUE_HPP
#define MAX_CLIQUE_HPP

#define THREADS_PER_CORE 8

#include "BaseAlgorithm.h"
#include "IGraph.h"

#include <ciso646>
#include <atomic>
#include <shared_mutex>

class MaxClique : public BaseAlgorithm
{
public:
    MaxClique();
    virtual ~MaxClique();

    virtual const char* GetFullName() const override { return "Max Clique"; };
    virtual const char* GetShortName() const override { return "mc"; };

    virtual bool EnumParameter(IndexType index, AlgorithmParam* outParamInfo) const override;
    virtual void SetParameter(const AlgorithmParam* outParamInfo) override;

    virtual bool Calculate() override;

    virtual IndexType GetResultCount() const override;
    virtual AlgorithmResult GetResult(IndexType index) const override;

    virtual IndexType GetHightlightNodesCount() const override;
    virtual ObjectId GetHightlightNode(IndexType index) const override;
    virtual IndexType GetHightlightEdgesCount() const override;
    virtual NodesEdge GetHightlightEdge(IndexType index) const override;

    virtual void UnitTest() const override;

protected:
    enum struct Algorithm { Heuristic, Hybrid, Exact };
    std::vector<std::pair<Algorithm, std::string>> m_algo_types;

    void FindMaxClique(Algorithm algorithm_type);

    static const IndexType m_index_type_no_value = -1u;

    IndexType m_param_expected_size = m_index_type_no_value;
    IndexType m_param_lower_bound = 0, m_param_upper_bound = m_index_type_no_value;
    Algorithm m_param_algorithm_type = Algorithm::Heuristic;

private:
    using ColourType = IndexType;

    void LogState() const;

    void BranchHeuristic(std::uint32_t thread_id, ObjectId v, std::vector<ObjectId> &neighbours, std::vector<ObjectId> &clique, IndexType &max_clique_size, IndexType depth = 1);
    void BranchExact(std::uint32_t thread_id, ObjectId v, std::vector<ObjectId> &neighbours, std::vector<ColourType> &colours, std::vector<ObjectId> &clique, IndexType &max_clique_size, IndexType depth = 1);

    bool EnlargeClique(std::uint32_t thread_id, IndexType &max_clique_size, IndexType depth);
    std::vector<ObjectId> VertexNeighbours(ObjectId v, const std::vector<ObjectId> &neighbours);
    std::vector<ColourType> SortByGreedyColours(std::vector<ObjectId> &vertices);

    std::vector<ObjectId> m_vertices;
    mutable std::vector<NodesEdge> m_max_clique_edges;

    std::shared_mutex m_max_clique_mtx;
    std::vector<ObjectId> m_max_clique;
    IndexType m_overall_max_clique_size = 0;
    std::uint32_t m_max_clique_owner_thread_id = -1u;
    std::atomic_bool m_upper_bound_reached = false;

    std::uint32_t m_num_threads = std::thread::hardware_concurrency() * THREADS_PER_CORE;
};

#endif /* MAX_CLIQUE_HPP */
