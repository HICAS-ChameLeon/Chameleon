/**
 * Copyright  ：深圳先进研究所异构智能计算体系结构与系统研究中心
 * Author     ：weiguo
 * Date       ：18-12-4
 * Description：Get TLB information from computer
 */

// GTEST DEPENDENCIES
#include <gtest/gtest.h>

// STOUT DEPENDENCIES

#include <resource_collector/tlb_collector.hpp>

TEST(TLBCollectorTest, Test) {
    chameleon::TLBCollector tlb;
    TLBCollection *tlbCollection = tlb.get_tlb_info();
    for (auto i = tlbCollection->tlb_infos().begin(); i != tlbCollection->tlb_infos().end(); i++) {
        for (int j = 0; j < i->l1_data_tlb().size(); j++) {
            LOG(INFO) << i->l1_data_tlb(j);
        }
        for (int k = 0; k < i->l1_instruction_tlb().size(); k++) {
            LOG(INFO) << i->l1_instruction_tlb(k);
        }
        LOG(INFO) << i->l2_tlb() ;
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

