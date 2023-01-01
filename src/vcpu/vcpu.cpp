#include <cstring>

#include "vcpu.h"

using namespace std;
using namespace sc_core;
using namespace tlm;
using namespace ARM::AXI4;

void vcpu::clock_posedge()
{
    if (aw_state == ACK)
        aw_state = CLEAR;

    if (w_state == ACK)
        w_state = CLEAR;

    if (ar_state == ACK)
        ar_state = CLEAR;

    if (cr_state == ACK)
        cr_state = CLEAR;

    if (cr_incoming)
    {
        cr_queue.push_back(cr_incoming);
        cr_incoming = NULL;
    }

    if (rack_incoming)
    {
        rack_queue.push_back(rack_incoming);
        rack_incoming = NULL;
    }

    if (wack_incoming)
    {
        wack_queue.push_back(wack_incoming);
        // write ack return
        wack_incoming = NULL;
    }
}

void vcpu::clock_negedge()
{
    /* Send next payload AWVALID */
    if (aw_state == CLEAR && !aw_queue.empty())
    {
        Payload* payload = aw_queue.front();
        Phase phase = AW_VALID;

        //timestamp insert
        sc_time cur_time = sc_time_stamp();
        payload->timestamp = cur_time;

        w_queue.push_back(payload);
        aw_queue.pop_front();

        aw_state = REQ;
        tlm_sync_enum reply = master.nb_transport_fw(*payload, phase);
        if (reply == TLM_UPDATED)
        {
            sc_assert(phase == AW_READY);
            aw_state = ACK;
        }
    }

    /* Send next payload ARVALID */
    if (ar_state == CLEAR && !ar_queue.empty())
    {
        Payload* payload = ar_queue.front();
        Phase phase = AR_VALID;

        ar_queue.pop_front();

        ar_state = REQ;
        tlm_sync_enum reply = master.nb_transport_fw(*payload, phase);
        if (reply == TLM_UPDATED)
        {
            sc_assert(phase == AR_READY);
            ar_state = ACK; 
        }
    }

    /* Send write beat WVALID */
    if (w_state == CLEAR && !w_queue.empty())
    {
        Payload* payload = w_queue.front();
        Phase phase = W_VALID;

        /* Example beat data using the beat count as data */
        uint8_t data_beat[128 / 8];
        memset(data_beat, w_beat_count, 128 / 8); //memset(point *var, uchar value(0~255), sizeof(var))
        w_beat_count++;
        payload->write_in_beat(data_beat);  //write date to payload beatdata;

        if (w_beat_count == payload->get_beat_count())
        {
            phase = W_VALID_LAST;
            w_queue.pop_front();
            w_beat_count = 0;
        }

        w_state = REQ;
        tlm_sync_enum reply = master.nb_transport_fw(*payload, phase);
        if (reply == TLM_UPDATED)   // state clear->req->ack->clear, or clear->ack->clear,  如果是
        {
            sc_assert(phase == W_READY);
            w_state = ACK;
        }
    }

    /* Send CRVALID response but no snoop data */
    if (cr_state == CLEAR && !cr_queue.empty())
    {
        Phase phase = CR_VALID;
        Payload* payload = cr_queue.front();

        cr_queue.pop_front();

        cr_state = REQ;
        tlm_sync_enum reply = master.nb_transport_fw(*payload, phase);
        if (reply == TLM_UPDATED)
        {
            sc_assert(phase == CR_READY);
            cr_state = ACK;
            payload->unref();
        }
    }

    /* Send WACK */
    if (!wack_queue.empty())  /* cannot understand why need this ack flow */
    {
        Phase phase = WACK;
        Payload* payload = wack_queue.front();

        wack_queue.pop_front();

        tlm_sync_enum reply = master.nb_transport_fw(*payload, phase);
        sc_assert(reply == TLM_ACCEPTED);

        payload->unref();
    }

    /* Send RACK */
    if (!rack_queue.empty())
    {
        Phase phase = RACK;
        Payload* payload = rack_queue.front();

        rack_queue.pop_front();

        tlm_sync_enum reply = master.nb_transport_fw(*payload, phase);
        sc_assert(reply == TLM_ACCEPTED);

        payload->unref();
    }
}

tlm_sync_enum vcpu::nb_transport_bw(Payload& payload,
    Phase& phase)
{
    switch (phase)
    {
    case AW_READY:
        aw_state = ACK;
        return TLM_ACCEPTED;
    case W_READY:
        w_state = ACK;
        return TLM_ACCEPTED;
    case B_VALID:
        wack_incoming = &payload;
        phase = B_READY;
        return TLM_UPDATED;
    case AR_READY:    //逆向传输是不需要payload数据的
        ar_state = ACK;
        return TLM_ACCEPTED;
    case R_VALID_LAST:
        /* Move to RACK queue after last beat. */
        rack_incoming = &payload;
    /* Fall through */
    case R_VALID:
        phase = R_READY;
        return TLM_UPDATED;
    case AC_VALID:
        cr_incoming = &payload;
        /* Keep the AC payload to use with CR response. */
        payload.ref();
        phase = AC_READY;
        return TLM_UPDATED;
    case CR_READY:
        cr_state = ACK;
        payload.unref();
        return TLM_ACCEPTED;
    default:
        sc_assert(!"Unrecognised phase");
        return TLM_ACCEPTED;
    }
}

