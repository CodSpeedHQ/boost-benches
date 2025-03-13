#ifndef GRAPH_BENCH_H
#define GRAPH_BENCH_H

#include <benchmark/benchmark.h>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/astar_search.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>
#include <vector>
#include <utility>
#include <random>
#include <limits>

// Generate a random graph with given vertices and edges
template <typename Graph, typename WeightMap>
void generate_random_graph(Graph& g, WeightMap weight_map, int num_vertices, int num_edges, int max_weight = 100) {
  // Add vertices
  for (int i = 0; i < num_vertices; ++i) {
    boost::add_vertex(g);
  }
  
  // Random generator
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> vertex_dist(0, num_vertices - 1);
  std::uniform_int_distribution<> weight_dist(1, max_weight);
  
  // Add random edges
  for (int i = 0; i < num_edges; ++i) {
    int source = vertex_dist(gen);
    int target = vertex_dist(gen);
    int weight = weight_dist(gen);
    
    // Skip self-loops
    if (source != target) {
      // Check if edge already exists
      bool found = false;
      typename boost::graph_traits<Graph>::out_edge_iterator ei, ei_end;
      for (boost::tie(ei, ei_end) = boost::out_edges(source, g); ei != ei_end; ++ei) {
        if (boost::target(*ei, g) == target) {
          found = true;
          break;
        }
      }
      
      if (!found) {
        typename boost::graph_traits<Graph>::edge_descriptor e;
        bool inserted;
        boost::tie(e, inserted) = boost::add_edge(source, target, g);
        if (inserted) {
          weight_map[e] = weight;
        }
      }
    }
  }
}

// Benchmark for Dijkstra's shortest path algorithm
static void BM_BoostGraphDijkstra(benchmark::State& state) {
  // Graph parameters
  int num_vertices = state.range(0);
  int num_edges = num_vertices * state.range(1); // Average number of edges per vertex
  
  // Define the graph type
  typedef boost::adjacency_list<
    boost::vecS,                // OutEdgeList
    boost::vecS,                // VertexList
    boost::directedS,           // Directed graph
    boost::no_property          // Vertex properties
  > Graph;
  
  typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
  typedef boost::graph_traits<Graph>::edge_descriptor Edge;
  
  // Create graph
  Graph g;
  
  // Edge weights
  std::map<Edge, int> weights;
  
  // Create weight map
  boost::associative_property_map<std::map<Edge, int>> weight_map(weights);
  
  // Generate graph
  generate_random_graph(g, weight_map, num_vertices, num_edges);
  
  // Pick source vertex
  Vertex source = 0;
  
  for (auto _ : state) {
    // Vector for storing distances
    std::vector<int> distances(num_vertices);
    auto dist_map = boost::make_iterator_property_map(distances.begin(), boost::get(boost::vertex_index, g));
    
    // Vector for predecessors
    std::vector<Vertex> predecessors(num_vertices);
    auto pred_map = boost::make_iterator_property_map(predecessors.begin(), boost::get(boost::vertex_index, g));
    
    // Run Dijkstra's algorithm
    boost::dijkstra_shortest_paths(g, source, 
      boost::distance_map(dist_map).
      predecessor_map(pred_map).
      weight_map(weight_map));
    
    benchmark::DoNotOptimize(distances);
    benchmark::DoNotOptimize(predecessors);
  }
  
  state.counters["Vertices"] = num_vertices;
  state.counters["Edges"] = num_edges;
  state.counters["EdgeDensity"] = static_cast<double>(num_edges) / num_vertices;
}
BENCHMARK(BM_BoostGraphDijkstra)
  ->Args({100, 5})     // Small graph (100 vertices, ~5 edges per vertex)
  ->Args({1000, 10})   // Medium graph (1000 vertices, ~10 edges per vertex)
  ->Args({5000, 20});  // Large graph (5000 vertices, ~20 edges per vertex)

// A* heuristic for grid-based graphs
template <typename Graph, typename PositionMap>
class ManhattanDistanceHeuristic : public boost::astar_heuristic<Graph, int> {
public:
  typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
  
  ManhattanDistanceHeuristic(Vertex goal, const PositionMap& pos_map)
    : m_goal(goal), m_position_map(pos_map) {}
  
  int operator()(Vertex v) const {
    std::pair<int, int> pos_v = m_position_map[v];
    std::pair<int, int> pos_goal = m_position_map[m_goal];
    
    return std::abs(pos_v.first - pos_goal.first) +
           std::abs(pos_v.second - pos_goal.second);
  }
  
private:
  Vertex m_goal;
  const PositionMap& m_position_map;
};

// Visitor for A* search termination
template <typename Vertex>
class AStarGoalVisitor : public boost::default_astar_visitor {
public:
  AStarGoalVisitor(Vertex goal) : m_goal(goal) {}
  
  template <typename Graph>
  void examine_vertex(Vertex u, const Graph&) {
    if (u == m_goal) {
      throw std::runtime_error("Goal reached");
    }
  }
  
private:
  Vertex m_goal;
};

