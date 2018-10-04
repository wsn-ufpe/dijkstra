// -*- c-basic-offset: 4; -*-
#include <iostream>
#include <float.h>
#include "wsnode.hpp"
#include "dSFMT.h"

const int MAX_ATTEMPTS_GEN_NETWORK = 1000;

WSNode::WSNode()
    : _neighbors(0), _neighbors_size(0), _x(0), _y(0), _consumption(0),
      _outgoing_traffic(0), _incoming_traffic(0),
      _battery_capacity(0), _msg_rate(0), _penalty(0), _penalty_increment(0)
{
}


WSNode::WSNode(const WSNode& src)
    : _neighbors(0), _neighbors_size(src._neighbors_size), _x(src._x), _y(src._y),
      _consumption(0), _outgoing_traffic(0), _incoming_traffic(0),
      _battery_capacity(src._battery_capacity), _msg_rate(src._msg_rate),
      _penalty(0), _penalty_increment(0)
{
    if(_neighbors_size)
	_neighbors.reset(new int[_neighbors_size]);
}


WSNode::WSNode(double x, double y, double battery_capacity, double msg_rate, double penalty)
    : _neighbors(0), _neighbors_size(0), _x(x), _y(y), _consumption(0),
      _outgoing_traffic(0), _incoming_traffic(0),
      _battery_capacity(battery_capacity), _msg_rate(msg_rate),
      _penalty(0), _penalty_increment(penalty)
{
}


WSNode& WSNode::operator=(const WSNode& src)
{
    if(_neighbors_size)
	_neighbors.reset(new int[_neighbors_size]);
    else
        _neighbors.reset(0);
    _neighbors_size = src._neighbors_size;
    _x = src._x;
    _y = src._y;
    _battery_capacity = src._battery_capacity;
    _msg_rate = src._msg_rate;
    _penalty_increment = src._penalty_increment;
}


void WSNode::generate_random_node(double x, double y, double dx, double dy, double battery,
				  double msg_rate, double penalty, dsfmt_t* rng)
{
    _x = x + (2*dsfmt_genrand_close_open(rng) - 1)*dx;
    _y = y + (2*dsfmt_genrand_close_open(rng) - 1)*dy;
    _battery_capacity = battery;
    _msg_rate = msg_rate;
    _penalty_increment = penalty;
}


double WSNode::squared_distance_from(double x, double y) const
{
    double dx = _x - x;
    double dy = _y - y;
    return dx*dx + dy*dy;
}


void WSNode::set_neighbors(int* neighbors, int neighbors_size)
{
    _neighbors_size = neighbors_size;
    if(neighbors_size > 0)
	_neighbors.reset(new int[neighbors_size]);
    else
	_neighbors.reset(0);
     while(neighbors_size-- > 0)
         _neighbors[neighbors_size] = neighbors[neighbors_size];
}
