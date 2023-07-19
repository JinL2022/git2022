#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "vbus.h"

using namespace std;
using namespace tlm;
using namespace sc_core;
using namespace ARM::AXI4;

void vbus::clock_posedge()
{
    // do something only in block
    switch_req();

    switch_resp();

    //do some edit

    for (int i = 0; i < 4; i++)
    {
        if (aw_state[i] == ACK)
            aw_state[i] = CLEAR;

        if (w_state[i] == ACK)
            w_state[i] = CLEAR;

        if (ar_state[i] == ACK)
            ar_state[i] = CLEAR;

        if (b_state[i] == ACK)
            b_state[i] = CLEAR;

        if (r_state[i] == ACK)
            r_state[i] = CLEAR; 
    }

    if (!r_outgoing[0] && !r_queue[0].empty())
    {
        r_outgoing[0] = r_queue[0].front();
        r_queue[0].pop_front();
        r_beat_count[0] = r_outgoing[0]->get_beat_count();      
    }
    /* Is there a write to respond to? */
    if (!b_outgoing[0] && !b_queue[0].empty())
    {
        b_outgoing[0] = b_queue[0].front();
        b_queue[0].pop_front();
        /* Unref for the w_queue beat ref call. */
        //b_outgoing[0]->unref();      
    }

    if (!r_outgoing[1] && !r_queue[1].empty())
    {
        r_outgoing[1] = r_queue[1].front();
        r_queue[1].pop_front();
        r_beat_count[1] = r_outgoing[1]->get_beat_count();
    }
    /* Is there a write to respond to? */
    if (!b_outgoing[1] && !b_queue[1].empty())
    {
        b_outgoing[1] = b_queue[1].front();
        b_queue[1].pop_front();
        /* Unref for the w_queue beat ref call. */
        //b_outgoing[1]->unref();
    }

    if (!r_outgoing[2] && !r_queue[2].empty())
    {
        r_outgoing[2] = r_queue[2].front();
        r_queue[2].pop_front();
        r_beat_count[2] = r_outgoing[2]->get_beat_count();
    }
    /* Is there a write to respond to? */
    if (!b_outgoing[2] && !b_queue[2].empty())
    {
        b_outgoing[2] = b_queue[2].front();
        b_queue[2].pop_front();
        /* Unref for the w_queue beat ref call. */
        //b_outgoing[2]->unref();
    }

    if (!r_outgoing[3] && !r_queue[3].empty())
    {
        r_outgoing[3] = r_queue[3].front();
        r_queue[3].pop_front();
        r_beat_count[3] = r_outgoing[3]->get_beat_count();
    }
    /* Is there a write to respond to? */
    if (!b_outgoing[3] && !b_queue[3].empty())
    {
        b_outgoing[3] = b_queue[3].front();
        b_queue[3].pop_front();
        /* Unref for the w_queue beat ref call. */
        //b_outgoing[3]->unref();
    }
    //if (cr_state == ACK)
    //    cr_state = CLEAR;

    //if (cr_incoming)
    //{
    //    cr_queue.push_back(cr_incoming);
    //    cr_incoming = NULL;
    //}

    //if (rack_incoming)
    //{
    //    rack_queue.push_back(rack_incoming);
    //    rack_incoming = NULL;
    //}

    //if (wack_incoming)
    //{
    //    wack_queue.push_back(wack_incoming);
    //
    //    // write ack return
    //    wack_incoming = NULL;
    //}
}