// Generate a grid graph for A* search
template <typename Graph, typename PositionMap, typename WeightMap>
void generate_grid_graph(Graph& g, PositionMap pos_map, WeightMap weight_map, int width, int height) {
  typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
  typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
  
  // Create vertices representing grid points
  std::vector<Vertex> vertices(width * height);
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      Vertex v = boost::add_vertex(g);
      pos_map[v] = std::make_pair(x, y);
      vertices[y * width + x] = v;
    }
  }
  
  // Add edges between adjacent grid points
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      Vertex current = vertices[y * width + x];
      
      // Connect to neighbors (4-way connectivity)
      // Right neighbor
      if (x + 1 < width) {
        Vertex right = vertices[y * width + (x + 1)];
        Edge e;
        bool inserted;
        boost::tie(e, inserted) = boost::add_edge(current, right, g);
        if (inserted) {
          // Add weight (can be based on some terrain cost)
          weight_map[e] = 1;
        }
      }
      
      // Down neighbor
      if (y + 1 < height) {
        Vertex down = vertices[(y + 1) * width + x];
        Edge e;
        bool inserted;
        boost::tie(e, inserted) = boost::add_edge(current, down, g);
        if (inserted) {
          weight_map[e] = 1;
        }
      }
      
      // Add diagonal neighbors for more complex paths
      // Down-right neighbor
      if (x + 1 < width && y + 1 < height) {
        Vertex down_right = vertices[(y + 1) * width + (x + 1)];
        Edge e;
        bool inserted;
        boost::tie(e, inserted) = boost::add_edge(current, down_right, g);
        if (inserted) {
          // Diagonal has slightly higher weight
          weight_map[e] = 14; // ~sqrt(2) * 10
        }
      }
      
      // Down-left neighbor
      if (x > 0 && y + 1 < height) {
        Vertex down_left = vertices[(y + 1) * width + (x - 1)];
        Edge e;
        bool inserted;
        boost::tie(e, inserted) = boost::add_edge(current, down_left, g);
        if (inserted) {
          weight_map[e] = 14; // ~sqrt(2) * 10
        }
      }
    }
  }
  
  // Add some random obstacles (by removing edges)
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> vertex_dist(0, width * height - 1);
  
  int num_obstacles = (width * height) / 10; // 10% obstacles
  for (int i = 0; i < num_obstacles; ++i) {
    int idx = vertex_dist(gen);
    Vertex v = vertices[idx];
    
    // Remove all edges connected to this vertex
    boost::clear_vertex(v, g);
  }
}

// Benchmark for A* search algorithm
static void BM_BoostGraphAStar(benchmark::State& state) {
  // Grid parameters
  int width = state.range(0);
  int height = state.range(0); // Square grid
  
  typedef boost::adjacency_list<
    boost::vecS, 
    boost::vecS, 
    boost::bidirectionalS
  > Graph;
  
  typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
  typedef boost::graph_traits<Graph>::edge_descriptor Edge;
  
  for (auto _ : state) {
    // Create a new grid graph for each run
    Graph g;
    
    // Create property maps
    std::map<Vertex, std::pair<int, int>> positions;
    std::map<Edge, int> weights;
    
    boost::associative_property_map<std::map<Vertex, std::pair<int, int>>> pos_map(positions);
    boost::associative_property_map<std::map<Edge, int>> weight_map(weights);
    
    // Generate grid graph
    generate_grid_graph(g, pos_map, weight_map, width, height);
    
    // Choose source and target vertices (opposite corners)
    Vertex start = 0; // Top-left
    Vertex goal = boost::num_vertices(g) - 1; // Bottom-right
    
    // Prepare data structures for A* search
    std::vector<Vertex> predecessors(boost::num_vertices(g));
    std::vector<int> distances(boost::num_vertices(g), std::numeric_limits<int>::max());
    std::vector<int> costs(boost::num_vertices(g));
    std::vector<boost::default_color_type> colors(boost::num_vertices(g));
    
    // Property maps
    auto pred_map = boost::make_iterator_property_map(predecessors.begin(), boost::get(boost::vertex_index, g));
    auto dist_map = boost::make_iterator_property_map(distances.begin(), boost::get(boost::vertex_index, g));
    auto cost_map = boost::make_iterator_property_map(costs.begin(), boost::get(boost::vertex_index, g));
    auto color_map = boost::make_iterator_property_map(colors.begin(), boost::get(boost::vertex_index, g));
    
    // Initialize start vertex
    distances[start] = 0;
    
    // Create heuristic functor
    ManhattanDistanceHeuristic<Graph, boost::associative_property_map<std::map<Vertex, std::pair<int, int>>>> 
      heuristic(goal, pos_map);
    
    // Create visitor
    AStarGoalVisitor<Vertex> visitor(goal);
    
    try {
      // Run A* search
      boost::astar_search(g, start, heuristic,
        boost::predecessor_map(pred_map).
        distance_map(dist_map).
        weight_map(weight_map).
        visitor(visitor).
        rank_map(cost_map).
        color_map(color_map));
    }
    catch (std::runtime_error&) {
      // Goal was found
    }
    
    benchmark::DoNotOptimize(distances);
    benchmark::DoNotOptimize(predecessors);
  }
  
  state.counters["GridSize"] = width * height;
  state.counters["Width"] = width;
  state.counters["Height"] = height;
}
BENCHMARK(BM_BoostGraphAStar)
  ->Arg(20)    // Small grid (20x20)
  ->Arg(50)    // Medium grid (50x50)
  ->Arg(100);  // Large grid (100x100)

