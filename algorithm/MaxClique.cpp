//
//  MaxClique.cpp
//  Graphoffline
//
//  Created by Anas Rchid on 06/04/2022
//
//

#include "MaxClique.h"
#include "Utils.h"
#include "Logger.h"

#include <algorithm>
#include <execution>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <unordered_set>

MaxClique::MaxClique()
{
    m_algo_types.emplace_back(Algorithm::Exact, "exact");
    m_algo_types.emplace_back(Algorithm::Heuristic, "heuristic");
    m_algo_types.emplace_back(Algorithm::Hybrid, "hybrid");
}

MaxClique::~MaxClique()
{
}

bool MaxClique::EnumParameter(IndexType index, AlgorithmParam* outParamInfo) const
{
    switch(index)
    {
        case 0:
        case 1:
        {
            std::strncpy(outParamInfo->paramName, index ? "ub" : "lb", ALGO_PARAM_STRING_SIZE); // upper/lower bounds
            outParamInfo->type = APT_NUMBER;
            return true;
        }

        case 2:
        {
            std::strncpy(outParamInfo->paramName, "algo", ALGO_PARAM_STRING_SIZE); // which algorithm type
            outParamInfo->type = APT_STRING;
            return true;
        }

        case 3:
        {
            std::strncpy(outParamInfo->paramName, "expected", ALGO_PARAM_STRING_SIZE); // expected max clique size
            outParamInfo->type = APT_NUMBER;
            return true;
        }

        case 4:
        {
          std::strncpy(outParamInfo->paramName, "threads", ALGO_PARAM_STRING_SIZE); // expected max clique size
          outParamInfo->type = APT_NUMBER;
          return true;
        }

        default:
        {
            return false;
        }
    }
}

void MaxClique::SetParameter(const AlgorithmParam* outParamInfo)
{
    if (std::strncmp(outParamInfo->paramName, "expected", ALGO_PARAM_STRING_SIZE) == 0)
    {
        m_param_expected_size = std::abs((int)outParamInfo->data.val);
    }
    else if (std::strncmp(outParamInfo->paramName, "ub", ALGO_PARAM_STRING_SIZE) == 0)
    {
        m_param_upper_bound = std::abs((int)outParamInfo->data.val);
    }
    else if (std::strncmp(outParamInfo->paramName, "lb", ALGO_PARAM_STRING_SIZE) == 0)
    {
        m_param_lower_bound = std::abs((int)outParamInfo->data.val);
    }
    else if (std::strncmp(outParamInfo->paramName, "algo", ALGO_PARAM_STRING_SIZE) == 0)
    {
        bool found = false;
        for (const auto &algo : m_algo_types)
        {
            if (std::strncmp(outParamInfo->data.str, algo.second.c_str(), ALGO_PARAM_STRING_SIZE) == 0)
            {
                m_param_algorithm_type = algo.first;
                found = true;
                break;
            }
        }
        if (not found)
        {
            std::string str_err = std::string(outParamInfo->data.str) + " no such algorithm";
            throw std::invalid_argument(str_err.c_str());
        }
    } 
    else if (std::strncmp(outParamInfo->paramName, "threads", ALGO_PARAM_STRING_SIZE) == 0)
    {
      m_num_threads = std::abs((int)outParamInfo->data.val);
    }

    if (m_param_lower_bound > m_param_upper_bound) {
        std::string str_err = "bounds are incorrect "
            + std::to_string(m_param_lower_bound) + " > " + std::to_string(m_param_upper_bound);
        throw std::invalid_argument(str_err.c_str());
    }
}

void MaxClique::LogState() const
{
    std::string algo_s;
    for (const auto &algo : m_algo_types)
    {
        if (m_param_algorithm_type == algo.first)
        {
            algo_s = algo.second;
            break;
        }
    }

    LOG_INFO("Find a max clique in the range [" << m_param_lower_bound << ", " << m_param_upper_bound << "]"
             << " using Algorithm " << algo_s << " using " << m_num_threads
             << " Threads");
}