void vbus::clock_negedge()
{
    if (aw_incoming[0]) {
        Phase phase = AW_READY;

        if (outstand_cnt[0] < cfg_outstand) {
            tlm_sync_enum reply = slave_a.nb_transport_bw(*aw_incoming[0], phase);

            aw_queue[0].push_back(aw_incoming[0]);
            //aw_incoming[0]->ref();

            aw_incoming[0] = NULL;
        }
    }

    if (aw_incoming[1]) {
        Phase phase = AW_READY;

        if (outstand_cnt[1] < cfg_outstand) {
            tlm_sync_enum reply = slave_b.nb_transport_bw(*aw_incoming[1], phase);

            aw_queue[1].push_back(aw_incoming[1]);
            //aw_incoming[1]->ref();

            aw_incoming[1] = NULL;
        }
    }

    if (aw_incoming[2]) {
        Phase phase = AW_READY;

        if (outstand_cnt[2] < cfg_outstand) {
            tlm_sync_enum reply = slave_c.nb_transport_bw(*aw_incoming[2], phase);

            aw_queue[2].push_back(aw_incoming[2]);
            //aw_incoming[2]->ref();

            aw_incoming[2] = NULL;
        }
    }

    if (aw_incoming[3]) {
        Phase phase = AW_READY;

        if (outstand_cnt[3] < cfg_outstand) {
            tlm_sync_enum reply = slave_d.nb_transport_bw(*aw_incoming[3], phase);

            aw_queue[3].push_back(aw_incoming[3]);
            //aw_incoming[3]->ref();

            aw_incoming[3] = NULL;
        }
    }

    if (ar_incoming[0]) {
        Phase phase = AR_READY;

        if (ar_queue[0].size() < cfg_outstand) {
            tlm_sync_enum reply = slave_a.nb_transport_bw(*ar_incoming[0], phase);

            ar_queue[0].push_back(ar_incoming[0]);
            //ar_incoming[0]->ref();

            ar_incoming[0] = NULL;
        }
    }

    if (ar_incoming[1]) {
        Phase phase = AR_READY;

        if (ar_queue[1].size() < cfg_outstand) {
            tlm_sync_enum reply = slave_b.nb_transport_bw(*ar_incoming[1], phase);

            ar_queue[1].push_back(ar_incoming[1]);
            //ar_incoming[1]->ref();

            ar_incoming[1] = NULL;
        }
    }

    if (ar_incoming[2]) {
        Phase phase = AR_READY;

        if (ar_queue[2].size() < cfg_outstand) {
            tlm_sync_enum reply = slave_c.nb_transport_bw(*ar_incoming[2], phase);

            ar_queue[2].push_back(ar_incoming[2]);
            //ar_incoming[2]->ref();

            ar_incoming[2] = NULL;
        }
    }

    if (ar_incoming[3]) {
        Phase phase = AR_READY;

        if (ar_queue[3].size() < cfg_outstand) {
            tlm_sync_enum reply = slave_d.nb_transport_bw(*ar_incoming[3], phase);

            ar_queue[3].push_back(ar_incoming[3]);
            //ar_incoming[3]->ref();

            ar_incoming[3] = NULL;
        }
    }

    // Tranmits Part One
    if (aw_state[0] == CLEAR && !s_aw_queue[0].empty())
    {
        Payload* payload;
        Phase phase = AW_VALID;
        payload = s_aw_queue[0].front();      
        //w_queue0.push_back(payload); 

        s_aw_queue[0].pop_front();
        aw_state[0] = REQ;
        tlm_sync_enum reply = master_a.nb_transport_fw(*payload, phase);
        //outstand_cnt[0]--;
        if (reply == TLM_UPDATED)
        {
            sc_assert(phase == AW_READY);
            aw_state[0] = ACK;
        }
    }

    if (ar_state[0] == CLEAR && !s_ar_queue[0].empty())
    {
        Payload* payload;
        Phase phase = AR_VALID;        
        payload = s_ar_queue[0].front();
        s_ar_queue[0].pop_front();  
        ar_state[0] = REQ;
        tlm_sync_enum reply = master_a.nb_transport_fw(*payload, phase);
        //outstand_cnt[0]--;
        if (reply == TLM_UPDATED)
        {
            sc_assert(phase == AR_READY);
            ar_state[0] = ACK;
        }
    }

    // Send write beat WVALID
    if (w_state[0] == CLEAR && !s_w_queue[0].empty())
    {
        Payload* payload = s_w_queue[0].front();
        Phase phase = W_VALID;     
     
        w_beat_count[0]++;
        //payload->write_in_beat(data_beat);  //write date to payload beatdata;

        if (w_beat_count[0] == payload->get_beat_count())
        {
            phase = W_VALID_LAST;
            s_w_queue[0].pop_front();
            w_beat_count[0] = 0;           
        }

        w_state[0] = REQ;
        tlm_sync_enum reply = master_a.nb_transport_fw(*payload, phase);

        buffer_cnt[0]--;
        if (reply == TLM_UPDATED)   // state clear->req->ack->clear, or clear->ack->clear,  �����
        {
            sc_assert(phase == W_READY);
            w_state[0] = ACK;
        }
    } 

    if (r_state[0] == CLEAR && r_outgoing[0])
    {
        Phase phase = R_VALID;

        r_beat_count[0]--;
        if (r_beat_count[0] == 0)
            phase = R_VALID_LAST;

        r_state[0] = REQ;
        tlm_sync_enum reply = slave_a.nb_transport_bw(*r_outgoing[0], phase);
        if (reply == TLM_UPDATED)
        {
            sc_assert(phase == R_READY);
            r_state[0] = ACK;
        }
        //outstand_cnt[0]--;
        /* Unref for the ar_queue.push_back() ref. */
        if (r_beat_count[0] == 0)
        {
            //r_outgoing[0]->unref();
            r_outgoing[0] = NULL;
        }
    }

    if (b_outgoing[0])
    {
        Phase phase = B_VALID;

        b_state[0] = REQ;
        tlm_sync_enum reply = slave_a.nb_transport_bw(*b_outgoing[0], phase);
        if (reply == TLM_UPDATED)
        {
            sc_assert(phase == B_READY);
            b_state[0] = ACK;
        }

        //outstand_cnt[0]--;
        /* Unref for a aw_queue.push_back() ref. */
        //b_outgoing[0]->unref();
        b_outgoing[0] = NULL;   // then, next back command;
    }

    // Tranmits Part Two
    if (aw_state[1] == CLEAR && !s_aw_queue[1].empty())
    {
        Payload* payload;
        Phase phase = AW_VALID;
        payload = s_aw_queue[1].front();
        //w_queue0.push_back(payload); 

        s_aw_queue[1].pop_front();
        aw_state[1] = REQ;
        tlm_sync_enum reply = master_b.nb_transport_fw(*payload, phase);
        outstand_cnt[1]--;
        if (reply == TLM_UPDATED)
        {
            sc_assert(phase == AW_READY);
            aw_state[1] = ACK;
        }
    }

    if (ar_state[1] == CLEAR && !s_ar_queue[1].empty())
    {
        Payload* payload;
        Phase phase = AR_VALID;
        payload = s_ar_queue[1].front();
        s_ar_queue[1].pop_front();
        ar_state[1] = REQ;
        tlm_sync_enum reply = master_b.nb_transport_fw(*payload, phase);
        outstand_cnt[1]--;
        if (reply == TLM_UPDATED)
        {
            sc_assert(phase == AR_READY);
            ar_state[1] = ACK;
        }
    }

    // Send write beat WVALID
    if (w_state[1] == CLEAR && !s_w_queue[1].empty())
    {
        Payload* payload = s_w_queue[1].front();
        Phase phase = W_VALID;

        w_beat_count[1]++;
        //payload->write_in_beat(data_beat);  //write date to payload beatdata;

        if (w_beat_count[1] == payload->get_beat_count())
        {
            phase = W_VALID_LAST;
            s_w_queue[1].pop_front();
            w_beat_count[1] = 0;
        }

        w_state[1] = REQ;
        tlm_sync_enum reply = master_b.nb_transport_fw(*payload, phase);
        buffer_cnt[1]--;
        if (reply == TLM_UPDATED)   // state clear->req->ack->clear, or clear->ack->clear,  �����
        {
            sc_assert(phase == W_READY);
            w_state[1] = ACK;
        }
    }

    if (r_state[1] == CLEAR && r_outgoing[1])
    {
        Phase phase = R_VALID;

        r_beat_count[1]--;
        if (r_beat_count[1] == 0)
            phase = R_VALID_LAST;

        r_state[1] = REQ;
        tlm_sync_enum reply = slave_b.nb_transport_bw(*r_outgoing[1], phase);
        if (reply == TLM_UPDATED)
        {
            sc_assert(phase == R_READY);
            r_state[1] = ACK;
        }
        //outstand_cnt[0]--;
        /* Unref for the ar_queue.push_back() ref. */
        if (r_beat_count[1] == 0)
        {
            //r_outgoing[1]->unref();
            r_outgoing[1] = NULL;
        }
    }

    if (b_outgoing[1])
    {
        Phase phase = B_VALID;

        b_state[1] = REQ;
        tlm_sync_enum reply = slave_b.nb_transport_bw(*b_outgoing[1], phase);
        if (reply == TLM_UPDATED)
        {
            sc_assert(phase == B_READY);
            b_state[1] = ACK;
        }

        //outstand_cnt[0]--;
        /* Unref for a aw_queue.push_back() ref. */
        //b_outgoing[1]->unref();
        b_outgoing[1] = NULL;   // then, next back command;
    }
    
    // Tranmits Part Three
    if (aw_state[2] == CLEAR && !s_aw_queue[2].empty())
    {
        Payload* payload;
        Phase phase = AW_VALID;
        payload = s_aw_queue[2].front();
        //w_queue0.push_back(payload); 

        s_aw_queue[2].pop_front();
        aw_state[2] = REQ;
        tlm_sync_enum reply = master_c.nb_transport_fw(*payload, phase);
        outstand_cnt[2]--;
        if (reply == TLM_UPDATED)
        {
            sc_assert(phase == AW_READY);
            aw_state[2] = ACK;
        }
    }

    if (ar_state[2] == CLEAR && !s_ar_queue[2].empty())
    {
        Payload* payload;
        Phase phase = AR_VALID;
        payload = s_ar_queue[2].front();
        s_ar_queue[2].pop_front();
        ar_state[2] = REQ;
        tlm_sync_enum reply = master_c.nb_transport_fw(*payload, phase);
        outstand_cnt[2]--;
        if (reply == TLM_UPDATED)
        {
            sc_assert(phase == AR_READY);
            ar_state[2] = ACK;
        }
    }

    // Send write beat WVALID
    if (w_state[2] == CLEAR && !s_w_queue[2].empty())
    {
        Payload* payload = s_w_queue[2].front();
        Phase phase = W_VALID;

        w_beat_count[2]++;
        //payload->write_in_beat(data_beat);  //write date to payload beatdata;

        if (w_beat_count[2] == payload->get_beat_count())
        {
            phase = W_VALID_LAST;
            s_w_queue[2].pop_front();
            w_beat_count[2] = 0;
        }

        w_state[2] = REQ;
        tlm_sync_enum reply = master_c.nb_transport_fw(*payload, phase);
        buffer_cnt[2]--;
        if (reply == TLM_UPDATED)   // state clear->req->ack->clear, or clear->ack->clear,  �����
        {
            sc_assert(phase == W_READY);
            w_state[2] = ACK;
        }
    }

    if (r_state[2] == CLEAR && r_outgoing[2])
    {
        Phase phase = R_VALID;

        r_beat_count[2]--;
        if (r_beat_count[2] == 0)
            phase = R_VALID_LAST;

        r_state[2] = REQ;
        tlm_sync_enum reply = slave_c.nb_transport_bw(*r_outgoing[2], phase);
        if (reply == TLM_UPDATED)
        {
            sc_assert(phase == R_READY);
            r_state[2] = ACK;
        }
        //outstand_cnt[0]--;
        /* Unref for the ar_queue.push_back() ref. */
        if (r_beat_count[2] == 0)
        {
            //r_outgoing[2]->unref();
            r_outgoing[2] = NULL;
        }
    }

    if (b_outgoing[2])
    {
        Phase phase = B_VALID;

        b_state[2] = REQ;
        tlm_sync_enum reply = slave_c.nb_transport_bw(*b_outgoing[2], phase);
        if (reply == TLM_UPDATED)
        {
            sc_assert(phase == B_READY);
            b_state[2] = ACK;
        }

        //outstand_cnt[0]--;
        /* Unref for a aw_queue.push_back() ref. */
        //b_outgoing[2]->unref();
        b_outgoing[2] = NULL;   // then, next back command;
    }
    
    // Tranmits Part Four
    if (aw_state[3] == CLEAR && !s_aw_queue[3].empty())
    {
        Payload* payload;
        Phase phase = AW_VALID;
        payload = s_aw_queue[3].front();
        //w_queue0.push_back(payload); 

        s_aw_queue[3].pop_front();
        aw_state[3] = REQ;
        tlm_sync_enum reply = master_d.nb_transport_fw(*payload, phase);
        outstand_cnt[3]--;
        if (reply == TLM_UPDATED)
        {
            sc_assert(phase == AW_READY);
            aw_state[3] = ACK;
        }
    }

    if (ar_state[3] == CLEAR && !s_ar_queue[3].empty())
    {
        Payload* payload;
        Phase phase = AR_VALID;
        payload = s_ar_queue[0].front();
        s_ar_queue[3].pop_front();
        ar_state[3] = REQ;
        tlm_sync_enum reply = master_d.nb_transport_fw(*payload, phase);
        outstand_cnt[3]--;
        if (reply == TLM_UPDATED)
        {
            sc_assert(phase == AR_READY);
            ar_state[3] = ACK;
        }
    }

    // Send write beat WVALID
    if (w_state[3] == CLEAR && !s_w_queue[3].empty())
    {
        Payload* payload = s_w_queue[3].front();
        Phase phase = W_VALID;

        w_beat_count[3]++;
        //payload->write_in_beat(data_beat);  //write date to payload beatdata;

        if (w_beat_count[3] == payload->get_beat_count())
        {
            phase = W_VALID_LAST;
            s_w_queue[3].pop_front();
            w_beat_count[3] = 0;
        }

        w_state[3] = REQ;
        tlm_sync_enum reply = master_d.nb_transport_fw(*payload, phase);
        buffer_cnt[3]--;
        if (reply == TLM_UPDATED)   // state clear->req->ack->clear, or clear->ack->clear,  �����
        {
            sc_assert(phase == W_READY);
            w_state[3] = ACK;
        }
    }

    if (r_state[3] == CLEAR && r_outgoing[3])
    {
        Phase phase = R_VALID;

        r_beat_count[3]--;
        if (r_beat_count[3] == 0)
            phase = R_VALID_LAST;

        r_state[3] = REQ;
        tlm_sync_enum reply = slave_d.nb_transport_bw(*r_outgoing[3], phase);
        if (reply == TLM_UPDATED)
        {
            sc_assert(phase == R_READY);
            r_state[3] = ACK;
        }
        //outstand_cnt[0]--;
        /* Unref for the ar_queue.push_back() ref. */
        if (r_beat_count[3] == 0)
        {
            //r_outgoing[3]->unref();
            r_outgoing[3] = NULL;
        }
    }

    if (b_outgoing[3])
    {
        Phase phase = B_VALID;

        b_state[3] = REQ;
        tlm_sync_enum reply = slave_d.nb_transport_bw(*b_outgoing[3], phase);
        if (reply == TLM_UPDATED)
        {
            sc_assert(phase == B_READY);
            b_state[3] = ACK;
        }

        //outstand_cnt[0]--;
        /* Unref for a aw_queue.push_back() ref. */
        //b_outgoing[3]->unref();
        b_outgoing[3] = NULL;   // then, next back command;
    }
}

