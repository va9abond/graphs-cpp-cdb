// [TODO]: is_tree (связность + ацикличность)
// [TODO]: Back removing
// [TODO]: Kruskal
// [TODO]: Prima
// [TODO]: Boruvka
//
// [TODO]: make ostov
// [TODO]: make min ostov


#ifndef GRAPHALGO_HPP
#define GRAPHALGO_HPP


#include <queue>
#include <stack>
#include <set>
#include <utility>
#include <algorithm>
#include "graph.hpp"
#include "dsf.hpp"


// template <
//     class Graph_t
// >
// inline void BFSvoid (const Graph_t& Graph, int Vstart = 1) {
//     std::queue<int> Queue;
//     std::set<int> Visited;
//
//     Queue.push(Vstart);
//     Visited.insert(Vstart);
//
//     while (!Queue.empty())
//     {
//         int Vnow = Queue.front();
//         const std::vector<int>& Neighbours = Graph[Vnow];
//
//         for (int Vnext {1}; Vnext <= Neighbours.size(); ++Vnext)
//         {
//             if (Neighbours[Vnext - 1]) { // Vnow connected with Vnext
//                 if (Visited.find(Vnext) != Visited.end()) {
//                     Visited.insert(Vnext);
//                     Queue.push(Vnext);
//                 }
//             }
//         }
//         Queue.pop();
//     }
// }
//
//
// template <
//     class Graph_t
// >
// inline void DFSvoid (
//     const Graph_t& Graph,
//     int Vnow, // [TODO]: Vnow is Visited.front() always??
//     std::set<int>& Visited
// ) {
//     // std::set<int> Visited;
//     const std::vector<int>& Neighbours = Graph[Vnow];
//
//     for (int Vnext {1}; Vnext <= Neighbours.size(); ++Vnext) {
//        if (Neighbours[Vnext - 1]) { // Vnow connected with Vnex
//             if (Visited.find(Vnext) != Visited.end()) {
//                 // Vnext doesn't be visited before
//                 Visited.insert(Vnext);
//                 DFS(Graph, Vnext, Visited);
//             }
//         }
//     }
// }


// [pseudocode]: generate Minimal Spannig Tree
// inline weighted_graph<int> generic_MST (const weighted_graph<int>& Graph) {
    // 0. init MST {Graph.m_Verts, empty_set};
    // 1. Choose random vert and insert it in MST.m_Verts
    // 2. while (MST is not Spanning Tree) {
    //        cutGraph = cutting_graph(respect wirh MST.m_Verts)
    //        find_all_safe_edges() {
    //            for (vi : Graph.m_Verts) {
    //                for (vj : Graph.m_Verts) {
    //                    if (vi in cutGraph.m_Verts && vj in Graph.m_Verts && vj not in cutGraph.m_Verts) {
    //                              edge e {vi, vj} is safe;
    //                              add e in safe_set
    //                    }
    //                }
    //            }
    //        };
    //        choose light edge from all sefe
    //        add safe edge to MST
    // }
// }

enum MST_Algo_t {
    Prim,
    Kruskal
};

template <
    MST_Algo_t algo_t
>
inline weighted_graph<int> generateMST (const weighted_graph<int>& Graph);

template <> // template specialization when generate MST by Kruskal's algorithm
inline weighted_graph<int> generateMST<Kruskal> (const weighted_graph<int>& Graph) {
    int countV = Graph.sizeV();

    weighted_graph<int> Result(countV);

    dsf dsf_verts(countV);
    for (int i = 0; i < countV; ++i) {
        dsf_verts.make_set(Graph.m_Verts[i]);
    }

    for (auto Eit = Graph.m_Edges.begin(); Eit != Graph.m_Edges.end(); ++Eit) {
        vert* U = (*Eit).sou;
        vert* V = (*Eit).tar;
        if (dsf_verts.find_set(U) != dsf_verts.find_set(V)) {
            dsf_verts.unite_sets(U,V);
#if 1
            Result.m_Verts[*U] = new vert(*U);
            Result.m_Verts[*V] = new vert(*V);
            std::cout << "old pointer U: " << U << " -> " << *U
                << "; old pointer V: " << V << " -> " << *V << "\n";
            std::cout << "new pointer U: " << Result.m_Verts[*U] << " -> " << *Result.m_Verts[*U]
                << "; new pointer V: " << Result.m_Verts[*V] << " -> " << *Result.m_Verts[*V] << "\n";
#else
            Result.m_Verts[*U] = U;
            Result.m_Verts[*V] = V;
#endif
            Result.m_Edges.insert({
                Result.m_Verts[*U],
                Result.m_Verts[*V],
                Graph.m_Weightfunc[*U][*V]
            });
        }
        if (Result.sizeE() == countV - 1) { break; }
    }
    Result.m_Weightfunc = Graph.m_Weightfunc;
    return Result;
}


template <
    class Weight_t
> void print (const weighted_graph<Weight_t>& Graph) {
    std::cout << "\nGraph: |V| = " << Graph.sizeV() << ", |E| = " << Graph.sizeE() << "\n";
    for (auto Eit = Graph.m_Edges.begin(); Eit != Graph.m_Edges.end(); ++Eit) {
        // std::cout << "hello";
        std::cout << "source: " << *(*Eit).sou << ";  target: " << *(*Eit).tar << " [" << (*Eit).wei << "]\n";
    }
}

#if 0

inline void DFS_main (const weighted_graph<int> Graph, const vert Vnow, std::vector<vert>& Used, std::vector<vert>& Vindxs) {
    Used[Vnow] = 1;
    std::vector<int> weightNbrs { Graph.m_Weightfunc[Vnow] }; // weights of edges with nbrs
    for (int i = 0; i < (int)weightNbrs.size(); ++i) { //
        if (weightNbrs[i] && !Used[i]) { // filter neighbours: just unvisiter yet
            DFS_main(Graph, i, Used, Vindxs);
            Vindxs.push_back(i);
        }
    }

}


inline std::vector<vert> DFSvoid (const weighted_graph<int> Graph) {
    int countV = Graph.sizeV();

    std::vector<int> Vindxs; // verts indexes
    std::vector<vert> Visited(countV, 0);

    for (int v = 0; v < countV ; ++v) {
        DFS_main(Graph, *Graph.m_Verts[v], Visited, Vindxs);
    }

    return Vindxs;
}

#endif

#endif // GRAPHALGO_HPP
