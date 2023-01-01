#pragma once
#ifndef HM_UTILS_STAT_H
#define HM_UTILS_STAT_H

#include "utils_include.h"

using namespace std;
using namespace boost::property_tree;


// Base Class for Statis
class st_base {

public:
	long total_acc;  //总体累积量, 例如：延迟、收发的字节数、缓存占用&时间
	long phase_acc;  //当前Phase统计的累积量

	int total_jitter;  //jitter 总值
	int phase_jitter;  //jitter 阶段总值

	int total_cnt;   //收到的统计请求个数, 可用于统计平均包长、平均延迟等逐包或者逐命令的信息
	int phase_cnt;   //当前Phase统计的请求数

	bool is_on;
	string name;    //统计头定义
	string head_per_line;
	string head_per_phase;

	long total_max;     //最大值
	long total_min;     //最小值
	double total_avg;	  //平均值

	long phase_max;     //最大值
	long phase_min;     //最小值
	double phase_avg;	  //平均值

	int first_ph_stmp;  //第一次phase report时间戳, 单位为: ns
	int last_ph_stmp;   //记录中,上一次phase report时间戳
	int last_stmp;      //记录中,上一次统计请求的时间戳

	sc_time cur_stmp;

	ostringstream stream;

public:
	//
	st_base(string sta_name){
		//is_on = 0;
		is_on = 1;
		total_cnt = 0;
		phase_cnt = 0;
		total_acc = 0;
		phase_acc = 0;
		total_jitter = 0;
		phase_jitter = 0;

		name = sta_name;
		head_per_line = " ";
		head_per_phase = " ";

		total_max = 0;
		total_min = 10000;  // need to set a big num
		total_avg = 0;
		phase_max = 0;
		phase_min = 10000;
		phase_avg = 0;
	};

	//
	~st_base() {

	};
	
	void base_cnt(int var = 1) {
		cur_stmp = sc_core::sc_time_stamp();
		if (is_on) {
			phase_cnt++;
			total_cnt++;
			//last_stmp = cur_stmp.to_double() / 1000; //ns, sc_time.to_double default unit is ps;
		}
	};

	void base_report_phase() {

		cur_stmp = sc_core::sc_time_stamp();

		if (is_on == 0) {
			is_on == 1;

			first_ph_stmp = cur_stmp.to_double()/1000; //ns, sc_time.to_double default unit is ps;
			last_ph_stmp = cur_stmp.to_double() / 1000;
			last_stmp = last_ph_stmp;
		}
		else {
			//last_ph_stmp = cur_stmp.to_double()/1000;
			//输出统计信息字符串			
		}
	};

	// bool (string filename);
};

/* Class for Statics of Delay per command or per packet */
class st_delay : public st_base {

public:

	ostringstream phase_head;
	ostringstream line_head;


public:
	st_delay(string sta_name):st_base(sta_name) {

		//head
		phase_head <<endl<< "----  sta delay ----" << " : " << " phase :max/min/avg       " << "  total: max/min/avg " << endl;
	};


	~st_delay() {
		//report_total(); 释放最后一次统计结果, 预留, 后续实现;
	};

	void sta(int delay = 0) {
		base_cnt(delay);

		if (is_on) {
			if (delay > total_max) {
				total_max = delay;
			}
			if (delay < total_min) {
				total_min = delay;
			}
			total_acc += delay;

			if (delay > phase_max) {
				phase_max = delay;
			}
			if (delay < phase_min) {
				phase_min = delay;
			}
			phase_acc += delay;
		}
	};

	string report() {
		base_report_phase();

		line_head.str("");
		if ((is_on)&&(phase_cnt>0)) {
			//cur_stmp = sc_;
			phase_avg = phase_acc / phase_cnt;
			total_avg = total_acc / total_cnt;
		}
		line_head << phase_head.str();
		line_head <<setw(10)<<sc_time_stamp() << setw(10) << name << " : " << setw(8) << phase_max << setw(8) << phase_min << setw(8) << phase_avg << "   "
		                                   <<setw(8) << total_max << setw(8) << total_min << setw(8) << total_avg << endl;

		if (is_on) {
			phase_acc = 0;
			phase_cnt = 0;
			phase_max = 0;
			phase_min = 10000;
			phase_avg = 0;
		}

		return line_head.str();
	};
	// bool (string filename);
};