tlm_sync_enum vbus::nb_transport_fwa(Payload& payload, Phase& phase)
{
    switch (phase)
    {
    case AW_VALID:
        phase = AW_READY;

        outstand_cnt[0]++;
        if ((outstand_cnt[0] < cfg_outstand)&&(buffer_cnt[0]<cfg_buffer)) {
            aw_queue[0].push_back(&payload);
            //payload.ref();

            return TLM_UPDATED;
        }
        else {
            //cout << "more write command !" << endl;
            aw_incoming[0] = &payload;
            return TLM_ACCEPTED;
        }

    case W_VALID_LAST:
        w_queue[0].push_back(&payload); //Need to know target id;
        //payload.ref();
        buffer_cnt[0] += payload.get_beat_count();
        phase = W_READY;
        return TLM_UPDATED;
    case W_VALID:
        phase = W_READY;
        return TLM_UPDATED;

    case AR_VALID:
        phase = AR_READY;
        outstand_cnt[0]++;
        if (outstand_cnt[0] < cfg_outstand) {

            ar_queue[0].push_back(&payload);            
            //payload.ref();
            return TLM_UPDATED;
        }
        else {
            //cout << "more read command !" << endl;
            ar_incoming[0] = &payload;
            return TLM_ACCEPTED;
        }
    case R_READY:
        //r_state = ACK;
        return TLM_ACCEPTED;
    case RACK:   /* cannot understand why need this ack flow */
    case WACK:
        return TLM_ACCEPTED;
    default:
        sc_assert(!"Unrecognised phase");
        return TLM_ACCEPTED;
    }
}
tlm_sync_enum vbus::nb_transport_fwb(Payload& payload, Phase& phase)
{
    switch (phase)
    {
    case AW_VALID:
        phase = AW_READY;

        outstand_cnt[1]++;
        if ((outstand_cnt[1] < cfg_outstand) && (buffer_cnt[1] < cfg_buffer)) {
            aw_queue[1].push_back(&payload);
            //payload.ref();

            return TLM_UPDATED;
        }
        else {
            //cout << "more write command !" << endl;
            aw_incoming[1] = &payload;
            return TLM_ACCEPTED;
        }

    case W_VALID_LAST:
        w_queue[1].push_back(&payload); //Need to know target id;
        //payload.ref();
        buffer_cnt[1] += payload.get_beat_count();
        phase = W_READY;
        return TLM_UPDATED;
    case W_VALID:
        phase = W_READY;
        return TLM_UPDATED;

    case AR_VALID:
        phase = AR_READY;
        outstand_cnt[1]++;
        if (outstand_cnt[1] < cfg_outstand) {

            ar_queue[1].push_back(&payload);
            //payload.ref();
            return TLM_UPDATED;
        }
        else {
            //cout << "more read command !" << endl;
            ar_incoming[1] = &payload;
            return TLM_ACCEPTED;
        }
    case R_READY:
        //r_state = ACK;
        return TLM_ACCEPTED;
    case RACK:   /* cannot understand why need this ack flow */
    case WACK:
        return TLM_ACCEPTED;
    default:
        sc_assert(!"Unrecognised phase");
        return TLM_ACCEPTED;
    }
}
tlm_sync_enum vbus::nb_transport_fwc(Payload& payload, Phase& phase)
{
    switch (phase)
    {
    case AW_VALID:
        phase = AW_READY;

        outstand_cnt[2]++;
        if ((outstand_cnt[2] < cfg_outstand) && (buffer_cnt[2] < cfg_buffer)) {
            aw_queue[2].push_back(&payload);
            //payload.ref();

            return TLM_UPDATED;
        }
        else {
            //cout << "more write command !" << endl;
            aw_incoming[2] = &payload;
            return TLM_ACCEPTED;
        }

    case W_VALID_LAST:
        w_queue[2].push_back(&payload); //Need to know target id;
        //payload.ref();
        buffer_cnt[2] += payload.get_beat_count();
        phase = W_READY;
        return TLM_UPDATED;
    case W_VALID:
        phase = W_READY;
        return TLM_UPDATED;

    case AR_VALID:
        phase = AR_READY;
        outstand_cnt[2]++;
        if (outstand_cnt[2] < cfg_outstand) {

            ar_queue[2].push_back(&payload);
            //payload.ref();
            return TLM_UPDATED;
        }
        else {
            //cout << "more read command !" << endl;
            ar_incoming[2] = &payload;
            return TLM_ACCEPTED;
        }
    case R_READY:
        //r_state = ACK;
        return TLM_ACCEPTED;
    case RACK:   /* cannot understand why need this ack flow */
    case WACK:
        return TLM_ACCEPTED;
    default:
        sc_assert(!"Unrecognised phase");
        return TLM_ACCEPTED;
    }
}
tlm_sync_enum vbus::nb_transport_fwd(Payload& payload, Phase& phase)
{
    switch (phase)
    {
    case AW_VALID:
        phase = AW_READY;

        outstand_cnt[3]++;
        if ((outstand_cnt[3] < cfg_outstand) && (buffer_cnt[3] < cfg_buffer)) {
            aw_queue[3].push_back(&payload);
            //payload.ref();

            return TLM_UPDATED;
        }
        else {
            //cout << "more write command !" << endl;
            aw_incoming[3] = &payload;
            return TLM_ACCEPTED;
        }

    case W_VALID_LAST:
        w_queue[3].push_back(&payload); //Need to know target id;
        //payload.ref();
        buffer_cnt[3] += payload.get_beat_count();
        phase = W_READY;
        return TLM_UPDATED;
    case W_VALID:
        phase = W_READY;
        return TLM_UPDATED;

    case AR_VALID:
        phase = AR_READY;
        outstand_cnt[3]++;
        if (outstand_cnt[3] < cfg_outstand) {

            ar_queue[3].push_back(&payload);
            //payload.ref();
            return TLM_UPDATED;
        }
        else {
            //cout << "more read command !" << endl;
            ar_incoming[3] = &payload;
            return TLM_ACCEPTED;
        }
    case R_READY:
        //r_state = ACK;
        return TLM_ACCEPTED;
    case RACK:   /* cannot understand why need this ack flow */
    case WACK:
        return TLM_ACCEPTED;
    default:
        sc_assert(!"Unrecognised phase");
        return TLM_ACCEPTED;
    }
}

