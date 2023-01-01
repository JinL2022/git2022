#include <time.h>

#include <common/utils_include.h>
#include <common/utils_mon.h>
#include <common/utils_cfg.h>

#include <vcpu/vcpu.h>
#include <vbus/vbus.h>
#include <vmem/vmem.h>

using namespace std;
using namespace sc_core;
using namespace ARM::AXI4;
/*
void add_payloads_to_tg(vcpu& tg)
{
    tg.add_payload(COMMAND_READ,  0x00003000, SIZE_16, 3);
    tg.add_payload(COMMAND_WRITE, 0x00006000, SIZE_16, 15);
   //tg.add_payload(COMMAND_READ,  0x00002000, SIZE_16, 3);
   //tg.add_payload(COMMAND_WRITE, 0x00005000, SIZE_16, 3);
   //tg.add_payload(COMMAND_READ,  0x00003000, SIZE_16, 3);
   //tg.add_payload(COMMAND_WRITE, 0x00004000, SIZE_16, 3);
   //tg.add_payload(COMMAND_READ,  0x00004000, SIZE_16, 3);
   //tg.add_payload(COMMAND_WRITE, 0x00003000, SIZE_16, 3);
   //tg.add_payload(COMMAND_READ,  0x00005000, SIZE_16, 3);
   //tg.add_payload(COMMAND_WRITE, 0x00002000, SIZE_16, 3);
   //tg.add_payload(COMMAND_READ,  0x00006000, SIZE_16, 3);
   //tg.add_payload(COMMAND_WRITE, 0x00001000, SIZE_16, 3);
   //tg.add_payload(COMMAND_WRITE, 0x00001010, SIZE_16, 1);
   //tg.add_payload(COMMAND_WRITE, 0x00001020, SIZE_16, 1);
   //tg.add_payload(COMMAND_WRITE, 0x00001030, SIZE_16, 4);
}
*/
/*
void add_payloads_to_tgb(vcpu& tg)
{
    tg.add_payload(COMMAND_WRITE, 0x00000600, SIZE_16, 3);
    tg.add_payload(COMMAND_WRITE, 0x00000700, SIZE_16, 31);
    tg.add_payload(COMMAND_READ, 0x00000100, SIZE_16, 3);
    tg.add_payload(COMMAND_READ, 0x00000200, SIZE_16, 3);
    tg.add_payload(COMMAND_READ, 0x00000300, SIZE_16, 3);
    tg.add_payload(COMMAND_READ, 0x00000400, SIZE_16, 3);
    tg.add_payload(COMMAND_READ, 0x00000500, SIZE_16, 3);
  
    tg.add_payload(COMMAND_READ, 0x00000600, SIZE_16, 3);
    tg.add_payload(COMMAND_READ, 0x00000700, SIZE_16, 3);
    tg.add_payload(COMMAND_READ, 0x00000800, SIZE_16, 3);
   
}
*/
int sc_main(int argc, char* argv[])
{
    //clock define
    sc_clock clk_1("clk0", 0.5, SC_NS, 0.5);    // 1GHz
    sc_clock clk_2("clk1", 0.5, SC_NS, 0.5);    // 800MHz
    sc_clock clk_3("clk2", 0.5, SC_NS, 0.5);    // 500MHz
    sc_clock clk_4("clk3", 0.5, SC_NS, 0.5);    // 1500MHz
    sc_clock clk_5("clk4", 0.5, SC_NS, 0.5);    // 1200MHz

    vcpu virtual_cpuss("cpuss", "conf.xml", 0);
    vcpu virtual_npuss("npuss", "conf.xml", 1);
    vcpu virtual_apuss("apuss", "conf.xml", 2);
    vcpu virtual_synoc("synoc", "conf.xml", 3);

    vbus virtual_bus("vbus", 128, "conf.xml");

    vmem virtual_mem_0("vmem_0", "conf.xml", 0);
    vmem virtual_mem_1("vmem_1", "conf.xml", 1);
    vmem virtual_mem_2("vmem_2", "conf.xml", 2);
    vmem virtual_mem_3("vmem_3", "conf.xml", 3);

    utils_mon mon_0_0("mon_r0", 128);
    utils_mon mon_0_1("mon_r1", 128);
    utils_mon mon_0_2("mon_r2", 128);
    utils_mon mon_0_3("mon_r3", 128);

    utils_mon mon_1_0("mon_s0", 128);
    utils_mon mon_1_1("mon_s1", 128);
    utils_mon mon_1_2("mon_s2", 128);
    utils_mon mon_1_3("mon_s3", 128);

    utils_config top_config("conf.xml");

    virtual_cpuss.clock.bind(clk_1);
    virtual_npuss.clock.bind(clk_4);
    virtual_apuss.clock.bind(clk_5);
    virtual_synoc.clock.bind(clk_3);

    virtual_bus.clock.bind(clk_4);
    virtual_mem_0.clock.bind(clk_1);
    virtual_mem_1.clock.bind(clk_1);
    virtual_mem_2.clock.bind(clk_1);
    virtual_mem_3.clock.bind(clk_1);

    virtual_cpuss.master.bind(mon_0_0.slave);
    mon_0_0.master.bind(virtual_bus.slave_a);

    virtual_npuss.master.bind(mon_0_1.slave);
    mon_0_1.master.bind(virtual_bus.slave_b);

    virtual_apuss.master.bind(mon_0_2.slave);
    mon_0_2.master.bind(virtual_bus.slave_c);

    virtual_synoc.master.bind(mon_0_3.slave);
    mon_0_3.master.bind(virtual_bus.slave_d);

    //virtual_bus.master.bind(virtual_mem.slave);
    virtual_bus.master_a.bind(mon_1_0.slave);
    mon_1_0.master.bind(virtual_mem_0.slave);

    virtual_bus.master_b.bind(mon_1_1.slave);
    mon_1_1.master.bind(virtual_mem_1.slave);

    virtual_bus.master_c.bind(mon_1_2.slave);
    mon_1_2.master.bind(virtual_mem_2.slave);

    virtual_bus.master_d.bind(mon_1_3.slave);
    mon_1_3.master.bind(virtual_mem_3.slave);
    //add_payloads_to_tg(virtual_cpu_a);
    //add_payloads_to_tgb(virtual_cpu_b);

    int current_cnt = 0;

    // more simulation control
    while (current_cnt < top_config.total_time) {
        sc_start(top_config.phase_time, top_config.sim_unit);
        mon_0_0.st_print();
        mon_0_1.st_print();
        mon_0_2.st_print();
        mon_0_3.st_print();

        mon_1_0.st_print();
        mon_1_1.st_print();
        mon_1_2.st_print();
        mon_1_3.st_print();

        virtual_bus.st_print();
        sc_time current_time = sc_time_stamp();

        current_cnt += top_config.phase_time;
        cout <<"run simulation :" << ((current_cnt * 100) / top_config.total_time)
             << "%      real time:" <<setw(8) << current_time.to_string() << "\r";
    }
        
    //sc_start(top_config.total_time, top_config.sim_unit);
    cout << endl << endl;
    Payload::debug_payload_pool(cout);


    //获取系统时间  
    time_t now_time = time(NULL);
    //获取本地时间  
    tm* t_tm = localtime( &now_time );
    //转换为年月日星期时分秒结果，如图：  
    //printf("local time is    : %s\n", asctime(t_tm)); 
    cout << "Simulation End !!      " <<setw(20)<<asctime(t_tm) << endl;

    return 0;
}

