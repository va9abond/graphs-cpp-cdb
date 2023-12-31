#ifndef GRAPHALGO_HPP
#define GRAPHALGO_HPP


#include <queue>
#include <stack>
#include <set>
#include <utility>
#include <algorithm>
#include "graph.hpp"
#include "dsf.hpp"


enum class MST_Algo_t {
    Prim,
    Kruskal
};

template <
    MST_Algo_t algo_t
>
inline weighted_graph<int> generateMST (const weighted_graph<int>& Graph);

template <> // template specialization when MST generating by Kruskal's algorithm
inline weighted_graph<int> generateMST<MST_Algo_t::Kruskal> (const weighted_graph<int>& Graph) {
    int countV = Graph.sizeV();

    int MSTcountE = 0;
    std::vector<std::vector<int>> res_weightfunc (countV, std::vector<int>(countV, 0));

    dsf dsf_verts(countV);
    for (int i = 0; i < countV; ++i) {
        dsf_verts.make_set(Graph.m_Verts[i]);
    }

    for (
        auto Eit = Graph.m_Edges.begin();
        Eit != Graph.m_Edges.end() && MSTcountE < countV - 1;
        ++Eit
    ) {
        vert* U = (*Eit).sou;
        vert* V = (*Eit).tar;
        if (dsf_verts.find_set(U) != dsf_verts.find_set(V)) {
            dsf_verts.unite_sets(U,V);
            // connect tree U with tree V
            res_weightfunc[*U][*V] = Graph.m_Weightfunc[*U][*V];
            res_weightfunc[*V][*U] = Graph.m_Weightfunc[*V][*U];
            ++MSTcountE;
        }
    }

    // construct MST graph by res_weightfunc
    weighted_graph<int> Result(res_weightfunc);
    return Result;
}


// ==== custom less for graph printing ====
template <
    class Weight_t
> bool custom_edge_less (const edge<Weight_t>& Lhs, const edge<Weight_t>& Rhs) {
    vert s = *Lhs.sou; vert rs = *Rhs.sou;
    vert t = *Lhs.tar; vert rt = *Rhs.tar;
    if (s != rs) {
        return s < rs;
    } else if (t != rt) {
        return t < rt;
    } else {
        return Lhs.wei < Rhs.wei;
    }
}

// ==== Graph printing ====
template <
    class Weight_t
> void print (const weighted_graph<Weight_t>& Graph) {
    std::cout << "\n========================================\n";
    std::cout << "Graph: |V| = " << Graph.sizeV() << ", |E| = " << Graph.sizeE() << "\n";
    std::cout << "========================================\n";
    printf("%40s\n", "Vertices");

    for (const auto& V : Graph.m_Verts) {
        std::cout << "(" << V << ") " << *V << "\n";
    }

    std::vector<edge<Weight_t>> edges (Graph.m_Edges.begin(), Graph.m_Edges.end());
    std::sort(edges.begin(), edges.end(), custom_edge_less<Weight_t>);

    printf("%40s\n", "Edges");
    for (auto Eit = edges.begin(); Eit != edges.end(); ++Eit) {
        printf("source: %2d; target: %2d; [%d]\n", *(*Eit).sou, *(*Eit).tar, (*Eit).wei);
    }
    printf("%40s\n", "End");
    std::cout << "========================================\n";
}

inline void DFSprint_main (const weighted_graph<int>& Graph, const vert* Vnow, std::vector<vert>& Visited, std::vector<vertptr>& Vindxs) {
    Visited[*Vnow] = 1;
    std::vector<int> weightNbrs { Graph.m_Weightfunc[*Vnow] }; // weights of edges with nbrs
    for (int vi = 0; vi < (int)weightNbrs.size(); ++vi) { //
        if (weightNbrs[vi] && !Visited[vi]) { // filter neighbours: just unvisiter yet
            DFSprint_main(Graph, Graph.m_Verts[vi], Visited, Vindxs);
            Vindxs.push_back(Graph.m_Verts[vi]);
        }
    }
}

inline std::vector<vertptr> DFSprint_init (const weighted_graph<int>& Graph) {
    int countV = Graph.sizeV();

    std::vector<vertptr> Vindxs; // verts indexes
    std::vector<vert> Visited (countV, 0);

    for (int vi = 0; vi < countV ; ++vi) {
        if (!Visited[vi]) {
            DFSprint_main(Graph, Graph.m_Verts[vi], Visited, Vindxs);
        }
    }

    return Vindxs;
}