// Benchmark for Breadth-First Search algorithm
static void BM_BoostGraphBFS(benchmark::State& state) {
  // Graph parameters
  int num_vertices = state.range(0);
  int num_edges = num_vertices * state.range(1); // Average number of edges per vertex
  
  // Define the graph type
  typedef boost::adjacency_list<
    boost::vecS,       // OutEdgeList
    boost::vecS,       // VertexList
    boost::undirectedS // Undirected graph
  > Graph;
  
  typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
  typedef boost::graph_traits<Graph>::edge_descriptor Edge;
  
  // Create graph
  Graph g;
  
  // Edge weights (not used for BFS, but needed for interface)
  std::map<Edge, int> weights;
  boost::associative_property_map<std::map<Edge, int>> weight_map(weights);
  
  // Generate graph
  generate_random_graph(g, weight_map, num_vertices, num_edges);
  
  // Pick source vertex
  Vertex source = 0;
  
  for (auto _ : state) {
    // Prepare data structures for BFS
    std::vector<Vertex> predecessors(num_vertices, 
                                   boost::graph_traits<Graph>::null_vertex());
    std::vector<int> distances(num_vertices, -1);
    
    // Property maps
    auto pred_map = boost::make_iterator_property_map(predecessors.begin(), 
                                                     boost::get(boost::vertex_index, g));
    auto dist_map = boost::make_iterator_property_map(distances.begin(), 
                                                     boost::get(boost::vertex_index, g));
    
    // Run BFS
    boost::breadth_first_search(g, source, 
      boost::visitor(
        boost::make_bfs_visitor(
          std::make_pair(
            boost::record_distances(dist_map, boost::on_tree_edge()),
            boost::record_predecessors(pred_map, boost::on_tree_edge())
          )
        )
      )
    );
    
    benchmark::DoNotOptimize(distances);
    benchmark::DoNotOptimize(predecessors);
  }
  
  state.counters["Vertices"] = num_vertices;
  state.counters["Edges"] = num_edges;
  state.counters["EdgeDensity"] = static_cast<double>(num_edges) / num_vertices;
}
BENCHMARK(BM_BoostGraphBFS)
  ->Args({100, 5})     // Small graph (100 vertices, ~5 edges per vertex)
  ->Args({1000, 10})   // Medium graph (1000 vertices, ~10 edges per vertex)
  ->Args({5000, 20});  // Large graph (5000 vertices, ~20 edges per vertex)

// DFS Visitor to count components
class DFSVisitor : public boost::default_dfs_visitor {
public:
  DFSVisitor(int* count) : m_count(count) {}
  
  template <typename Vertex, typename Graph>
  void start_vertex(Vertex, const Graph&) {
    (*m_count)++;
  }
  
private:
  int* m_count;
};

// Benchmark for Depth-First Search algorithm
static void BM_BoostGraphDFS(benchmark::State& state) {
  // Graph parameters
  int num_vertices = state.range(0);
  int num_edges = num_vertices * state.range(1); // Average number of edges per vertex
  
  // Define the graph type
  typedef boost::adjacency_list<
    boost::vecS,       // OutEdgeList
    boost::vecS,       // VertexList
    boost::undirectedS // Undirected graph
  > Graph;
  
  typedef boost::graph_traits<Graph>::edge_descriptor Edge;
  
  // Create graph
  Graph g;
  
  // Edge weights (not used for DFS, but needed for interface)
  std::map<Edge, int> weights;
  boost::associative_property_map<std::map<Edge, int>> weight_map(weights);
  
  // Generate graph
  generate_random_graph(g, weight_map, num_vertices, num_edges);
  
  for (auto _ : state) {
    // Vector for vertex colors (used by DFS to track visited vertices)
    std::vector<boost::default_color_type> colors(num_vertices);
    auto color_map = boost::make_iterator_property_map(colors.begin(), 
                                                      boost::get(boost::vertex_index, g));
    
    // Count number of connected components
    int component_count = 0;
    DFSVisitor visitor(&component_count);
    
    // Run DFS
    boost::depth_first_search(g, boost::visitor(visitor).color_map(color_map));
    
    benchmark::DoNotOptimize(colors);
    benchmark::DoNotOptimize(component_count);
  }
  
  state.counters["Vertices"] = num_vertices;
  state.counters["Edges"] = num_edges;
  state.counters["EdgeDensity"] = static_cast<double>(num_edges) / num_vertices;
}
BENCHMARK(BM_BoostGraphDFS)
  ->Args({100, 5})     // Small graph (100 vertices, ~5 edges per vertex)
  ->Args({1000, 10})   // Medium graph (1000 vertices, ~10 edges per vertex)
  ->Args({5000, 20});  // Large graph (5000 vertices, ~20 edges per vertex)

#endif // GRAPH_BENCH_H