// -*- c-basic-offset: 8; -*-
#ifndef WSNODE_HPP
#define WSNODE_HPP

#include <memory>
#include "dSFMT.h"

class WSNode {
public:
	WSNode();
        WSNode(const WSNode& src);
	WSNode(float x, float y, float battery_capacity, float msg_rate, float penalty);
        WSNode& operator=(const WSNode& src);

	double squared_distance_from(double x, double y) const;
	void generate_random_node(double x, double y, double dx, double dy, double battery,
				  double msg_rate, double penalty, dsfmt_t* rng);
	void reset_counters();

	double get_transmission_cost(const WSNode* dst) const;
	double get_reception_cost() const;
	double get_processing_cost() const;

	void set_neighbors(int* neighbors, int neighbors_size);
	const int* get_neighbors() const { return _neighbors.get(); }
	int get_nbr_neighbors() const { return _neighbors_size; }
	double get_msg_rate() const { return _msg_rate; }
	double get_penalty_increment() const { return _penalty_increment; }
	void update_penalty(double amount) { _penalty += amount; }
	double get_penalty() const { return _penalty; }
	void update_outgoing_counters(double msg_rate, double cost);
	void update_incoming_counters(double msg_rate, double cost);
	double get_outgoing_traffic() const { return _outgoing_traffic; }
	double get_incoming_traffic() const { return _incoming_traffic; }
	double get_consumption() const { return _consumption; }
	double get_lifetime() const { return _battery_capacity/_consumption; }
	double get_x() const { return _x; }
	double get_y() const { return _y; }
	void set_x(double x) { _x = x; }
	void set_y(double y) { _y = y; }

protected:
	std::unique_ptr<int[]> _neighbors;
        int _neighbors_size;

        double _x, _y;
	double _consumption;
	double _outgoing_traffic;
	double _incoming_traffic;
        double _battery_capacity;
        double _msg_rate;
	double _penalty;
	double _penalty_increment;
};

inline void WSNode::reset_counters()
{
	_consumption = _incoming_traffic = _outgoing_traffic = _penalty = 0;
}

inline void WSNode::update_outgoing_counters(double msg_rate, double cost)
{
	_outgoing_traffic += msg_rate;
	_consumption += cost;
}

inline void WSNode::update_incoming_counters(double msg_rate, double cost)
{
	_incoming_traffic += msg_rate;
	_consumption += msg_rate*this->get_reception_cost();
}

inline double WSNode::get_transmission_cost(const WSNode* dst) const
{
	/* 
	 * Transmitter working at 20 mA at 3.3 V gives 66 mW. At 200
	 * kbps (assuming a raw rate of 250 kbps and a 20% protocol
	 * overhead) gives 330 nJ/bit.
	 */
	// return 3.3e-7;
	// return 5e-12; // for a path loss of 2 and a radius of 100
	return 1e-10;
}

inline double WSNode::get_reception_cost() const
{
	/*
	 * Assuming 50 nJ/bit per the marvelous indian guys' paper.
	 */
	return 5e-8;
}

inline double WSNode::get_processing_cost() const
{
	/*
	 * Assuming 50 nJ/bit per the marvelous indian guys' paper.
	 */
	return 5e-8;
}

#endif // WSNODE_HPP
