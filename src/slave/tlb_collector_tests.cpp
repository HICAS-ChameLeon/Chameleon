/**
 * Copyright  ：深圳先进研究所异构智能计算体系结构与系统研究中心
 * Author     ：weiguo
 * Date       ：18-12-4
 * Description：Get TLB information from computer
 */

// GTEST DEPENDENCIES
#include <gtest/gtest.h>
#include <gmock/gmock.h>

// STOUT DEPENDENCIES
#include <stout/gtest.hpp>

#include <tlb_collector.hpp>

TEST(TLBCollectorTest,Test){
    chameleon::TLBCollector tlb;
    TLBCollection* tlbCollection = tlb.get_tlb_info();
    for(int i=0; i < tlbCollection->tlb_infos().l1_data_tlb().size();i++){
        Option<string> l1_data_tlb =  tlbCollection->mutable_tlb_infos()->l1_data_tlb(i);
        EXPECT_SOME(l1_data_tlb);
    }
    for(int j = 0;j<tlbCollection->tlb_infos().l1_instruction_tlb().size();j++){
        Option<string> l1_instruction_tlb = tlbCollection->mutable_tlb_infos()->l1_instruction_tlb(j);
        EXPECT_SOME(l1_instruction_tlb);
    }
    Option<string> l2_tlb = tlbCollection->tlb_infos().l2_tlb();
    EXPECT_SOME(l2_tlb);
}

int main(int argc,char** argv){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}

