#ifndef HM_VMEM_H
#define HM_VMEM_H

#include <common/utils_include.h>
#include <common/utils_cfg.h>
#include <common/utils_mon.h>
#include <common/utils_stat.h>

#include <stdint.h>

#define MEMORY_SIZE (0x10000)

class vmem : public sc_core::sc_module
{
protected:
    SC_HAS_PROCESS(vmem);

    enum ChannelState
    {
        CLEAR,
        REQ,
        ACK
    };

    /* Backing store for memory. */
    uint8_t mem_data[MEMORY_SIZE];

    /* Incoming queues per channel. */
    std::deque<ARM::AXI4::Payload*> aw_queue;
    std::deque<ARM::AXI4::Payload*> w_queue;
    std::deque<ARM::AXI4::Payload*> ar_queue;

    /* Current state of each bw channel. */
    ChannelState b_state;
    ChannelState r_state;

    int cfg_outstand; // outstand number
    int cfg_freq;   // frequecy
    int cfg_bitw;   // bitwidth
    int cfg_buffer; // buffer size
    int cfg_delay;  // delay number

    std::deque<ARM::AXI4::Payload*> delay_queue_w;  // all command first be sent to delay queue, and then be act in memory.
    std::deque<ARM::AXI4::Payload*> delay_queue_r;  // all command first be sent to delay queue, and then be act in memory.

    std::deque<ARM::AXI4::Payload*> delayed_queue_w;
    std::deque<ARM::AXI4::Payload*> delayed_queue_r;

    int outstand_cnt; // stats all the command number in current memory
    int inst_id;

    /* Outgoing communications to send at negedge. */
    ARM::AXI4::Payload* b_outgoing;
    ARM::AXI4::Payload* r_outgoing;

    ARM::AXI4::Payload* aw_incoming;
    ARM::AXI4::Payload* ar_incoming;

    /* Number of beats (of r_outgoing) already sent. */
    unsigned r_beat_count;

    void clock_posedge();
    void clock_negedge();

    void read_config(string cfgname);

    tlm::tlm_sync_enum nb_transport_fw(ARM::AXI4::Payload& payload,
        ARM::AXI4::Phase& phase);

public:
    vmem(sc_core::sc_module_name name, string configname, int id);

    ARM::AXI4::SimpleSlaveSocket<vmem> slave;

    sc_core::sc_in<bool> clock;
    //sc_core::sc_clock   clock;
};

#endif // HM_VMEM_H