tlm_sync_enum vbus::nb_transport_bwa(Payload& payload, Phase& phase)
{
    Phase prev_phase = phase;    
    tlm_sync_enum reply;

    switch (phase)
    {
    case AW_READY:
        aw_state[0] = ACK;
        return TLM_ACCEPTED;
    case W_READY:
        w_state[0] = ACK;
        return TLM_ACCEPTED;
    case AR_READY:
        ar_state[0] = ACK;
        return TLM_ACCEPTED;
    case B_VALID:
        //something new
        s_b_queue[0].push_back(&payload);
        phase = B_READY;
        return TLM_UPDATED;

    case R_VALID_LAST:
        s_r_queue[0].push_back(&payload);
    case R_VALID:    
        phase = R_READY;
        return TLM_UPDATED;
    default:
        sc_assert(!"Unrecognised phase");
        return TLM_ACCEPTED;
    }
}
tlm_sync_enum vbus::nb_transport_bwb(Payload& payload, Phase& phase)
{
    Phase prev_phase = phase;
    tlm_sync_enum reply;

    switch (phase)
    {
    case AW_READY:
        aw_state[1] = ACK;
        return TLM_ACCEPTED;
    case W_READY:
        w_state[1] = ACK;
        return TLM_ACCEPTED;
    case AR_READY:
        ar_state[1] = ACK;
        return TLM_ACCEPTED;
    case B_VALID:
        //something new
        s_b_queue[1].push_back(&payload);
        phase = B_READY;
        return TLM_UPDATED;
    case R_VALID_LAST:
        s_r_queue[1].push_back(&payload);
    case R_VALID:    
        phase = R_READY;
        return TLM_UPDATED;
    default:
        sc_assert(!"Unrecognised phase");
        return TLM_ACCEPTED;
    }
}
tlm_sync_enum vbus::nb_transport_bwc(Payload& payload, Phase& phase)
{
    Phase prev_phase = phase;
    tlm_sync_enum reply;

    switch (phase)
    {
    case AW_READY:
        aw_state[2] = ACK;
        return TLM_ACCEPTED;
    case W_READY:
        w_state[2] = ACK;
        return TLM_ACCEPTED;
    case AR_READY:
        ar_state[2] = ACK;
        return TLM_ACCEPTED;
    case B_VALID:
        //something new
        s_b_queue[2].push_back(&payload);
        phase = B_READY;
        return TLM_UPDATED;
    case R_VALID_LAST:
        s_r_queue[2].push_back(&payload);
    case R_VALID:
        phase = R_READY;
        return TLM_UPDATED;
    default:
        sc_assert(!"Unrecognised phase");
        return TLM_ACCEPTED;
    }
}
tlm_sync_enum vbus::nb_transport_bwd(Payload& payload, Phase& phase)
{
    Phase prev_phase = phase;
    tlm_sync_enum reply;

    switch (phase)
    {
    case AW_READY:
        aw_state[3] = ACK;
        return TLM_ACCEPTED;
    case W_READY:
        w_state[3] = ACK;
        return TLM_ACCEPTED;
    case AR_READY:
        ar_state[3] = ACK;
        return TLM_ACCEPTED;
    case B_VALID:
        //something new
        s_b_queue[3].push_back(&payload);
        phase = B_READY;
        return TLM_UPDATED;
    case R_VALID_LAST:
        s_r_queue[3].push_back(&payload);
    case R_VALID:
        phase = R_READY;
        return TLM_UPDATED;
    default:
        sc_assert(!"Unrecognised phase");
        return TLM_ACCEPTED;
    }
}