vcpu::vcpu(sc_core::sc_module_name name, std::string config_name, int gid) :
    sc_module(name),
    aw_state(CLEAR),
    w_state(CLEAR),
    ar_state(CLEAR),
    cr_state(CLEAR),
    cr_incoming(NULL),
    wack_incoming(NULL),
    rack_incoming(NULL),
    w_beat_count(0),
    cfg_id(gid),
    sys_time_base(sc_time(0,SC_NS)),
    configname(config_name),
    master("master", *this, &vcpu::nb_transport_bw,
           ARM::TLM::PROTOCOL_ACE, 128),
    clock("clock")
{
    SC_METHOD(clock_posedge);
    sensitive << clock.pos();
    dont_initialize();

    SC_METHOD(clock_negedge);
    sensitive << clock.neg();
    dont_initialize();

    SC_METHOD(send_req);
    sensitive << clock.pos();
    dont_initialize();

    read_config();
}

void vcpu::add_payload(Command command, uint64_t address, Size size,
    uint8_t len, sc_time inc_time)
{
    Payload* payload = Payload::new_payload(command, address, size, len, BURST_INCR);

    payload->cache = CacheBitEnum() | CACHE_AW_B;
    payload->id = cfg_id;
    payload->region = address >> 16; //get high address ( 0x00014000 -> 0x0001 ) for target ID
    payload->user   = cfg_id;   // Inst ID for master ID
    payload->qos    = cfg_qos;
    //payload->timestamp = sc_time_stamp();
    payload->send_ts_base = inc_time;
   
    //cout << " req " << vpu_id << "  : " << payload->send_ts_base<< endl;
    wait_queue.push_back(payload);     
}

void vcpu::send_req()
{
    Payload* payload;
    sc_time cur_time = sc_time_stamp();
    bool is_on = 1;

    while (wait_queue.size()>0 && is_on) {
        payload = wait_queue.front();
        
        if (cur_time > payload->send_ts_base) {
            payload->timestamp = cur_time;
            switch (payload->get_command())
            {
            case COMMAND_WRITE:
                aw_queue.push_back(payload);
                break;
            case COMMAND_READ:
                ar_queue.push_back(payload);
                break;
            default:
                sc_assert(!"Can only generate read and write traffic");
            }

            wait_queue.pop_front();
        }
        else {
            is_on = 0;
        }
    }
}

void vcpu::read_config()
{
    ptree pt;
    read_xml(configname.c_str(), pt, xml_parser::trim_whitespace);

    //ConfigParser parser;

    ptree root = pt.get_child("root");// get all sub point from root
    //Iterator
    for (ptree::iterator pos = root.begin(); pos != root.end(); ++pos)  //boost中的auto
    {
        //cout << pos->first << endl;
        if (pos->first == "vcpu") {
            if (cfg_id == pos->second.get<int>("<xmlattr>.id")){ 

                cfg_qos   = pos->second.get<int>("<xmlattr>.qos");
                cfg_burst = pos->second.get<int>("<xmlattr>.burst");
                ptree child = pos->second.get_child("gen");
                sc_time rund_base = sc_time(0, SC_NS);

                for (auto posx = child.begin(); posx != child.end(); ++posx) {
                    int rnd_num = posx->second.get<int>("<xmlattr>.num");
                    int rnd_intr = posx->second.get<int>("<xmlattr>.interval");
                    string rnd_unit = posx->second.get<string>("<xmlattr>.ts_unit");
                    sc_time rund_intr = sc_time(rnd_intr, ts_fix(rnd_unit));

                    for (int i = 0; i < rnd_num; i++) {

                        ptree subchild = pos->second.get_child("pattern"); // pos -- vcpu point                        
                        //read detail point
                        for (auto posy = subchild.begin(); posy != subchild.end();++posy) 
                        {
                            if (posy->first == "req")
                            {
                                string req_cmd    = posy->second.get<string>("<xmlattr>.cmd");
                                string req_addr   = posy->second.get<string>("<xmlattr>.addr");
                                int    req_size   = posy->second.get<int>("<xmlattr>.size") -1;  // default value - 1

                                string req_burst  = posy->second.get<string>("<xmlattr>.burst");                                
                                int    req_ts_inc = posy->second.get<int>("<xmlattr>.ts_inc");
                                
                                string  req_ts_iu = posy->second.get<string>("<xmlattr>.ts_iu");                                
                                sc_time req_ts_i  = sc_time(req_ts_inc, ts_fix(req_ts_iu)) + rund_base;
                                stringstream addr_s;
                                uint64_t  addr_real;
                                addr_s << hex << req_addr;
                                addr_s >> addr_real;

                                rund_base += sc_time(req_ts_inc, ts_fix(req_ts_iu));
                                add_payload(req_fix(req_cmd), addr_real, SIZE_16, req_size, req_ts_i);
                            }
                        }
                        rund_base += rund_intr;
                    }
                }
            }          
        }
    }
}

sc_time_unit vcpu::ts_fix(string instring) {

    sc_time_unit tmp;

    if (instring == "us" or instring == "US") {
        tmp = SC_US;
    }
    else if (instring == "ns" or instring == "NS") {
        tmp = SC_NS;
    }
    else if (instring == "ms" or instring == "MS") {
        tmp = SC_MS;
    }
    else {
        cout << "Unknow Simulation Time Unit!!" << instring << "" << endl;
        tmp = SC_NS;
    }

    return tmp;
}

ARM::AXI4::Command vcpu::req_fix(string instring) {

    ARM::AXI4::Command tmp;
    if (instring == "read" or instring=="READ") {
        tmp = COMMAND_READ;
    }
    else if (instring == "write" or instring == "WRITE") {
        tmp = COMMAND_WRITE;
    }
    else {
        cout << "Unknow Simulation Time Unit!!" << instring << "" << endl;
        tmp = COMMAND_READ;
    }

    return tmp;
}