bool MaxClique::Calculate()
{
    LogState();

    if (m_param_upper_bound < 2 || m_pGraph->GetNodesCount() == 0)
    {
        if (m_param_upper_bound != 0 && m_pGraph->GetNodesCount() != 0)
        {
            m_max_clique.emplace_back(m_pGraph->GetNode((IndexType)0));
        }
        return true;
    }

    std::unordered_map<ObjectId, IndexType> neighbours_degs;

    neighbours_degs.max_load_factor(0.5);
    neighbours_degs.reserve(m_pGraph->GetNodesCount());

    m_vertices.reserve(m_pGraph->GetNodesCount());

    for (IndexType i = 0; i < m_pGraph->GetNodesCount(); ++i)
    {
        ObjectId v = m_pGraph->GetNode(i);
        m_vertices.emplace_back(v);

        for (IndexType j = 0; j < m_pGraph->GetConnectedNodes(v); ++j)
        {
            neighbours_degs[v] += m_pGraph->GetConnectedNodes(m_pGraph->GetConnectedNode(v, j));
        }
    }

    std::sort(std::execution::par_unseq, m_vertices.begin(), m_vertices.end(),
              [&neighbours_degs, this](ObjectId u, ObjectId v) {
                  IndexType u_deg = m_pGraph->GetConnectedNodes(u);
                  IndexType v_deg = m_pGraph->GetConnectedNodes(v);
                  return u_deg > v_deg || (u_deg == v_deg && neighbours_degs[u] > neighbours_degs[v]);
              });

    m_overall_max_clique_size = m_param_lower_bound > 0 ? m_param_lower_bound - 1 : 0;
    if (m_param_algorithm_type == Algorithm::Hybrid)
    {
        std::vector<ObjectId> vertices_copy = m_vertices;

        FindMaxClique(Algorithm::Heuristic);
        if (m_overall_max_clique_size != m_param_upper_bound)
        {
            m_upper_bound_reached = false;
            m_max_clique_owner_thread_id = m_index_type_no_value;
            m_vertices = std::move(vertices_copy);
            FindMaxClique(Algorithm::Exact);
        }
    }
    else
    {
        FindMaxClique(m_param_algorithm_type);
    }

    if (m_max_clique.size() != m_param_expected_size && m_param_expected_size != m_index_type_no_value)
    {
        LOG_WARNING("Max Clique size " << m_max_clique.size() << " is "
                    << (m_max_clique.size() < m_param_expected_size ? "less" : "greater")
                    << " than expected " << m_param_expected_size);
    }
    else if (m_max_clique.size() < m_param_lower_bound)
    {
        LOG_INFO("No Max Clique found for a lower bound of " << m_param_lower_bound);
    }

    return not m_max_clique.empty();
}

IndexType MaxClique::GetResultCount() const
{
    return m_max_clique.size() + 1;
}

AlgorithmResult MaxClique::GetResult(IndexType index) const
{
    static IndexType local_index = 1;

    AlgorithmResult res;
    if (index == 0)
    {
        local_index = 0;
        res.type = ART_INT;
        res.nValue = m_max_clique.size();
    }
    else if (local_index < m_max_clique.size())
    {
        res.type = ART_NODE_ID;
        res.nodeId = m_max_clique[local_index++];
    }

    return res;
}

IndexType MaxClique::GetHightlightNodesCount() const
{
    return m_max_clique.size();
}

ObjectId MaxClique::GetHightlightNode(IndexType index) const
{
    return m_max_clique[index];
}

IndexType MaxClique::GetHightlightEdgesCount() const
{
    if (m_max_clique_edges.empty())
    {
        m_max_clique_edges.reserve(m_max_clique.size() * (m_max_clique.size() - 1));

        for (ObjectId v : m_max_clique)
        {
            for (ObjectId u : m_max_clique)
            {
                if (u != v)
                {
                    NodesEdge edge;

                    edge.source = u;
                    edge.target = v;
                    edge.edgeId = m_pGraph->GetEdge(u, v);

                    m_max_clique_edges.emplace_back(std::move(edge));
                }
            }
        }
    }

    return m_max_clique_edges.size();
}

NodesEdge MaxClique::GetHightlightEdge(IndexType index) const
{
    return m_max_clique_edges[index];
}

void MaxClique::UnitTest() const
{
    std::ostringstream oss;

    for (ObjectId v : m_max_clique)
    {
        for (ObjectId u : m_max_clique)
        {
            if (u != v)
            {
                if (not m_pGraph->AreNodesConnected(v, u))
                {
                    oss << "Invalid Clique! No edge between " << v << " and " << u;
                    throw std::runtime_error(oss.str().c_str());
                }
            }
        }
    }

    if (m_max_clique.size() < m_param_expected_size && m_param_expected_size != m_index_type_no_value)
    {
        oss << "Max Clique size " << m_max_clique.size() << " is less than expected " << m_param_expected_size;
        throw std::runtime_error(oss.str().c_str());
    }
}

