更改spark executor 运行的数量
到 ~/open-source/spark-2.3.0 目录下，vim conf/spark-defaults.conf
如：
```shell
spark.executor.cores   1
spark.executor.memory  600m
spark.cores.max        4
```
则需要运行的spark-executor 数量=spark.cores.max/spark.executor.cores = 4/1 = 4

16384m = 16g
11000M 实际上需要 12000 m
./bin/spark-submit \
--class org.apache.spark.examples.JavaSparkPi \
--master mesos://172.20.110.228:6060 \
--deploy-mode client \
--supervise \
--executor-memory 11000m /home/lemaker/open-source/spark-2.3.0-bin-hadoop2.7/examples/jars/spark-examples_2.11-2.3.0.jar 100