/**
 * Copyright  ：深圳先进研究所异构智能计算体系结构与系统研究中心
 * Author     ：weiguo
 * Date       ：18-12-4
 * Description：Get TLB information from computer
 */
#include <tlb_collector.hpp>

namespace chameleon {
    TLBCollection *TLBCollector::get_tlb_info() {
        TLBInfo* ti = new TLBInfo;
        TLBCollection *tlbCollection = new TLBCollection();

        Option<string> l1_data_tlb;
        Option<string> l1_instruction_tlb;
        Option<string> l2_tlb;

        Try<Subprocess> cache = subprocess(
                "cpuid -1 | grep TLB",
                Subprocess::FD(STDIN_FILENO),
                Subprocess::PIPE(),
                Subprocess::FD(STDIN_FILENO)
        );

        process::Future<string> ganied = process::io::read(cache.get().out().get());

        vector<string> tlb = strings::split(ganied.get(), "\n");

        for (int i = 0; i < tlb.size() - 1; i++) {
            vector<string> vec = strings::tokenize(tlb[i], "TLB");
            vector<string> vec_front = strings::split(vec.front(), ":");
            vector<string> vec_back = strings::split(vec.back(), ":");
            string data = strings::trim(vec_front.back());

            if (data == "data") {
                l1_data_tlb = vec_back.back();
                ti->add_l1_data_tlb(l1_data_tlb.get());
                continue;
            }
            if (data == "instruction") {
                l1_instruction_tlb = vec_back.back();
                ti->add_l1_instruction_tlb(l1_instruction_tlb.get());
                continue;
            }
            if (data == "") {
                if (vec_back.back() != "") {
                    l2_tlb = vec_back.back();
                    ti->set_l2_tlb(l2_tlb.get());
                }
                continue;
            }
        }
        tlbCollection->set_allocated_tlb_infos(ti);
        return tlbCollection;
    };
}
