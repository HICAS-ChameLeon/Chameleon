# FLink-demo(flink1.4)

### **目录结构：**
```shell
.
├── README
├── collectingData.py            获取性能指标和相应的配置
├── data                           
│   ├── best-conf.csv            存放性能top n的配置参数
│   ├── demo.csv                 训练数据样本
│   ├── intermediate.csv         用于GA搜索最优配置的初始解空间
│   ├── micro
│   │   ├── defConf
│   │   └── expertConf
│   └── models                   
│       └── Jmodel.pkl           性能分析模型
├── flink-conf.yaml              flink配置文件
├── gmExp.py                     GANs-based generator
├── isTask.sh
├── optimize.py                  搜索最优配置的脚本
├── requirement                  依赖库
├── run.py                       收集训练集的脚本
├── training.py                  训练performance model的脚本
└── util.py
```
---
### **Demo 执行过程：**
```shell
1): 安装Demo需要的依赖包 pip install -r requirement.txt
####  收集训练数据依托于flink集群环境，该步骤已跳过(python run.py / python gmExp.py)，训练数据存储于./data/demo.csv文件中；
2): 生成性能模型: python training.py, 保存模型于./data/models/Jmodel.pkl
3): 寻找最优配置参数组合: python optimize.py，输出得到的最优解（写到配置文件中）
```
---


#### **需要修改的文件访问路径,集群参数,ip如下：**
- /home/zyx/Chameleon/src/configure/Flink-demo/collectingData.py 
   
   224行: `os.system( 'nohup /home/zyx/open-source/flink-1.4.2/build-target/bin/mesos-appmaster.sh &')`
   
   229行: `os.system('scp ./flink-conf.yaml zyx@' + str(i) + ':/home/zyx/open-source/flink-1.4.2/build-target/conf')`
   
   248行: `os.system("ssh zyx@172.20.110.100 '/home/zyx/open-source/flink-1.4.2/build-target/bin/flink run /home/zyx/open-source/flink-1.4.2/build-target/examples/streaming/TopSpeedWindowing.jar'")`
   
- /home/zyx/Chameleon/src/configure/Flink-demo/util.py
 
   50,59,69行: `http://172.20.110.100:8081/joboverview/running`
   
   78行: `jID = os.popen("ssh zyx@172.20.110.100 '/home/zyx/open-source/flink-1.4.2/build-target/bin/flink list'")`
   
   84行: `s.system("ssh zyx@172.20.110.100 '/home/zyx/open-source/flink-1.4.2/build-target/bin/flink cancel " + jobid + "'")`
   
   87行:  `jobinfo = handle_html('http://172.20.110.100:8081/jobs/' + str(jobid))`