// -*- c-basic-offset: 4; -*-
#include <iostream>
#include <float.h>
#include <math.h>
#include "wsnetwork.hpp"
#include "wschannel.hpp"


const double WSNetwork::MIN_SQR_DISTANCE = 1.0;
const double WSNetwork::MIN_SPREAD = 0.01;


WSNetwork::WSNetwork(int network_size, WSChannel* channel, int nbr_iter, int seed)
    : _nodes(new WSNode[network_size]),
      _tmp_nodes(new WSNode[network_size]),
      _sink_idx(network_size-1),
      _network_size(network_size),
      _routing_table(new int[network_size*network_size]),
      _routes_len(new int[network_size]),
      _best_routing_table(new int[network_size*network_size]),
      _best_routes_len(new int[network_size]),
      _channel(channel),
      _nbr_iter(nbr_iter),
      _best_min_lifetime(-1),
      _nodes_costs(new double[network_size]),
      _predecessors(new int[network_size])
{
    dsfmt_init_gen_rand(&_rng, seed);
}



const int MAX_ATTEMPTS_GEN_NETWORK = 1000;

bool WSNetwork::generate_random_network(double x_size, double y_size, double battery,
					double msg_rate, double penalty)
{
    for(int node_idx=0; node_idx<_network_size; node_idx++) {
	int attempts = 0;
	do {
	    if(++attempts > MAX_ATTEMPTS_GEN_NETWORK)
		return false;
	    _nodes[node_idx].generate_random_node(0, 0, x_size/2, y_size/2, battery, msg_rate,
						  penalty, &_rng);
	} while(!this->is_good_node(node_idx));
        _tmp_nodes[node_idx] = _nodes[node_idx];
    }

#if 0
    /* Forcing sink node to be at least 100 m from any node*/
    _tmp_nodes[_network_size - 1].set_x(0);
    _tmp_nodes[_network_size - 1].set_y(-y_size/2 - 100);
    _nodes[_network_size - 1] = _tmp_nodes[_network_size - 1];
#endif

    return true;
}


bool WSNetwork::is_good_node(int node_idx) const
{
    for(int prev_node_idx=0; prev_node_idx<node_idx; prev_node_idx++) {
        WSNode* node = &_nodes[node_idx];
        double dx = node->get_x() - _nodes[prev_node_idx].get_x();
        double dy = node->get_y() - _nodes[prev_node_idx].get_y();
        if(dx*dx + dy*dy < MIN_SQR_DISTANCE)
            return false;
    }
    return true;
}


void WSNetwork::set_network(const double* xs, const double* ys, const double* batteries,
			    double msg_rate, double penalty_increment)
{
    for(int node_idx=0; node_idx<_network_size; node_idx++) {
	_nodes[node_idx].set_x(xs[node_idx]);
	_nodes[node_idx].set_y(ys[node_idx]);
	_nodes[node_idx].set_battery(batteries[node_idx]);
	_nodes[node_idx].set_msg_rate(msg_rate);
	_nodes[node_idx].set_penalty_increment(penalty_increment);

        _tmp_nodes[node_idx] = _nodes[node_idx];
    }
}



void WSNetwork::calculate_neighborhoods()
{
    for(int n1_idx=_network_size-2; n1_idx>=0; n1_idx--)
	this->set_neighbors(&_nodes[n1_idx]);
}


void WSNetwork::set_neighbors(WSNode* src_node)
{
    std::unique_ptr<int[]> neighbors(new int[_network_size]);

    int neighbor_idx = 0;
    for(int n1_idx=0; n1_idx<_network_size; n1_idx++) {
	WSNode* dst_node = &_nodes[n1_idx];
	if(_channel->is_within_range(src_node, dst_node))
	    neighbors[neighbor_idx++] = n1_idx;
    }

    src_node->set_neighbors(neighbors.get(), neighbor_idx);
}


bool WSNetwork::is_connected()
{
    int* route = _routing_table.get();
    int* p_route_len = _routes_len.get();
    for(int node_idx=_network_size-2; node_idx>=0; node_idx--)
        if(!_nodes[node_idx].get_nbr_neighbors() || !dijkstra(node_idx, route, p_route_len))
            return false;
    return true;
}


#include <iostream>

bool WSNetwork::optimize_maximum_lifetime()
{
    int counter = 0;
    int node_idx = 0;
    bool first_round = true;

    bool has_converged = false;
    this->reset_counters();
    do {
	this->find_best_route(first_round, node_idx);
	if(this->check_best()) {
	    std::cout << "On iteration " << counter << " Min lifetime " << _best_min_lifetime
		      << std::endl;
	}
	this->add_penalties();
	if(++node_idx >= _network_size-1) {
	    node_idx = 0;
	    first_round = false;
	    this->normalize_penalties();
	    has_converged = this->check_convergence();
	}
	counter++;
    } while(!has_converged && (counter < _nbr_iter));

    _routing_table.swap(_best_routing_table);
    _routes_len.swap(_best_routes_len);
    this->calculate_consumption();

    return counter < _nbr_iter;
}


