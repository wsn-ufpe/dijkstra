// -*- c-basic-offset: 8; -*-
#ifndef WSNETWORK_HPP
#define WSNETWORK_HPP

#include <memory>
#include "wsnode.hpp"
#include "wschannel.hpp"

#define SFMT_MEXP 19937
#include "dSFMT.h"

class WSNetwork {
public:
        WSNetwork(int network_size, WSChannel* channel, int nbr_iter, int seed=654321);
        bool generate_random_network(double x_size, double y_size, double battery,
				     double msg_rate, double penalty);
	void calculate_neighborhoods();
	void calculate_consumption();
        bool is_connected();
	bool optimize_minimum_energy();
	bool optimize_maximum_lifetime();

	const WSNode* get_nodes() const { return _nodes.get(); }
	int get_network_size() const { return _network_size; }
	const int* get_routing_table() const { return _routing_table.get(); }
	const int* get_routes_len() const { return _routes_len.get(); }
	int get_best_lifetime() { return _best_min_lifetime; }
	const int* get_best_routing_table() const { return _best_routing_table.get(); }
	const int* get_best_routes_len() const { return _best_routes_len.get(); }

	double get_x_min() const { return _x_min; }
	double get_x_max() const { return _x_max; }
	double get_y_min() const { return _y_min; }
	double get_y_max() const { return _y_max; }

	WSNode* get_sink() { return &_nodes[_sink_idx]; }
	const WSNode* get_sink() const { return &_nodes[_sink_idx]; }
	void set_sink(WSNode* new_sink) { _nodes[_sink_idx] = *new_sink; }

protected:
	void update_traffic_along_route(int src_idx, const int* route,
					int route_len, double msg_rate);
	bool check_best();
	bool is_good_node(int node_idx) const;
	void set_neighbors(WSNode* node);
	void find_best_route(bool first_round, int node_idx);
	void add_penalties();
	void normalize_penalties();
	bool check_convergence();
	int* dijkstra(int src_node_idx, int* route, int* route_len);

	std::unique_ptr<WSNode[]> _nodes;
	std::unique_ptr<WSNode[]> _tmp_nodes;
	int _sink_idx;
        int _network_size;
	int _nbr_iter;
	double _x_min, _x_max;
	double _y_min, _y_max;
	std::unique_ptr<int[]> _routing_table;
	std::unique_ptr<int[]> _routes_len;
	std::unique_ptr<int[]> _best_routing_table;
	std::unique_ptr<int[]> _best_routes_len;
	WSChannel* _channel;
	double _best_min_lifetime;
	std::unique_ptr<double[]> _nodes_costs;
	std::unique_ptr<int[]> _predecessors;
        dsfmt_t _rng;

private:
	static const double MIN_SQR_DISTANCE;
	static const double MIN_SPREAD;
};

#endif // WSNETWORK_HPP
