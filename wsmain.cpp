// -*- c-basic-offset: 4; -*-
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <time.h>
#include <dSFMT.h>
#include <jsoncpp/json/json.h>
#include <tclap/CmdLine.h>
#include "wsnetwork.hpp"
#include "wschannel.hpp"


using std::string;

void output_data(const WSNetwork* network, const char filename[]);

const int DEF_NBR_NODES = 100;
const int DEF_NBR_ITER = 50000;
const int DEF_SEED = 654321;
const double DEF_PATH_LOSS = 2;
const double DEF_FIELD_SIZE = 500;
const double DEF_BATTERY = 6;
const double DEF_RATE = 100;
const double DEF_PENALTY = 1e-7;
const double DEF_UNIT_BATTERY = 1.0;

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
    TCLAP::ValueArg<double> x_dim_arg("x", "xdim", "Rectangular field X size in meters", false,
                                         DEF_FIELD_SIZE, "double", cmd);
    TCLAP::ValueArg<double> y_dim_arg("y", "ydim", "Rectangular field Y size in meters", false,
                                         DEF_FIELD_SIZE, "double", cmd);
    TCLAP::ValueArg<double> path_loss_arg("p", "pathloss", "Path loss exponent", false,
                                         DEF_PATH_LOSS, "double", cmd);
    TCLAP::ValueArg<double> battery_arg("b", "battery", "Battery capacity", false,
                                         DEF_BATTERY, "double", cmd);
    TCLAP::ValueArg<double> msg_rate_arg("r", "msgrate", "Message rate in bits per time unit", false,
                                         DEF_RATE, "double", cmd);
    TCLAP::ValueArg<double> penalty_arg("P", "penalty", "Penalty value", false,
                                         DEF_PENALTY, "double", cmd);
    TCLAP::ValueArg<string> network_arg("N", "network", "File with network to be used", false,
					"", "string", cmd);
    TCLAP::SwitchArg all_reachable("a","all_reachable","Consider all nodes reachable",
				   cmd, false);

    cmd.parse( argc, argv );

    int seed = use_fixed_seed.getValue() ? seed_arg.getValue() : time(0);
    WSChannel dummy(path_loss_arg.getValue(), all_reachable.getValue());

    int nbr_nodes = 0;
    std::unique_ptr<double[]> xs;
    std::unique_ptr<double[]> ys;
    std::unique_ptr<double[]> batteries;
    if(network_arg.getValue() != "") {
	std::ifstream ifs(network_arg.getValue());
	Json::Reader reader;
	Json::Value root;
	reader.parse(ifs, root);
	Json::Value xs_json = root["xs"];
	xs.reset(new double[xs_json.size()]);
	for(int i=0; i<xs_json.size(); i++)
	    xs[i] = xs_json[i].asDouble();
	Json::Value ys_json = root["ys"];
	ys.reset(new double[ys_json.size()]);
	for(int i=0; i<ys_json.size(); i++)
	    ys[i] = ys_json[i].asDouble();
	Json::Value batteries_json = root["batteries"];
	batteries.reset(new double[batteries_json.size()]);
	for(int i=0; i<batteries_json.size(); i++)
	    batteries[i] = batteries_json[i].asDouble();

	if((xs_json.size() != ys_json.size()) || (xs_json.size() != batteries_json.size())) {
	    std::cerr << "Inconsistencies on the network declaration in file "
		      << argv[0] << "." << std::endl;
	    return -3;
	}
	nbr_nodes = xs_json.size();
    } else {
	nbr_nodes = nbr_nodes_arg.getValue();
    }

    WSNetwork network(nbr_nodes, &dummy, nbr_iter_arg.getValue(), seed);

    if(network_arg.getValue() == "") {
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
   } else {
	network.set_network(xs.get(), ys.get(), batteries.get(), msg_rate_arg.getValue(),
			    penalty_arg.getValue());
	network.calculate_neighborhoods();
	if (!network.is_connected()) {
	    std::cerr << "Network passed in file " << network_arg.getValue()
		      << " is not connected!  Aborting..." << std::endl;
	    return 2;
	}
    }

    //*************
    const WSNode* nodes = network.get_nodes();
    const char DASH[] = "     -      ";
    std::cout << DASH;
    for(int i=0; i<network.get_network_size() - 1; i++)
	std::cout << " " << dummy.get_link_cost(nodes + network.get_network_size() - 1, nodes + i,
						msg_rate_arg.getValue());
    std::cout << std::endl;
    for(int i=0; i<network.get_network_size() - 1; i++) {
	std::cout << dummy.get_link_cost(nodes + network.get_network_size() - 1, nodes + i,
					 msg_rate_arg.getValue());
	for(int j=0; j<network.get_network_size() - 1; j++) {
	    std::cout << " ";
	    if(i == j)
		std::cout << DASH;
	    else
		std::cout << dummy.get_link_cost(nodes + i, nodes + j, msg_rate_arg.getValue());
	}
	std::cout << std::endl;
    }
    //*************

    network.optimize_minimum_energy();
    output_data(&network, ("min_energy_" + output_file_arg.getValue() + ".dat").c_str());

    network.optimize_maximum_lifetime();
    output_data(&network, ("max_lifetime_" + output_file_arg.getValue() + "_pre.dat").c_str());
    network.redistribute_batteries(DEF_UNIT_BATTERY);
    if(network.optimize_maximum_lifetime())
	std::cerr << "Optmization for lifetime converged." << std::endl;
    output_data(&network, ("max_lifetime_" + output_file_arg.getValue() + "_pos.dat").c_str());

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
    Json::Value batteries(Json::arrayValue);
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
	batteries.append(Json::Value(node->get_battery()));

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
    network_value["batteries"] = batteries;
    network_value["lifetime"] = lifetime;
    network_value["routes"] = routes;

    std::ofstream data_file(filename);
    data_file << network_value << std::endl;
}