void MaxClique::FindMaxClique(Algorithm algorithm_type)
{
    std::shared_mutex thread_mtx;
    std::condition_variable_any thread_pool;
    std::vector<std::thread> threads(m_num_threads);
    std::vector<bool> available(m_num_threads, true);

    std::unordered_set<ObjectId> pruned;
    pruned.max_load_factor(0.5);
    pruned.reserve(m_pGraph->GetNodesCount());

    bool abort_search = false;
    while (not m_vertices.empty() && not abort_search && not m_upper_bound_reached)
    {
        std::vector<ColourType> colours = SortByGreedyColours(m_vertices);

        {
            std::unique_lock thread_lock(thread_mtx);
            thread_pool.wait(thread_lock, [&available] {
                return std::any_of(available.begin(), available.end(),
                                   [](bool th) { return th; });
            });
        }

        if (m_upper_bound_reached)
        {
            break;
        }

        IndexType current_max_clique_size;
        {
            std::shared_lock clique_lock(m_max_clique_mtx);
            current_max_clique_size = m_overall_max_clique_size;
        }

        for (std::uint32_t thread_id = 0; thread_id < m_num_threads
                 && not m_vertices.empty() && not m_upper_bound_reached; ++thread_id)
        {
            if (available[thread_id])
            {
                available[thread_id] = false;

                ColourType current_colour = colours.back();
                colours.pop_back();

                if (current_colour < current_max_clique_size)
                {
                    abort_search = true;
                    break;
                }

                std::thread &current_thread = threads[thread_id];
                if (current_thread.joinable())
                {
                    current_thread.join();
                }

                ObjectId v = m_vertices.back();
                m_vertices.pop_back();

                std::mutex read_mtx;
                bool read_flag = false;
                std::condition_variable reading;
                current_thread = std::thread([&, this](std::uint32_t thread_id, ObjectId v,
                                                       IndexType local_max_clique_size) {
                    EndOfScopeExecutor release_thread([thread_id, &thread_mtx,
                                                       &available, &thread_pool] {
                        {
                            std::shared_lock thread_lock(thread_mtx);
                            available[thread_id] = true;
                        }
                        thread_pool.notify_one();
                    });

                    std::vector<ObjectId> local_neighbours;
                    local_neighbours.reserve(m_pGraph->GetConnectedNodes(v));

                    for (int i = 0; i < m_pGraph->GetConnectedNodes(v); ++i)
                    {
                        ObjectId u = m_pGraph->GetConnectedNode(v, i);
                        if (pruned.count(u) == 0)
                        {
                            local_neighbours.emplace_back(u);
                        }
                    }
                    pruned.emplace(v);

                    {
                        std::unique_lock read_lock(read_mtx);
                        read_flag = true;
                    }
                    reading.notify_one();

                    if (local_neighbours.empty())
                    {
                        abort_search = true;
                        return;
                    }

                    std::vector<ColourType> local_colours = SortByGreedyColours(local_neighbours);
                    if (local_colours.back() < local_max_clique_size)
                    {
                        abort_search = true;
                        return;
                    }

                    std::vector<ObjectId> local_clique;
                    if (m_param_algorithm_type == Algorithm::Heuristic)
                    {
                        BranchHeuristic(thread_id, v, local_neighbours, local_clique, local_max_clique_size);
                    }
                    else
                    {
                        BranchExact(thread_id, v, local_neighbours, local_colours, local_clique, local_max_clique_size);
                    }

                    {
                        std::unique_lock clique_lock(m_max_clique_mtx);
                        if (local_clique.size() > m_max_clique.size() && thread_id == m_max_clique_owner_thread_id)
                        {
                            m_max_clique = std::move(local_clique);
                        }
                    }
                }, thread_id, v, current_max_clique_size);

                std::unique_lock read_lock(read_mtx);
                reading.wait(read_lock, [&read_flag] { return read_flag; });
            }
        }
    }

    for (auto &th : threads)
    {
        if (th.joinable())
        {
            th.join();
        }
    }

}

void MaxClique::BranchHeuristic(std::uint32_t thread_id, ObjectId v, std::vector<ObjectId> &neighbours,
                                std::vector<ObjectId> &clique, IndexType &max_clique_size, IndexType depth)
{
    if (m_upper_bound_reached)
    {
        return;
    }

    {
        std::shared_lock clique_lock(m_max_clique_mtx);
        max_clique_size = m_overall_max_clique_size;
    }

    ObjectId u = neighbours.back();
    neighbours.pop_back();

    const IndexType prev_max_clique_size = max_clique_size;
    const IndexType next_depth = depth + 1;

    std::vector<ObjectId> new_neighbours = VertexNeighbours(u, neighbours);
    if (new_neighbours.empty() || next_depth == m_param_upper_bound)
    {
        if (EnlargeClique(thread_id, max_clique_size, next_depth))
        {
            if (next_depth == m_param_upper_bound)
            {
                m_upper_bound_reached = true;
            }
            clique.clear();
            clique.reserve(next_depth);
            clique.emplace_back(u);
        }
    }
    else
    {
        std::vector<ColourType> new_colours = SortByGreedyColours(new_neighbours);
        if (next_depth + new_colours.back() > max_clique_size)
        {
            BranchHeuristic(thread_id, u, new_neighbours, clique, max_clique_size, next_depth);
        }
    }

    if (prev_max_clique_size < max_clique_size && thread_id == m_max_clique_owner_thread_id)
    {
        clique.emplace_back(v);
    }
}

