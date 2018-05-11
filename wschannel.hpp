// -*- c-basic-offset: 8; -*-
#ifndef WSCHANNEL_HPP
#define WSCHANNEL_HPP

#include "wsnode.hpp"

class WSChannel {
public:
	WSChannel(double power_factor) : _power_factor(power_factor) {}
	double get_link_cost(const WSNode* src, const WSNode* dst, double msg_rate) const;
	double get_link_outgoing_cost(const WSNode* src, const WSNode* dst, double msg_rate) const;
	double get_link_incoming_cost(const WSNode* dst, double msg_rate) const;
	bool is_within_range(const WSNode* src, const WSNode* dst) const;

protected:
	double _power_factor;
};

#endif // WSCHANNEL_HPP
