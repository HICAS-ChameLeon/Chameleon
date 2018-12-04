/**
 * Copyright  ：深圳先进研究所异构智能计算体系结构与系统研究中心
 * Author     ：weiguo
 * Date       ：18-12-4
 * Description：Get TLB information from computer
 */

#include <tlb_collector.hpp>

int main(){
    chameleon::TLBCollector tlb;
    TLBCollection* tlbCollection = tlb.get_tlb_info();
    for(int i=0; i < tlbCollection->tlb_infos().l1_data_tlb().size();i++){
        cout << tlbCollection->mutable_tlb_infos()->l1_data_tlb(i) << endl;
    }
    for(int j = 0;j<tlbCollection->tlb_infos().l1_instruction_tlb().size();j++){
        cout << tlbCollection->mutable_tlb_infos()->l1_instruction_tlb(j) << endl;
    }
}