inline void DFSprint (const weighted_graph<int>& Graph) {
    std::vector<vertptr> DFSresult = DFSprint_init(Graph);
    std::cout << "\n" << "{\n";
    for (const auto& V : DFSresult) {
        std::cout << "    (" << V << ")  " << *V << "\n";
    }
    std::cout << "};";
}


// ==== shortest-path problem algorithm type ====
enum class SP_Algo_t
{
    Bellman_Ford,
};

inline std::vector<vert> generate_path_ (std::vector<vertptr>& Preds, const vertptr Source, vertptr Target)
{
    // assert(Source != Target);
    std::vector<vert> Path;
    vertptr Vprev = Target;

    while (Vprev != Source) { // Preds[*Vprev], i.e. Preds[*Vstart] = nullptr;
        Path.push_back(*Vprev);
        Vprev = Preds[*Vprev];
    }
    Path.push_back(*Source);
    std::reverse(Path.begin(), Path.end());
    return Path;
}

template <
    SP_Algo_t algo_t
>
inline std::map<vertptr, std::pair<int, std::vector<vert>>> generateSP (
    const weighted_graph<int>&, const vertptr);

template<>
inline std::map<vertptr, std::pair<int, std::vector<vert>>> generateSP<SP_Algo_t::Bellman_Ford> (
    const weighted_graph<int>& Graph, const vertptr Source)
{
    using wedge = weighted_graph<int>::wedge;
    const int countV = Graph.sizeV();

    // init single source
    const int int_max = std::numeric_limits<int>::max();
    std::vector<int> estimates (countV, int_max);
    std::vector<vertptr> preds (countV, nullptr); // predecessor of each vert
    estimates[*Source] = 0;

    // main loop
    for (int i = {0}; i < countV - 1; ++i) {
        // relaxation
        for (const wedge& E : Graph.m_Edges) {
            int& sou_est = estimates[*E.sou];
            int& tar_est = estimates[*E.tar];
            bool is_need_relax = (sou_est < int_max) && (tar_est > sou_est + E.wei);

            if (is_need_relax) {
                tar_est = sou_est + E.wei;
                preds[*E.tar] = E.sou;
            }
        }
    }
    // check a negative loop
    for (const wedge& E : Graph.m_Edges) {
        // sou ---> tar
        assert(estimates[*E.tar] <= estimates[*E.sou] + E.wei);
    }

    // generate result
    std::map<vertptr, std::pair<int, std::vector<vert>>> general_res;
    for(int i = {0}; i < countV; ++i) {
        std::pair<int, std::vector<vert>> dis_path { estimates[i], generate_path_(preds, Source, Graph.m_Verts[i]) };
        general_res.emplace(Graph.m_Verts[i], dis_path);
    }
    return general_res;
}

// It's awful(
inline void generateSP_print (const std::map<vertptr, std::pair<int, std::vector<vert>>>& map, vertptr Source) {
    std::cout << "\n===== All Shortest Ways, source: " << *Source << " ====\n";
    printf("%-20s %s  path \n", "     target ", " dist. ");
    for (const auto& el : map) {
        auto path = el.second.second;
        printf("(%-14p) %2d %4d     ", (void*)el.first, *el.first, el.second.first);
        std::cout << " { ";
        for (const auto& v : path) {
            std::cout << v << " ";
        }
        std::cout << "}\n";
    }
}


// ==== maximum flow problem algorithm type (4.1) ====
enum class MF_Algo_t
{
    Edmonds_Karp, // Ford-Fulkerson + find path by bfs
};

template <
    MF_Algo_t algo_t
>
inline std::pair<int,std::vector<std::vector<int>>> generateMF ( // find maximum flow from Source to Target
    const weighted_graph<int>&, const vert, const vert
);