void WSNetwork::reset_counters()
{
    for(int node_idx=0; node_idx<_network_size; node_idx++)
	_nodes[node_idx].reset_counters();
    _best_min_lifetime = -1;
}

void WSNetwork::redistribute_batteries(float unit_battery)
{
    double total_battery = 0, total_consumption = 0;
    for(int node_idx=_network_size - 2; node_idx>=0; node_idx--) {
	total_battery += _nodes[node_idx].get_battery();
	total_consumption += _nodes[node_idx].get_consumption();
    }
    double remaining_battery = total_battery;
    for(int node_idx=_network_size - 2; node_idx>=0; node_idx--) {
	int tmp = (int) (_nodes[node_idx].get_consumption()*total_battery/total_consumption);
	if (tmp < unit_battery)
	    tmp = unit_battery;
	_nodes[node_idx].set_battery(tmp);
	remaining_battery -= tmp;
    }

    int min_idx = -1;
    double min_lifetime = FLT_MAX;
    for(int node_idx=_network_size - 2; node_idx>=0; node_idx--)
	if(_nodes[node_idx].get_lifetime() < min_lifetime) {
	    min_idx = node_idx;
	    min_lifetime = _nodes[node_idx].get_lifetime();
	}
    _nodes[min_idx].set_battery(_nodes[min_idx].get_battery() + remaining_battery);

    for(int node_idx=_network_size - 2; node_idx>=0; node_idx--)
	_tmp_nodes[node_idx] = _nodes[node_idx];
}


void WSNetwork::find_best_route(bool first_round, int node_idx)
{
    double src_msg_rate = _nodes[node_idx].get_msg_rate();
    int* p_route = &_routing_table[node_idx*_network_size];
    int* p_route_len = &_routes_len[node_idx];

    if (!first_round)
	this->update_traffic_along_route(node_idx, p_route, *p_route_len, -src_msg_rate);
    assert(this->dijkstra(node_idx, p_route, p_route_len)); /* dijkstra shouldn't be 0 */
    this->update_traffic_along_route(node_idx, p_route, *p_route_len, src_msg_rate);
}


void WSNetwork::update_traffic_along_route(int src_idx, const int* route, int route_len,
					   double msg_rate)
{
    WSNode* src_node = &_nodes[src_idx];

    for(int route_idx = 0; route_idx < route_len; route_idx++) {
	WSNode* dst_node = &_nodes[route[route_idx]];
	double cost = _channel->get_link_outgoing_cost(src_node, dst_node, msg_rate);
	src_node->update_outgoing_counters(msg_rate, cost);
	cost = _channel->get_link_incoming_cost(dst_node, msg_rate);
	dst_node->update_incoming_counters(msg_rate, cost);
	src_node = dst_node;
    }
}


bool WSNetwork::check_best()
{
    double min_lifetime = FLT_MAX;
    bool rc = false;

    _tmp_nodes.swap(_nodes);
    this->calculate_consumption();

    for(int idx=_network_size-2; idx>=0; idx--)
	if(_nodes[idx].get_lifetime() < min_lifetime)
	    min_lifetime = _nodes[idx].get_lifetime();
    if(min_lifetime > _best_min_lifetime) {
	_best_min_lifetime = min_lifetime;
	for(int node_idx=_network_size-2; node_idx>=0; node_idx--) {
	    int* route = _routing_table.get() + node_idx*_network_size;
	    int* best_route = _best_routing_table.get() + node_idx*_network_size;
	    for(int route_idx=0; route_idx<_routes_len[node_idx]; route_idx++)
		*best_route++ = *route++;
	    _best_routes_len[node_idx] = _routes_len[node_idx];
	} 
	rc = true;
    }

    _tmp_nodes.swap(_nodes);
    return rc;
}


void WSNetwork::add_penalties()
{
    double min_lifetime = FLT_MAX;
    for(int node_idx=_network_size-2; node_idx>=0; node_idx--) {
	double lifetime = _nodes[node_idx].get_lifetime();
	if (lifetime < min_lifetime)
	    min_lifetime = lifetime;
    }
    for(int node_idx=_network_size-2; node_idx>=0; node_idx--) {
	double lifetime = _nodes[node_idx].get_lifetime();
	if (lifetime == min_lifetime)
	    _nodes[node_idx].update_penalty(_nodes[node_idx].get_penalty_increment());
    }
}