/* Class for Statics of Bandwith */
class st_bw : public st_base {

public:

	ostringstream phase_head;
	ostringstream line_head;

	int phase_size;
	int total_size;


public:
	st_bw(string sta_name) :st_base(sta_name) {

		phase_size = 0;
		total_size = 0;
		//head
		phase_head << endl << "---- sta bw(Gbps) ----" << " : " << " phase :bps/pps       " << "  total: bps/pps " << endl;
	};


	~st_bw() {
		//report_total(); 释放最后一次统计结果, 预留, 后续实现;
	};

	void sta(int delay = 1) {
		base_cnt(delay);

		if (is_on) {		
			total_acc += delay;		
			phase_acc += delay;
		}
	};

	string report() {
		base_report_phase();
		line_head.str("");

		phase_avg = phase_acc / (cur_stmp.to_double() / 1000 - last_ph_stmp);
		total_avg = total_acc / (cur_stmp.to_double() / 1000 - first_ph_stmp);
		line_head << setw(20) << name << " : " 
			<< setw(12) << setiosflags(ios::fixed) << setprecision(3) << phase_avg
			<< setw(8) << phase_cnt<<"   "
			<< setw(12) << setiosflags(ios::fixed) << setprecision(3) << total_avg
			<< setw(8) << total_cnt << endl;

		if (is_on) {
			phase_acc = 0;
			phase_cnt = 0;	
			last_ph_stmp = cur_stmp.to_double()/1000;
		}

		return line_head.str();
	};
	// bool (string filename);
};

/* Class for Statics of Bandwith */
class st_depth : public st_base {

public:

	ostringstream phase_head;
	ostringstream line_head;

	int phase_size;
	int req_int_size;
	int last_depth;


public:
	st_depth(string sta_name) :st_base(sta_name) {

		//head
		req_int_size = 0;//记录每两次统计之间的间隔
		phase_size = 0;
		last_depth = 0;
		phase_head <<endl<< "---- sta depth  ----" << " : " << " phase :max/min/avg       " << "  total: max/min/avg " << endl;
	};


	~st_depth() {
		//report_total(); 释放最后一次统计结果, 预留, 后续实现;
	};

	void sta(int depth = 0) {
		base_cnt(depth);

		req_int_size = cur_stmp.to_double() / 1000 - last_stmp;

		if (is_on) {
			if (depth > total_max) {
				total_max = depth;
			}
			if (depth < total_min) {
				total_min = depth;
			}
			total_acc += last_depth*req_int_size;

			if (depth > phase_max) {
				phase_max = depth;
			}
			if (depth < phase_min) {
				phase_min = depth;
			}
			phase_acc += last_depth*req_int_size;
		}

		last_stmp = cur_stmp.to_double() / 1000;
		last_depth = depth;
		req_int_size = 0;
	};

	string report() {
		base_report_phase();

		if (is_on) {
			//cur_stmp = sc_;
			phase_avg = phase_acc / (cur_stmp.to_double()/1000 - last_ph_stmp);
			total_avg = total_acc / (cur_stmp.to_double()/1000 - first_ph_stmp);
		}
		line_head.str("");

		phase_size = cur_stmp.to_double() / 1000 - last_ph_stmp;
		line_head << setw(20) << name << " : "
			<< setw(8) << phase_max <<" "
			<< setw(8) << phase_min << " "
			<< setw(8) << setiosflags(ios::fixed) << setprecision(3) << phase_avg << "   "
			<< setw(8) << total_max << " "
			<< setw(8) << total_min << " "
			<< setw(8) << setiosflags(ios::fixed) << setprecision(3) << total_avg << endl;

		if (is_on) {
			phase_acc = 0;
			phase_cnt = 0;
			phase_max = 0;
			phase_min = 0;
			last_ph_stmp = cur_stmp.to_double() / 1000;
		}

		return line_head.str();
	};
	// bool (string filename);
};

#endif