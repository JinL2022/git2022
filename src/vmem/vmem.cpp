#include <cstring>

#include "vmem.h"

using namespace std;
using namespace tlm;
using namespace sc_core;
using namespace ARM::AXI4;

void vmem::clock_posedge() //internal process
{
    if (b_state == ACK)
        b_state = CLEAR;

    if (r_state == ACK)
        r_state = CLEAR;

    if (delay_queue_r.size() > cfg_delay)
    {
        //virtual memory read process, all operation in one cycle
        if (!delay_queue_r.front()) {
            delay_queue_r.pop_front();
        }
        else {
            delayed_queue_r.push_back(delay_queue_r.front());
            delay_queue_r.pop_front();           
        }
    }

    /* Is there a read still to perform? */
    if (!r_outgoing && !delayed_queue_r.empty())
    {
        //virtual memory read process, all operation in one cycle     
        r_outgoing = delayed_queue_r.front();
        delayed_queue_r.pop_front();

        r_beat_count = r_outgoing->get_beat_count();

        /* Populate read data in one go. */
        uint64_t addr = r_outgoing->get_base_address() & 0xffff;
        sc_assert(addr + r_outgoing->get_data_length() <= MEMORY_SIZE);
        r_outgoing->read_in(&mem_data[addr]);          
    }

    /* Read delay queue */
     //Payload* payload = Payload::new_payload(command, address, size, len, burst);
   
    if (!ar_queue.empty()) {
        delay_queue_r.push_back(ar_queue.front());
        ar_queue.pop_front();
    }
    else {        
        delay_queue_r.push_back(NULL);
    }

    if (delay_queue_w.size() > cfg_delay)
    {
        //virtual memory read process, all operation in one cycle
        if (!delay_queue_w.front()) {
            delay_queue_w.pop_front();
        }
        else {
            delayed_queue_w.push_back(delay_queue_w.front());
            delay_queue_w.pop_front();
        }
    }
    /* Is there a write to respond to? */
    if (!b_outgoing && !delayed_queue_w.empty())
    {
             //virtual memory write process, all operation in one cycle
        //in real world, the write operation will be send to DDRC, 
        //and when the write command sent by DFI, the write ack will be send back.
            b_outgoing = delayed_queue_w.front();
            delayed_queue_w.pop_front();
            //write operation done!
            b_outgoing->set_resp(RESP_OKAY);

            /* Write write data into backing store all in one go. */
            uint64_t addr = b_outgoing->get_base_address() & 0xFFFF;

            sc_assert(addr + b_outgoing->get_data_length() <= MEMORY_SIZE);
            b_outgoing->write_out(&mem_data[addr]);

            /* Unref for the w_queue beat ref call. */
            //b_outgoing->unref();     
    }

    if (!aw_queue.empty() && !w_queue.empty()) {
        sc_assert(aw_queue.front() == w_queue.front());

        delay_queue_w.push_back(aw_queue.front());
        aw_queue.pop_front();
        w_queue.pop_front();
    }
    else {        
        delay_queue_w.push_back(NULL);
    }
}

void vmem::clock_negedge()  //interface change
{
    if (r_state == CLEAR && r_outgoing)
    {
        Phase phase = R_VALID;

        r_beat_count--;
        if (r_beat_count == 0)
            phase = R_VALID_LAST;

        r_state = REQ;
        tlm_sync_enum reply = slave.nb_transport_bw(*r_outgoing, phase);
        if (reply == TLM_UPDATED)
        {
            sc_assert(phase == R_READY);
            r_state = ACK;
        }
        cfg_outstand--;
        /* Unref for the ar_queue.push_back() ref. */
        if (r_beat_count == 0)
        {
            //r_outgoing->unref();
            r_outgoing = NULL;
        }
    }

    if (b_outgoing)
    {
        Phase phase = B_VALID;

        b_state = REQ;
        tlm_sync_enum reply = slave.nb_transport_bw(*b_outgoing, phase);
        if (reply == TLM_UPDATED)
        {
            sc_assert(phase == B_READY);
            b_state = ACK;
        }

        outstand_cnt--;
        /* Unref for a aw_queue.push_back() ref. */
        //b_outgoing->unref();
        b_outgoing = NULL;   // then, next back command;
    }

    if (aw_incoming) {
        Phase phase = AW_READY;

        if (aw_queue.size() < cfg_buffer) {
            tlm_sync_enum reply = slave.nb_transport_bw(*aw_incoming, phase);

            aw_queue.push_back(aw_incoming);
            //aw_incoming->ref();

            aw_incoming = NULL;
        }
    }

    if (ar_incoming) {
        Phase phase = AR_READY;

        if (ar_queue.size() < cfg_buffer) {
            tlm_sync_enum reply = slave.nb_transport_bw(*ar_incoming, phase);

            ar_queue.push_back(ar_incoming);
            //ar_incoming->ref();

            ar_incoming = NULL;
        }
    }
}