void MaxClique::BranchExact(std::uint32_t thread_id, ObjectId v, std::vector<ObjectId> &neighbours,
                            std::vector<ColourType> &colours, std::vector<ObjectId> &clique,
                            IndexType &max_clique_size, IndexType depth)
{
    {
        std::shared_lock clique_lock(m_max_clique_mtx);
        max_clique_size = m_overall_max_clique_size;
    }

    const IndexType next_depth = depth + 1;
    while (not neighbours.empty() && not m_upper_bound_reached && depth + colours.back() > max_clique_size)
    {
        ObjectId u = neighbours.back();
        neighbours.pop_back();
        colours.pop_back();

        const IndexType prev_max_clique_size = max_clique_size;

        std::vector<ObjectId> new_neighbours = VertexNeighbours(u, neighbours);
        if (new_neighbours.empty() || next_depth == m_param_upper_bound)
        {
            if (EnlargeClique(thread_id, max_clique_size, next_depth))
            {
                if (next_depth == m_param_upper_bound)
                {
                    m_upper_bound_reached = true;
                }
                clique.clear();
                clique.reserve(next_depth);
                clique.emplace_back(u);
            }
        }
        else
        {
            std::vector<ColourType> new_colours = SortByGreedyColours(new_neighbours);

            if (next_depth + new_colours.back() > max_clique_size)
            {
                BranchExact(thread_id, u, new_neighbours, new_colours, clique, max_clique_size, depth + 1);
            }
        }

        if (prev_max_clique_size < max_clique_size && thread_id == m_max_clique_owner_thread_id)
        {
            clique.emplace_back(v);
        }
    }
}

bool MaxClique::EnlargeClique(std::uint32_t thread_id, IndexType &max_clique_size, IndexType depth)
{

    bool flag = false;

    std::shared_lock clique_shared_lock(m_max_clique_mtx);
    max_clique_size = m_overall_max_clique_size;
    if (depth > m_overall_max_clique_size)
    {
        clique_shared_lock.unlock();
        {
            std::unique_lock clique_unique_lock(m_max_clique_mtx);
            IndexType prev_overall_max_clique_size = m_overall_max_clique_size;
            m_overall_max_clique_size = std::max(m_overall_max_clique_size, depth);
            if (prev_overall_max_clique_size < m_overall_max_clique_size)
            {
                m_max_clique_owner_thread_id = thread_id;
            }
        }
        clique_shared_lock.lock();
        flag = m_max_clique_owner_thread_id == thread_id;
        max_clique_size = m_overall_max_clique_size;
    }

    return flag;
}

std::vector<ObjectId> MaxClique::VertexNeighbours(ObjectId v, const std::vector<ObjectId> &neighbours)
{
    std::vector<ObjectId> new_neighbours;
    new_neighbours.reserve(neighbours.size());

    for (ObjectId u : neighbours)
    {
        if (m_pGraph->AreNodesConnected(v, u))
        {
            new_neighbours.emplace_back(u);
        }
    }

    return new_neighbours;
}


std::vector<MaxClique::ColourType> MaxClique::SortByGreedyColours(std::vector<ObjectId> &vertices)
{
    std::vector<std::vector<ObjectId>> colour_classes(vertices.size());
    for (auto &colour_class : colour_classes)
    {
        colour_class.reserve(vertices.size());
    }

    IndexType num_colours = 0;
    for (ObjectId v : vertices)
    {
        ColourType colour = 0;
        while (std::any_of(colour_classes[colour].begin(), colour_classes[colour].end(),
                           [&v, this](ObjectId u) { return m_pGraph->AreNodesConnected(u, v); }))
        {
            colour++;
        }
        colour_classes[colour].emplace_back(v);
        num_colours = std::max(num_colours, colour + 1);
    }
    vertices.clear();

    std::vector<ColourType> colours;
    colours.reserve(num_colours);
    for (ColourType colour = 0; colour < num_colours; ++colour)
    {
        for (ObjectId v : colour_classes[colour])
        {
            vertices.emplace_back(v);
            colours.emplace_back(colour + 1);
        }
    }

    return colours;
}
