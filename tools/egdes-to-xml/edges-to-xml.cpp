#include <cassert>
#include <cstring>

#include <iostream>
#include <set>
#include <sstream>
#include <unordered_map>

#define XML_HEADER "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\"\nxmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\nxsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns\nhttp://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\">"
#define XML_FOOTER "</graphml>"

int main(int ac, const char *av[]) {
    if (ac < 3 || !(std::strcmp(av[2], "undirected") ^ std::strcmp(av[2], "directed"))) {
        std::cout << "usage: " << av[0] << " graphname [undirected|directed]\n";
        return -1;
    }

    const bool undirected = std::strcmp(av[2], "directed");

    int num_v, num_e;
    std::cin >> num_v >> num_e;

    std::unordered_map<int, std::set<int>> adj_lst; // not multigraph
    adj_lst.max_load_factor(0.5);
    adj_lst.reserve(num_v);
    int edges = num_e;
    int read_edges = 0;
    for (; not std::cin.eof() && edges; --edges) {
        int u, v;
        std::cin >> u >> v;
        if (adj_lst[v].count(u) == 0) {
            adj_lst[u].emplace(v);
            read_edges++;
        }
    }

    std::cerr << "read " << adj_lst.size() << "/" << num_v << " vertices and "
              << read_edges << "/" << (num_e - edges) << " edges\n";

    assert(num_v == adj_lst.size());
    assert(num_e == (num_e - edges));

    std::cout << XML_HEADER << "\n";
    std::cout << "<graph id=\"" << av[1] << "\" edgedefault=\"" << av[2] << "\">\n";

    for (const auto &[v, _] : adj_lst) {
        std::cout << "<node id=\"n" << v << "\"/>\n";
    }

    for (const auto &[v, nei] : adj_lst) {
        for (auto u : nei) {
            std::cout << "<edge source=\"n" << v << "\" target=\"n" << u << "\"/>\n";
        }
    }

    std::cout << "</graph>\n";
    std::cout << XML_FOOTER << "\n";
    return 0;
}
