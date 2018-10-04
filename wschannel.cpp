// -*- c-basic-offset: 4; -*-
#include <math.h>
#include "wschannel.hpp"

const double RADIO_RANGE_RADIUS = 100;  // radio range in meters

double WSChannel::get_link_cost(const WSNode* src, const WSNode* dst, double msg_rate) const
{
    return this->get_link_outgoing_cost(src, dst, msg_rate) +
	this->get_link_incoming_cost(dst, msg_rate) + dst->get_penalty();
}

double WSChannel::get_link_outgoing_cost(const WSNode* src, const WSNode* dst, double msg_rate) const
{
    double dx = src->get_x() - dst->get_x();
    double dy = src->get_y() - dst->get_y();
    double distance_factor = pow(dx*dx + dy*dy, _power_factor/2);
    return msg_rate*(src->get_processing_cost() +
		     distance_factor*src->get_transmission_cost(dst));
}

double WSChannel::get_link_incoming_cost(const WSNode* dst, double msg_rate) const
{
    return msg_rate*dst->get_reception_cost();
}

bool WSChannel::is_within_range(const WSNode* src, const WSNode* dst) const
{
    double dist = src->squared_distance_from(dst->get_x(), dst->get_y());
    if(dist > 0 && (_all_reachable || dist <= RADIO_RANGE_RADIUS*RADIO_RANGE_RADIUS))
	return true;
    return false;
}
