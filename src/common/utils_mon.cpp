
#include "utils_mon.h"

using namespace std;


tlm_sync_enum utils_mon::nb_transport_fw(Payload& payload, Phase& phase)
{
    Phase prev_phase = phase;
    tlm_sync_enum reply = master.nb_transport_fw(payload, phase);

    print_payload(payload, prev_phase, reply, phase, 0);

    
    if (payload.get_command() == COMMAND_WRITE  && prev_phase == AW_VALID) {
        wr_bw_sta->sta(payload.get_beat_count() * 128);
        cmd_bw_sta->sta(payload.get_beat_count() * 128);
    }
    else if(payload.get_command() == COMMAND_READ && prev_phase == AR_VALID)
    {
        rd_bw_sta->sta(payload.get_beat_count() * 128);
        cmd_bw_sta->sta(payload.get_beat_count() * 128);
    }    
    else {
        //nothing to do
    }
    
    return reply;
}

tlm_sync_enum utils_mon::nb_transport_bw(Payload& payload, Phase& phase)
{
    Phase prev_phase = phase;

    //delay statics need to do before bw function
    sc_time cur_time = sc_time_stamp();
    int delay = (cur_time - payload.timestamp).to_double() / 1000;  //unit is ns

    if ((phase == B_VALID) || (phase == R_VALID_LAST)) {
        delay_sta->sta(delay);
    }

    tlm_sync_enum reply = slave.nb_transport_bw(payload, phase);

    print_payload(payload, prev_phase, reply, phase, 1);


    return reply;
}