void WSNetwork::normalize_penalties()
{
    /* Check whether the minimum penalty is 0 or not.  If it is not,
       subtract the minimum from all penalties to make the minimum
       equal to 0.
     */
    double min_penalty = FLT_MAX;
    for(int node_idx=_network_size-2; node_idx>=0; node_idx--) {
        if(_nodes[node_idx].get_penalty() < min_penalty)
            min_penalty = _nodes[node_idx].get_penalty();
    }
    if ((min_penalty > 0) && (min_penalty < FLT_MAX))
        for(int node_idx=_network_size-2; node_idx>=0; node_idx--)
            _nodes[node_idx].update_penalty(-min_penalty);
}


bool WSNetwork::check_convergence()
{
    /* Find the minimum and maximum battery lifetime among all nodes
     * (except the sink).
     */
    double min_lifetime = FLT_MAX;
    double max_lifetime = -1;
    for(int node_idx=_network_size-2; node_idx>=0; node_idx--) {
	double lifetime = _nodes[node_idx].get_lifetime();
        if(lifetime > max_lifetime)
            max_lifetime = lifetime;
        if(lifetime < min_lifetime)
            min_lifetime = lifetime;
    }

    /* If the difference between them is small enough, return true,
       meaning the values have converged.
     */
    if(max_lifetime - min_lifetime < MIN_SPREAD)
        return true;

    return false;
}


bool WSNetwork::optimize_minimum_energy()
{
    for(int node_idx=0; node_idx<_network_size; node_idx++)
	_nodes[node_idx].reset_counters();

    _routes_len[_network_size-1] = 0;
    _routing_table[_network_size*(_network_size - 1)] = _network_size - 1;
    for(int node_idx=_network_size-2; node_idx>=0; node_idx--) {
	int* route = &_routing_table[node_idx*_network_size];
	int* p_route_len = &_routes_len[node_idx];
	double src_msg_rate = _nodes[node_idx].get_msg_rate();
	this->dijkstra(node_idx, route, p_route_len);
    }

    this->calculate_consumption();

    return true;
}


void WSNetwork::calculate_consumption()
{
    for(int node_idx=0; node_idx<_network_size; node_idx++)
	_nodes[node_idx].reset_counters();

    for(int node_idx=_network_size-2; node_idx>=0; node_idx--) {
	int* route = &_routing_table[node_idx*_network_size];
	int* p_route_len = &_routes_len[node_idx];
	double src_msg_rate = _nodes[node_idx].get_msg_rate();

	this->update_traffic_along_route(node_idx, route, *p_route_len, src_msg_rate);
    }
}


int* WSNetwork::dijkstra(int src_node_idx, int* route, int* route_len)
{
    int i, hops, node_idx;
    double src_msg_rate = _nodes[src_node_idx].get_msg_rate();
    assert(src_node_idx != _sink_idx);

    for(i = 0; i < _network_size; i++){
        _nodes_costs[i] = FLT_MAX;
        _predecessors[i] = -1;
    }
    _nodes_costs[src_node_idx] = 0.0;

    int best_idx = src_node_idx;
    while(best_idx != _sink_idx) {
	WSNode* src_node = &_nodes[best_idx];
	const int* nbs = _nodes[best_idx].get_neighbors();
        for(i = 0; i < _nodes[best_idx].get_nbr_neighbors(); i++) {
	    int nb_idx = *nbs++;
	    WSNode* dst_node = &_nodes[nb_idx];
	    double cost = _channel->get_link_cost(src_node, dst_node, src_msg_rate);
	    if(_nodes_costs[best_idx] + cost < _nodes_costs[nb_idx]) {
		_nodes_costs[nb_idx] = _nodes_costs[best_idx] + cost;
		_predecessors[nb_idx] = best_idx;
	    }
        }
        _nodes_costs[best_idx] = 0;

        double min = FLT_MAX;
        for(i = 0; i < _network_size; i++)
            if(0 < _nodes_costs[i] && _nodes_costs[i] < min) {
                min = _nodes_costs[i];
                best_idx = i;
            }
	if(min == FLT_MAX)
	    /* There is no path from the source node to the sink node */
	    return 0;
    }

    /* Now recover the best route  */
    route[0] = node_idx = _sink_idx;
    hops = 1;
    while(node_idx != src_node_idx)
        route[hops++] = node_idx = _predecessors[node_idx];
    /* Exclude the source node since it is known from the position of
       this route in the full routing table */
    hops--;

    *route_len = hops;
    node_idx = 0;
    while(node_idx < --hops) {
	int tmp = route[node_idx];
        route[node_idx++] = route[hops];
	route[hops] = tmp;
    }

    return route;
}
