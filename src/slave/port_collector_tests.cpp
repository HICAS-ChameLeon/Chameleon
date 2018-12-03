#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <port_collector.hpp>

TEST(PortCollectionTests, GetPortInfo)
{
    chameleon::PortCollector portCollector;
    PortCollection proto = *portCollector.split_port_string();
    PortInfo *index;
    cout<<"size : "<<proto.port_quantity()<<endl;
    
    for(int i;i<proto.port_quantity();i++) {
        index = proto.mutable_port_infos(i);
        cout<<"firstport["<<i<<"]: "<<index->firstport()<<endl;
        cout<<"lastport ["<<i<<"]: "<<index->lastport()<<endl;
    }
}

int main(int argc, char** argv) {
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
