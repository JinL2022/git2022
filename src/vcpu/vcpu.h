#ifndef HM_VCPU_H
#define HM_VCPU_H

#include <common/utils_include.h>
#include <common/utils_cfg.h>
#include <common/utils_mon.h>
#include <common/utils_stat.h>

class vcpu : public sc_core::sc_module
{
protected:
    SC_HAS_PROCESS(vcpu);

    enum ChannelState
    {
        CLEAR,
        REQ,
        ACK
    };

    /* Outgoing queues per channel. */
    std::deque<ARM::AXI4::Payload*> aw_queue;
    std::deque<ARM::AXI4::Payload*> w_queue;
    std::deque<ARM::AXI4::Payload*> ar_queue;
    std::deque<ARM::AXI4::Payload*> cr_queue;
    std::deque<ARM::AXI4::Payload*> wack_queue;
    std::deque<ARM::AXI4::Payload*> rack_queue;

    /* Command Store queues */
    std::deque<ARM::AXI4::Payload*> wait_queue;
    sc_time sys_time_base;
    string  configname;

    //std::deque<int> gen_round_queue;

    /* Current state of each fw channel. */
    ChannelState aw_state;
    ChannelState w_state;
    ChannelState ar_state;
    ChannelState cr_state;

    /* Incoming communications to push at posedge. */
    ARM::AXI4::Payload* cr_incoming;
    ARM::AXI4::Payload* wack_incoming;
    ARM::AXI4::Payload* rack_incoming;

    /* Number of beats (of the front Payload of w_queue) already sent. */
    int w_beat_count;
    int cfg_id;
    int cfg_qos;
    int cfg_burst;

    void clock_posedge();
    void clock_negedge();

    void send_req();
    void read_config();

    sc_time_unit ts_fix(string instring);
    ARM::AXI4::Command req_fix(string cmdstring);
    //ARM::AXI4::

    tlm::tlm_sync_enum nb_transport_bw(ARM::AXI4::Payload& payload,
        ARM::AXI4::Phase& phase);

public:
    vcpu(sc_core::sc_module_name name, std::string config_name, int gid = 0);

    /* Add a payload to the traffic queue. */
    void add_payload(ARM::AXI4::Command command, uint64_t address,
        ARM::AXI4::Size size, uint8_t len, sc_time inc_time);

    /* Create the defined stimulus */
    //void gen_cmd();

    /* Read the Configure from XML file */
    //void read_config();

    ARM::AXI4::SimpleMasterSocket<vcpu> master;

    sc_core::sc_in<bool> clock;
};

#endif // HM_VCPU_H