void utils_mon::print_payload(Payload& payload, Phase phase,
    tlm_sync_enum reply, Phase reply_phase, bool direc)
{
    ostringstream stream;

    Command command = payload.get_command();

    const char* phase_name = "?";
    int phase_id = 0;
    bool show_addr = true;
    bool show_data = false;
    bool show_resp = false;
    bool inc_beat = false;
    bool first_beat = false;
    bool last_beat = false;

    bool updated = reply == TLM_UPDATED;

    switch (phase)
    {
    case PHASE_UNINITIALIZED:
        phase_name = "PHASE_UNINITIALIZED";
        phase_id = 0;
        show_addr = false;
        break;
    case AW_VALID:
        phase_name = (updated ? "AW VALID READY" : "AW VALID -----");
        phase_id = 1;
        break;
    case AW_READY:
        phase_name = "AW ----- READY";
        phase_id = 2;
        break;
    case W_VALID:
    case W_VALID_LAST:
        phase_name = (updated ? " W  VALID READY" : " W  VALID -----");
        phase_id = 3;
        inc_beat = updated;
        show_data = true;
        first_beat = true;
        last_beat = updated;
        break;
    case W_READY:
        inc_beat = true;
        phase_id = 4;
        phase_name = " W  ----- READY";
        show_data = true;
        last_beat = true;
        break;
    case B_VALID:
        phase_name = (updated ? " B  VALID READY" : " B  VALID -----");
        phase_id = 5;
        show_resp = true;
        last_beat = updated;
        break;
    case B_READY:
        phase_name = " B  ----- READY";
        phase_id = 6;
        show_resp = true;
        last_beat = true;
        break;
    case AR_VALID:
        phase_name = (updated ? "AR VALID READY" : "AR VALID -----");
        phase_id = 7;
        break;
    case AR_READY:
        phase_name = "AR ----- READY";
        phase_id = 8;
        break;
    case R_VALID:
    case R_VALID_LAST:
        phase_name = (updated ? " R  VALID READY" : " R  VALID -----");
        phase_id = 9;
        inc_beat = updated;
        show_data = true;
        show_resp = true;
        first_beat = true;
        last_beat = updated;
        break;
    case R_READY:
        inc_beat = true;
        phase_name = " R  ----- READY";
        phase_id = 10;
        show_data = true;
        show_resp = true;
        last_beat = true;
        break;
    case AC_VALID:
        phase_name = (updated ? "AC VALID READY" : "AC VALID -----");
        phase_id = 11;
        break;
    case AC_READY:
        phase_name = "AC ----- READY";
        phase_id = 12;
        break;
    case CR_VALID:
        phase_name = (updated ? "CR VALID READY" : "CR VALID -----");
        phase_id = 13;
        break;
    case CR_READY:
        phase_name = "CR ----- READY";
        phase_id = 14;
        break;
    case CD_VALID:
    case CD_VALID_LAST:
        phase_name = (updated ? "CD VALID READY" : "CD VALID -----");
        phase_id = 15;
        inc_beat = updated;
        show_data = true;
        first_beat = true;
        last_beat = updated;
        break;
    case CD_READY:
        inc_beat = true;
        phase_name = "CD ----- READY";
        phase_id = 16;
        show_data = true;
        last_beat = true;
        break;
    case WACK:
        phase_name = "WACK";
        phase_id = 17;
        show_addr = false;
        break;
    case RACK:
        phase_name = "RACK";
        phase_id = 18;
        show_addr = false;
        break;
    default:
        show_addr = false;
        break;
    }

    /* Remember which beat we're up to in observed payloads. */
    if (first_beat && payload_burst_index.find(&payload) ==
        payload_burst_index.end())
    {
        payload_burst_index[&payload] = 0;
    }

    sc_time cur_time;
    cur_time = sc_time_stamp();

    stream <<setw(12)<< cur_time.to_string() << ' ' << name() << ": " << "     cid:"
        << setw(3) << right << payload.uid
        << "   mid:"
        << setw(3) << right << payload.id
        << setw(24) << right << phase_name << ' ';

    if (show_addr)
    {
        stream << "@" << setw(12) <<  hex
            << payload.get_address() << dec << ' ';

        if (command != COMMAND_SNOOP)
        {
            const static char* burst_types[] = { "FIXED", "INCR ", "WRAP " };
            Burst burst = payload.get_burst();

            stream <<setw(4)<< payload.get_beat_count() << "x" <<
                (8 * (1 << payload.get_size())) << "bits ";
            stream << (burst <= BURST_WRAP ? burst_types[burst] : "?????")
                << ' ';
        }
    }

    const static char* resp_types[] =
        { "OKAY  ", "EXOKAY", "SLVERR", "DECERR" };
    Resp resp = payload.get_resp();

    if (show_resp)
        stream << (resp <= RESP_DECERR ? resp_types[resp] : "??????") << ' ';
    else
        stream << "       ";

    if (show_data)
    {
        unsigned burst_index = payload_burst_index[&payload];

        stream << (burst_index == payload.get_len() ? "LAST " : "     ")
            << "DATA:";

        uint64_t byte_strobe(uint64_t(~0));

        switch (payload.get_command())
        {
        case COMMAND_WRITE:
            payload.write_out_beat(burst_index, beat_data);
            byte_strobe = payload.write_out_beat_strobe(burst_index);
            break;
        case COMMAND_READ:
            payload.read_out_beat(burst_index, beat_data);
            break;
        case COMMAND_SNOOP:
            payload.snoop_out_beat(burst_index, beat_data);
            break;
        default:
            assert(0);
            break;
        }

        stream << uppercase << hex;
        unsigned size = 1 << payload.get_size();
        for (int i = size - 1; i >= 0; i--)
        {
            if ((byte_strobe >> (i % 8)) & 1)
                stream << setw(2) << setfill('0') << unsigned(beat_data[i]);
            else
                stream << "XX";
            if (i != 0 && !(i % 8))
                stream << "_";
        }
        stream << dec;

        /* Increment beat index on data valid. */
        if (inc_beat)
            payload_burst_index[&payload] = burst_index + 1;
    }

    /*
     * Drop the payload_burst_index for this payload once all data has been
     * shown.
     * */
    if (last_beat && payload_burst_index[&payload] == payload.get_beat_count())
        payload_burst_index.erase(&payload);

    stream << '\n';
    //cout << stream.str();
    if (direc == 0) {
        log_master << stream.str();
    }
    else {
        log_slave << stream.str();
    }
    
}

utils_mon::utils_mon(sc_module_name name, unsigned port_width) :
    sc_module(name),
    beat_data(new uint8_t[port_width >> 3]),
    slave("slave", *this, &utils_mon::nb_transport_fw, ARM::TLM::PROTOCOL_ACE,
        port_width),
    master("master", *this, &utils_mon::nb_transport_bw, ARM::TLM::PROTOCOL_ACE,
        port_width)
{
    delay_sta = new st_delay("mon delay");

    cmd_bw_sta = new st_bw("mon   all-cmd bw");
    rd_bw_sta  = new st_bw("mon  read-cmd bw"); 
    wr_bw_sta  = new st_bw("mon write-cmd bw");


   // logfile <<name<<".log"<<endl;
    ostringstream filename;
    filename << name << "_master.log";
    log_master.open(filename.str().c_str());
    filename.str("");
    filename << name << "_slave.log";
    log_slave.open(filename.str().c_str());

    filename.str("");
    filename <<"sta_" << name << ".log";
    log_slave_sta.open(filename.str().c_str());
}

utils_mon::~utils_mon()
{
    log_master.close();
    log_slave.close();
    delete[] beat_data;
}


void utils_mon::st_print() {
    log_slave_sta << delay_sta->report();

    log_slave_sta << cmd_bw_sta->phase_head.str();
    log_slave_sta << cmd_bw_sta->report();
    log_slave_sta << wr_bw_sta->report();
    log_slave_sta << rd_bw_sta->report();    //

}
