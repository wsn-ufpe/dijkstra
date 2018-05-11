// -*- c-basic-offset: 4; -*-
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <time.h>
#include <dSFMT.h>
#include <jsoncpp/json/writer.h>
#include <tclap/CmdLine.h>
#include "wsnetwork.hpp"
#include "wschannel.hpp"


using std::string;

void output_data(const WSNetwork* network, const char filename[]);

const int DEF_NBR_NODES = 100;
const int DEF_NBR_ITER = 50000;
const int DEF_SEED = 654321;
const float DEF_PATH_LOSS = 2;
const float DEF_FIELD_SIZE = 500;
const float DEF_BATTERY = 5;
const float DEF_RATE = 100;
const float DEF_PENALTY = 1e-7;

int main(int argc, char* argv[])
{
    TCLAP::CmdLine cmd("A program to find a set of routes to maximize the lifetime of a "
		       "wireless sensor network");

    TCLAP::ValueArg<int> nbr_nodes_arg("n", "nbrnodes", "Number of nodes in the network", false,
                                       DEF_NBR_NODES, "int", cmd);
    TCLAP::ValueArg<int> nbr_iter_arg("I", "nbriter", "Number of iterations in the search for "
		   		      "the maximum lifetime", false, DEF_NBR_ITER, "int", cmd);
    TCLAP::ValueArg<int> seed_arg("S", "seed", "Seed for the rng", false, DEF_SEED,
				    "int", cmd);
    TCLAP::SwitchArg use_fixed_seed("s","fixed_seed","Use a fixed seed (given by the seed arg "
                                    "(-S)", cmd, false);
    TCLAP::ValueArg<string> output_file_arg("o", "output", "Suffix for the output files", false,
					"", "string", cmd);
    TCLAP::ValueArg<float> x_dim_arg("x", "xdim", "Rectangular field X size in meters", false,
                                         DEF_FIELD_SIZE, "float", cmd);
    TCLAP::ValueArg<float> y_dim_arg("y", "ydim", "Rectangular field Y size in meters", false,
                                         DEF_FIELD_SIZE, "float", cmd);
    TCLAP::ValueArg<float> path_loss_arg("p", "pathloss", "Path loss exponent", false,
                                         DEF_PATH_LOSS, "float", cmd);
    TCLAP::ValueArg<float> battery_arg("b", "battery", "Battery capacity", false,
                                         DEF_BATTERY, "float", cmd);
    TCLAP::ValueArg<float> msg_rate_arg("r", "msgrate", "Message rate in bits per time unit", false,
                                         DEF_RATE, "float", cmd);
    TCLAP::ValueArg<float> penalty_arg("P", "penalty", "Penalty value", false,
                                         DEF_PENALTY, "float", cmd);

    cmd.parse( argc, argv );

    int seed = use_fixed_seed.getValue() ? seed_arg.getValue() : time(0);
    WSChannel dummy(path_loss_arg.getValue());
    WSNetwork network(nbr_nodes_arg.getValue(), &dummy, nbr_iter_arg.getValue(), seed);

    const int MAX_TRIES = 10000;
    int tries = 0;
    do {
        if(!network.generate_random_network(x_dim_arg.getValue(), y_dim_arg.getValue(),
					    battery_arg.getValue(), msg_rate_arg.getValue(),
					    penalty_arg.getValue())) {
            std::cerr << "Unable to generate a sufficiently sparse network!" << std::endl;
            std::cerr << "The number of nodes is probably too large.  Aborting..." << std::endl;
            return 1;
        }
	network.calculate_neighborhoods();
    } while(tries++ < MAX_TRIES && !network.is_connected());

    if(tries >= MAX_TRIES) {
	std::cerr << "The network is partitioned into two or more disconnected " << std::endl
		  << "subnetworks.  Aborting..." << std::endl;
	return 2;
    }

    network.optimize_minimum_energy();
    output_data(&network, ("min_energy_" + output_file_arg.getValue() + ".dat").c_str());

    if(!network.optimize_maximum_lifetime()){
            std::cerr << "Optmization for lifetime stopped due to the "
		"number of iterations." << std::endl;
        }
    output_data(&network, ("max_lifetime_" + output_file_arg.getValue() + ".dat").c_str());

    return 0;
}


void output_data(const WSNetwork* network, const char filename[])
{
    Json::Value network_value;
    Json::Value xs(Json::arrayValue);
    Json::Value ys(Json::arrayValue);
    Json::Value neighborhood_list(Json::arrayValue);
    Json::Value incoming_traffic(Json::arrayValue);
    Json::Value outgoing_traffic(Json::arrayValue);
    Json::Value consumption(Json::arrayValue);
    Json::Value lifetime(Json::arrayValue);
    Json::Value routes(Json::arrayValue);
    std::unique_ptr<Json::Value[]> routes_values(new Json::Value[network->get_network_size()]);

    const WSNode* node = network->get_nodes();
    const int* routes_len = network->get_routes_len();
    for(int idx=0; idx<network->get_network_size(); idx++, node++) {
	xs.append(Json::Value(node->get_x()));
	ys.append(Json::Value(node->get_y()));
	outgoing_traffic.append(Json::Value(node->get_outgoing_traffic()));
	incoming_traffic.append(Json::Value(node->get_incoming_traffic()));
	consumption.append(Json::Value(node->get_consumption()));
	lifetime.append(Json::Value(node->get_lifetime()));

	const int* route_vec = network->get_routing_table() +
	    idx*network->get_network_size();
	for(int route_idx=0; route_idx<routes_len[idx]; route_idx++)
	    routes_values[idx].append(*route_vec++);
	routes.append(routes_values[idx]);
    }
    network_value["xs"] = xs;
    network_value["ys"] = ys;
    network_value["outgoing_traffic"] = outgoing_traffic;
    network_value["incoming_traffic"] = incoming_traffic;
    network_value["consumption"] = consumption;
    network_value["lifetime"] = lifetime;
    network_value["routes"] = routes;

    std::ofstream data_file(filename);
    data_file << network_value << std::endl;
}
