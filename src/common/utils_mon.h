#ifndef HM_UTILS_MON_H
#define HM_UTILS_MON_H

#include "utils_include.h"
#include "utils_stat.h"
#include <ARM/TLM/arm_axi4.h>

using namespace std;
using namespace tlm;
using namespace sc_core;
using namespace ARM::AXI4;

class utils_mon : public sc_core::sc_module
{
protected:
    SC_HAS_PROCESS(utils_mon);

    /* Beat-sized data for data printing. */
    uint8_t* beat_data;

    /* logfile */
    ofstream log_master;
    ofstream log_slave;

    ofstream log_slave_sta;

    /* Map of burst counts for observed Payloads. */
    std::map<ARM::AXI4::Payload*, unsigned> payload_burst_index;

    tlm::tlm_sync_enum nb_transport_fw(ARM::AXI4::Payload& payload,
        ARM::AXI4::Phase& phase);
    tlm::tlm_sync_enum nb_transport_bw(ARM::AXI4::Payload& payload,
        ARM::AXI4::Phase& phase);

    void print_payload(ARM::AXI4::Payload& payload,
        ARM::AXI4::Phase sent_phase, tlm::tlm_sync_enum reply,
        ARM::AXI4::Phase reply_phase, bool direc);

public:
    utils_mon(sc_core::sc_module_name name, unsigned port_width = 128);
    ~utils_mon();

    void st_print();

    ARM::AXI4::SimpleSlaveSocket<utils_mon> slave;
    ARM::AXI4::SimpleMasterSocket<utils_mon> master;

    st_delay* delay_sta;
    st_bw* cmd_bw_sta; //read&write cmd stat
    st_bw* wr_bw_sta;  //write data occupation stat
    st_bw* rd_bw_sta;  //read  data occupation stat

    //st_depth cmd

};

#endif // HM_UTILS_MON_H
