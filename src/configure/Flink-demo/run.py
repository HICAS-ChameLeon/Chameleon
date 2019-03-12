from __future__ import print_function
import collectingData, util

# flink_home = util.getFlinkHome()
slaves = util.getSlaves()

if __name__ == '__main__':
    for i in range(1):
        confValue = collectingData.makeRandomConfig()
        # collectingData.write2Configure(confValue)
        collectingData.collectData('./data/initData.csv', slaves, confValue)