vbus::vbus(sc_module_name name, unsigned port_width, string cfgfile) :
    sc_module(name),  
    slave_a("slave_0", *this, &vbus::nb_transport_fwa, ARM::TLM::PROTOCOL_ACE,
        port_width),
    slave_b("slave_1", *this, &vbus::nb_transport_fwb, ARM::TLM::PROTOCOL_ACE,
        port_width),
    slave_c("slave_2", *this, &vbus::nb_transport_fwc, ARM::TLM::PROTOCOL_ACE,
        port_width),
    slave_d("slave_3", *this, &vbus::nb_transport_fwd, ARM::TLM::PROTOCOL_ACE,
        port_width),
    master_a("master_0", *this, &vbus::nb_transport_bwa, ARM::TLM::PROTOCOL_ACE,
        port_width),
    master_b("master_1", *this, &vbus::nb_transport_bwb, ARM::TLM::PROTOCOL_ACE,
        port_width),
    master_c("master_2", *this, &vbus::nb_transport_bwc, ARM::TLM::PROTOCOL_ACE,
        port_width),
    master_d("master_3", *this, &vbus::nb_transport_bwd, ARM::TLM::PROTOCOL_ACE,
        port_width),
    clock("clock")
{
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 4; j++) {
            sch_rslt_ptr[i][j] = 0;
        }
    }
    for (int i = 0; i < 4; i++) {
        aw_state[i] = CLEAR;
         w_state[i] = CLEAR;
        ar_state[i] = CLEAR;
         b_state[i] = CLEAR;
         r_state[i] = CLEAR;

        w_beat_count[i] = 0;
        r_beat_count[i] = 0;
        outstand_cnt[i] = 0;
        buffer_cnt[i]   = 0;

        ar_incoming[i] = NULL;
        aw_incoming[i] = NULL;

        b_outgoing[i] = NULL;
        r_outgoing[i] = NULL;
    } 

    sch_mode = 2;
    cfg_outstand = 5;

    // config  
    read_config(cfgfile); 

    SC_METHOD(clock_posedge);
    sensitive << clock.pos();
    dont_initialize();

    SC_METHOD(clock_negedge);
    sensitive << clock.neg();
    dont_initialize();

    ostringstream filename;
    filename << name << "_master_sta.log";
    log_master.open(filename.str().c_str());
    // stat
    //sta_aw_que = new st_depth("mst0 aw");
    //sta_w_que0  = new st_depth("mst0  w");
    //sta_ar_que0 = new st_depth("mst0 ar"); 
}

