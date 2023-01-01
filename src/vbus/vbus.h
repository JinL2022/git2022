#ifndef HM_VBUS_H
#define HM_VBUS_H

#include <common/utils_include.h>
#include <common/utils_cfg.h>
#include <common/utils_mon.h>
#include <common/utils_stat.h>


/*    

*/
class vbus : public sc_core::sc_module
{
protected:
    SC_HAS_PROCESS(vbus);

    enum ChannelState
    {
        CLEAR,
        REQ,
        ACK
    };

    /* Outgoing queues per channel. */
    std::deque<ARM::AXI4::Payload*> aw_queue[4];  
    std::deque<ARM::AXI4::Payload*> w_queue [4];
    std::deque<ARM::AXI4::Payload*> ar_queue[4];

    std::deque<ARM::AXI4::Payload*> sw_aw_queue[4][4];
    std::deque<ARM::AXI4::Payload*> sw_w_queue [4][4];
    std::deque<ARM::AXI4::Payload*> sw_ar_queue[4][4];

    std::deque<ARM::AXI4::Payload*> sw_aw_h_queue[4];
    std::deque<ARM::AXI4::Payload*> sw_w_h_queue[4];
    std::deque<ARM::AXI4::Payload*> sw_ar_h_queue[4];
  
    std::deque<ARM::AXI4::Payload*> s_aw_queue[4], s_w_queue[4], s_ar_queue[4];
    std::deque<ARM::AXI4::Payload*> s_r_queue[4], s_b_queue[4];
    std::deque<ARM::AXI4::Payload*> r_queue[4], b_queue[4];
    //std::deque<ARM::AXI4::Payload*> wack_queue;
    //std::deque<ARM::AXI4::Payload*> rack_queue;

    /* */
    //std::deque<int> aw_fwd_deque;
    int cfg_outstand;
    int cfg_buffer;
    int cfg_queue;
    int sch_mode;  // Only CPU request is high priority
    int sch_rslt_ptr[2][4];  // 0: for requst sch, 1 for response sch;

    int outstand_cnt[4]; // In VBUS module, the max number for command;
    int buffer_cnt[4];  // Write data buffer is share memory;
    int w_beat_count[4];
    int r_beat_count[4];
    /* logfile */
    ofstream log_master;

    /* Current state of each fw channel. */
    ChannelState aw_state[4];
    ChannelState  w_state[4];
    ChannelState ar_state[4];

    ChannelState b_state[4];
    ChannelState r_state[4];

    /* Incoming communications to push at posedge. */
    ARM::AXI4::Payload* aw_incoming[4];
    ARM::AXI4::Payload* ar_incoming[4];

    ARM::AXI4::Payload* b_outgoing[4];
    ARM::AXI4::Payload* r_outgoing[4];
   
    //ARM::AXI4::Payload* rack_incoming;
    void clock_posedge();
    void clock_negedge();

    void switch_req();
    void switch_resp();

    int rr_sch(int last_ptr, bool val_0, bool val_1, bool val_2, bool val_3);

    tlm::tlm_sync_enum nb_transport_fwa(ARM::AXI4::Payload& payload,
        ARM::AXI4::Phase& phase);
    tlm::tlm_sync_enum nb_transport_fwb(ARM::AXI4::Payload& payload,
        ARM::AXI4::Phase& phase);
    tlm::tlm_sync_enum nb_transport_fwc(ARM::AXI4::Payload& payload,
        ARM::AXI4::Phase& phase);
    tlm::tlm_sync_enum nb_transport_fwd(ARM::AXI4::Payload& payload,
        ARM::AXI4::Phase& phase);

    tlm::tlm_sync_enum nb_transport_bwa(ARM::AXI4::Payload& payload,
        ARM::AXI4::Phase& phase);    
    tlm::tlm_sync_enum nb_transport_bwb(ARM::AXI4::Payload& payload,
        ARM::AXI4::Phase& phase);
    tlm::tlm_sync_enum nb_transport_bwc(ARM::AXI4::Payload& payload,
        ARM::AXI4::Phase& phase);
    tlm::tlm_sync_enum nb_transport_bwd(ARM::AXI4::Payload& payload,
        ARM::AXI4::Phase& phase);

public:
    vbus(sc_core::sc_module_name name, unsigned port_width, string cfgfile);
    ~vbus();

    ARM::AXI4::SimpleSlaveSocket<vbus> slave_a;
    ARM::AXI4::SimpleSlaveSocket<vbus> slave_b;
    ARM::AXI4::SimpleSlaveSocket<vbus> slave_c;
    ARM::AXI4::SimpleSlaveSocket<vbus> slave_d;

    ARM::AXI4::SimpleMasterSocket<vbus> master_a;
    ARM::AXI4::SimpleMasterSocket<vbus> master_b;
    ARM::AXI4::SimpleMasterSocket<vbus> master_c;
    ARM::AXI4::SimpleMasterSocket<vbus> master_d;
    sc_core::sc_in<bool> clock;

    void read_config(string cfgname);

    //stat
    st_depth* sta_aw_que[4];    
    st_depth* sta_w_que[4];
    st_depth* sta_ar_que[4];

    void st_print();
};

#endif // HM_VBUS_H
