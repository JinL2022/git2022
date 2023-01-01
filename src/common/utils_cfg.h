#pragma once
#ifndef HM_UTILS_CFG_H
#define HM_UTILS_CFG_H

#include "utils_include.h"

using namespace std;
using namespace boost::property_tree;


class utils_config {

public:
	int total_time;
	int phase_time;

	sc_time_unit sim_unit;

	std::deque<int> gen0;
	std::deque<int> gen1;


public:
	utils_config(string filename);
	~utils_config();


	bool read_config(string filename);
};

#endif // HM_UTILS_CFG_H