vbus::~vbus()
{

}
//read config info
void vbus::read_config(string cfgname) {
    ptree pt1;
    read_xml(cfgname.c_str(), pt1, xml_parser::trim_whitespace);   //����һ��xml�ļ�

    ptree child = pt1.get_child("root.vbus");

    //this->sch_mode = child.get<int>("<xmlattr>.mode", 2000);
    this->cfg_outstand = child.get<int>("<xmlattr>.outstand", 1);
    this->cfg_buffer = child.get<int>("<xmlattr>.buffer", 128);
    this->cfg_queue = child.get<int>("<xmlattr>.queue", 128);

    child = pt1.get_child("root.vbus.sch");
    this->sch_mode = child.get<int>("<xmlattr>.mode", 1);
}

void vbus::st_print() {
    //log_master <<sta_aw_que0->phase_head.str()
    //    <<sta_aw_que0->report()
    //    <<sta_aw_que1->report()
    //    <<sta_ar_que0->report()
    //    <<sta_ar_que1->report()
    //    <<endl;
}

void vbus::switch_req() {

    //1st stage
    for (int i = 0; i < 4; i++) {
        if (!aw_queue[i].empty() && !w_queue[i].empty()) {
            ARM::AXI4::Payload* payload =  aw_queue[i].front();
            int target_id = payload->region;

            if (payload->qos == 0)
            {
                int beats = aw_queue[i].front()->get_beat_count();
                sw_aw_h_queue[target_id].push_back(payload);
                aw_queue[i].pop_front();

                sw_w_h_queue[target_id].push_back(w_queue[i].front());
                w_queue[i].pop_front();

                outstand_cnt[i]--;
                buffer_cnt[i] -= beats;
            }
            else {                
                sw_aw_queue[i][target_id].push_back(payload);
                aw_queue[i].pop_front();

                sw_w_queue[i][target_id].push_back(w_queue[i].front());
                w_queue[i].pop_front();
            }           
        }

        if (!ar_queue[i].empty()) {
            ARM::AXI4::Payload* payload = ar_queue[i].front();
            int target_id = payload->region;
            if (payload->qos == 0) {
                int beats = ar_queue[i].front()->get_beat_count();
                sw_ar_h_queue[target_id].push_back(payload);
                ar_queue[i].pop_front();

                outstand_cnt[i]--;
                buffer_cnt[i] -= beats;
            }
            else
            {
                sw_ar_queue[i][target_id].push_back(payload);
                ar_queue[i].pop_front();
            }            
        }
    }

    //2nd stage
    for (int i = 0; i < 4; i++) {
        int sch_rslt = 4;
        
        if (!sw_aw_h_queue[i].empty()) {
            s_aw_queue[i].push_back(sw_aw_h_queue[i].front());
            sw_aw_h_queue[i].pop_front();

            s_w_queue[i].push_back(sw_w_h_queue[i].front());
            sw_w_h_queue[i].pop_front();
        }
        else {
            sch_rslt = rr_sch(sch_rslt_ptr[0][i], !sw_aw_queue[0][i].empty(),
                !sw_aw_queue[1][i].empty(), !sw_aw_queue[2][i].empty(), !sw_aw_queue[3][i].empty());

            if (sch_rslt < 4) {
                int beats = sw_aw_queue[sch_rslt][i].front()->get_beat_count();
                s_aw_queue[i].push_back(sw_aw_queue[sch_rslt][i].front());
                sw_aw_queue[sch_rslt][i].pop_front();

                s_w_queue[i].push_back(sw_w_queue[sch_rslt][i].front());
                sw_w_queue[sch_rslt][i].pop_front();

                outstand_cnt[sch_rslt]--;
                buffer_cnt[sch_rslt] -= beats;

                sch_rslt_ptr[0][i] = sch_rslt;
            }
        }
       
        if (!sw_ar_h_queue[i].empty())
        {
            s_ar_queue[i].push_back(sw_ar_h_queue[i].front());
            sw_ar_h_queue[i].pop_front();            
        }
        else {
            sch_rslt = rr_sch(sch_rslt_ptr[1][i], !sw_ar_queue[0][i].empty(),
                !sw_ar_queue[1][i].empty(), !sw_ar_queue[2][i].empty(), !sw_ar_queue[3][i].empty());

            if (sch_rslt < 4) {
                s_ar_queue[i].push_back(sw_ar_queue[sch_rslt][i].front());
                sw_ar_queue[sch_rslt][i].pop_front();

                outstand_cnt[sch_rslt]--;
                sch_rslt_ptr[1][i] = sch_rslt;
            }
        }        
    }
}
/* no cycle accurate */
void vbus::switch_resp()
{
    for (int i = 0; i < 4; i++) {
        if (!s_b_queue[i].empty()) {
            int mst_id = s_b_queue[i].front()->user;
            b_queue[mst_id].push_back(s_b_queue[i].front());
            s_b_queue[i].pop_front();
        }

        if (!s_r_queue[i].empty()) {
            int mst_id = s_r_queue[i].front()->user;
            r_queue[mst_id].push_back(s_r_queue[i].front());
            s_r_queue[i].pop_front();
        }
    }
}

/* sch simple */
int vbus::rr_sch(int last_ptr, bool val_0, bool val_1, bool val_2, bool val_3) {
    int  sch_rslt = 4;
    bool sch_ok = false;

    bool val_array[4];
    val_array[0] = val_0;
    val_array[1] = val_1;
    val_array[2] = val_2;
    val_array[3] = val_3;

    for (int i = 0; i < 4; i++) {
        int real_ptr = 0;
        if ((last_ptr + 1 + i) >= 4) {
            real_ptr = last_ptr + 1 + i - 4;
        }
        else {
            real_ptr = last_ptr + 1 + i;
        }

        if ((val_array[real_ptr])&&(!sch_ok))
        {
            sch_rslt = real_ptr;
            sch_ok = true;
        }
    }

    if ((val_0 || val_1 || val_2 || val_3) && (sch_rslt == 4)) {
        cout << "wrong sch result !" << endl;
    }

    return sch_rslt;
}


