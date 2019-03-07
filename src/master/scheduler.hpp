


#include "master.hpp"

using namespace chameleon::master;

namespace chameleon{
    namespace scheduler{

        class Scheduler{
        public:
            explicit Scheduler() {}

            virtual  ~Scheduler() {}

            virtual void initialize();

            Slave* find_slave_to_run_framework();

        private:
            Scheduler(const Scheduler&);
            Scheduler &operator=(const Scheduler &);

            Master* m_master;

            double  m_min_use_rate;
            Master::Slaves m_slaves;
            process::UPID m_slave_pid;

            RuntimeResourcesMessage m_runtime_info;

            hashmap<UPID, RuntimeResourcesMessage> m_slave_usage;
        };

    }
}