tlm_sync_enum vmem::nb_transport_fw(Payload& payload, Phase& phase)
{
    switch (phase)
    {
    case AW_VALID:
         phase = AW_READY;
         cfg_outstand++;
        
        if ((aw_queue.size() < cfg_buffer) && (outstand_cnt<cfg_outstand)){
            aw_queue.push_back(&payload);
            //payload.ref();      

            return TLM_UPDATED;
        }
        else {
            //cout << "more write command !" << endl;
            aw_incoming = &payload;           
            return TLM_ACCEPTED;
        }      

    case W_VALID_LAST:
        w_queue.push_back(&payload);
        //payload.ref();
        phase = W_READY;
        return TLM_UPDATED;
    case W_VALID:
        phase = W_READY;
        return TLM_UPDATED;
    case B_READY:    /* no use */
        b_state = ACK;
        return TLM_ACCEPTED;
    case AR_VALID:
        phase = AR_READY;
        outstand_cnt++;
        if ((ar_queue.size() < cfg_buffer)&& (outstand_cnt < cfg_outstand)) {

            ar_queue.push_back(&payload);
            //payload.ref();            
            return TLM_UPDATED;
        }
        else {
            //cout << "more read command !" << endl;
            ar_incoming = &payload;
            return TLM_ACCEPTED;
        }
    case R_READY:
        r_state = ACK;
        return TLM_ACCEPTED;
    case RACK:   /* cannot understand why need this ack flow */
    case WACK:
        return TLM_ACCEPTED;
    default:
        sc_assert(!"Unrecognised phase");
        return TLM_ACCEPTED;
    }
}

vmem::vmem(sc_module_name name, string configname, int id) :
    sc_module(name),
    b_state(CLEAR),
    r_state(CLEAR),
    b_outgoing(NULL),
    r_outgoing(NULL),
    inst_id(id),
    outstand_cnt(0),
    slave("slave", *this, &vmem::nb_transport_fw,
          ARM::TLM::PROTOCOL_ACE, 128),
    clock("clock")
{
    memset(mem_data, 0xdf, MEMORY_SIZE);
    read_config(configname);

    //clock = sc_clock("clk_vmem", 2, SC_NS, 0.5);

    SC_METHOD(clock_posedge);
    sensitive << clock.pos();
    dont_initialize();

    SC_METHOD(clock_negedge);
    sensitive << clock.neg();
    dont_initialize();
}


void vmem::read_config(string cfgname) {

    cfg_outstand = 5;  //default configure

    ptree pt;
    read_xml(cfgname.c_str(), pt, xml_parser::trim_whitespace);

    ptree root = pt.get_child("root");// get all sub point from root
    //Iterator
    for (ptree::iterator pos = root.begin(); pos != root.end(); ++pos)  //boostÖÐµÄauto
    {
        //cout << pos->first << endl;
        if (pos->first == "vmem") {
            if (inst_id == pos->second.get<int>("<xmlattr>.id")) {
                cfg_freq     = pos->second.get<int>("<xmlattr>.freq");
                cfg_bitw     = pos->second.get<int>("<xmlattr>.bitwidth");
                cfg_delay    = pos->second.get<int>("<xmlattr>.delay");
                cfg_buffer   = pos->second.get<int>("<xmlattr>.buffer");
                cfg_outstand = pos->second.get<int>("<xmlattr>.outstand");
            }
        }
    }
}