template <>
inline std::pair<int,std::vector<std::vector<int>>> generateMF<MF_Algo_t::Edmonds_Karp> (
    const weighted_graph<int>& Graph, const vert Source, const vert Target
) {
    using index_t = residual_network<int>::index_t;
    // using wedge = weighted_graph<int>::wedge;
    // const int countV = Graph.sizeV();

    residual_network<int> Rnet(Graph); // init residual network; now: zero flow
    auto ResultFlow (Rnet.m_Flow);     // init zero flow

    auto path = Rnet.find_path(Rnet.m_Verts[Source], Rnet.m_Verts[Target]); // path = std::pait<A,B> where
                                                    // A - c_f(p) - min residual capacity on path
                                                    // if Path not exist, then A = 0
                                                    // B - std::vector<vertptr> - augmenting path
    while (path.first) {
        for (index_t i = {0}; i < path.second.size() - 1; ++i) {
            vertptr s = path.second[i];
            vertptr t = path.second[i+1];
            if (Rnet.m_Weightfunc[*s][*t]) {
                Rnet.m_Flow[*s][*t] += path.first;
            } else {
                Rnet.m_Flow[*t][*s] -= path.first;
            }
        }
        Rnet.update_capacity();
        path = Rnet.find_path(Rnet.m_Verts[Source], Rnet.m_Verts[Target]);
    }

    return std::make_pair(Rnet.flow(Source), Rnet.m_Flow);
}


// ==== maximum matching problem algorithm type (4.2) ====
// based on maximum flow, but there are more algorithms: Hopcroft-Karp (p. 803)
        // 1. devide by teams
        // 2. construct extended graph EG
        // 3. find max flow on EG

inline std::pair<int,std::vector<std::vector<int>>> generateMM ( // find maximum matching
    const std::string& file_name
    ) {
    auto a = generate_2dvector_from_file_4_2(file_name);
    weighted_graph<int> bigraph (a.first);
    std::vector<vert> teamA { a.second };

    int border_index = teamA.size() - 1; // last index of teamA
                                         // all verts, which > index in team B
    std::vector<vert> teamB;
    for (int i {border_index}; i < bigraph.sizeV(); ++i) {
        teamB.push_back(i);
    }

    bigraph.extension(teamA, -1);
    bigraph.extension(teamB, -1);

    auto result = generateMF<MF_Algo_t::Edmonds_Karp>(bigraph, bigraph.sizeV()-1, bigraph.sizeV()-2);
    return result;
}


// ==== greedy coloring problem (4.3) ====
inline std::vector<int> greedy_coloring (const weighted_graph<int>& Graph) {
    auto sizeV = Graph.sizeV();
    std::vector<int> colors(sizeV, -1); // vertptr V colored in color[V]

    for (const vertptr vptr : Graph.m_Verts) {
        auto nbrs = Graph.m_Weightfunc[*vptr];
        int minColorNo = -1;
        for (int inbr {0}; inbr < sizeV; ++inbr) {
            if (nbrs[inbr]) {
                minColorNo = (minColorNo >= colors[inbr] ? minColorNo : colors[inbr]);
            }
        }
        colors[*vptr] = minColorNo + 1;
    }

    return colors;
}


// ==== hamiltonian loop problem ====
inline bool is_safe (
        const weighted_graph<int>& Graph,
        const std::vector<int>& Path,
        vert vto,
        vert vfrom
        ) {
    if (Graph.m_Weightfunc[Path[vto-1]][vfrom] == 0) {
        return false;
    }

    for (int i {0}; i < vto; ++i) {
        if (Path[i] == vfrom) {
            return false;
        }
    }
    return true;
}

inline bool find_next_vert (
        const weighted_graph<int>& Graph,
        std::vector<int>& Path,
        vert Pos
        ) {

    if (Pos == Graph.sizeV()) {
        if (Graph.m_Weightfunc[Path[Pos-1]][Path[0]]) {
            return true;
        } else {
            return false;
        }
    }

    for (vert vrt = 1; vrt < Graph.sizeV(); ++vrt) {
        if (is_safe(Graph, Path, Pos, vrt)) {
            Path[Pos] = vrt;

            if (find_next_vert(Graph, Path, Pos+1) == true) {
                return true;
            }
            Path[Pos] = -1;
        }
    }
    return false;
}

inline std::vector<int> generate_hamiltonian_loop (
        const weighted_graph<int>& Graph
        ) {
    std::vector<int> Path(Graph.sizeV(), -1);

    Path[0] = 0;
    if (find_next_vert(Graph, Path, 1) == false) {
        return {0,0};
    }

    return Path;
}

#endif // GRAPHALGO_HPP
