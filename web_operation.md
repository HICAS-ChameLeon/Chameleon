##控制模块 > 节点拓扑图的操作注意

###1. 开启Supermaster
 1. 点击“开启supermaster”按钮，会弹出“确认开启Supermaster?”的提醒
   
    * 选择“确认”可以成功开启supermaster
   
    * 选择“取消”则不会开启supermaster
   
 2. supermaster成功启动后，再次点击“开启supermaster”按钮，会弹出“supermaster已经开启”提醒
   
####注意:
 * 成功开启supermaster后，不要刷新页面。当刷新后再次点击“开启supermaster”按钮，即使已经开启了supermaster，仍会弹出提醒“确认开启Supermaster?”
    
    并且即使选择“确定”，实际上也并不会有什么效果。这个bug还尚待解决。
    
***
###2.查看Supermaster的拓扑图
将地址栏的端口号改为7000，才可以查看拓扑图

点击“节点拓扑图”标签，可以查看master节点的拓扑图

左侧“选择层数”按钮，选择“二层”，可以查看supermaster的拓扑图

####注意:
 * 点击“开启supermaster”按钮，会弹出提醒“确认开启Supermaster?”。逻辑上此时supermaster已经开启，是不应该弹出这个提醒的。
  
   之后可能会在7000端口页面将这个按钮取消. 
***
###3. 关闭Supermaster
 1. 在7000端口页面，点击“关闭supermaster”按钮，弹出提醒“确认停止Supermaster?" 
 
    * 选择“确认”，关闭Supermaster
    
    * 选择“取消”，不会关闭Supermaster
 2. 再当前页面再次点击“关闭supermaster”按钮，会提醒“supermaster已经关闭”
 
####注意:
 * 关闭supermaster后，7000端口页面已无法连接。打算关闭supermaster后直接跳转到空白页面;
 
 * 关闭supermaster后无法在6060端口页面重新开启supermaster，虽然选择确认开启supermaster后，也会提醒supermaster已经开启，但实际上并没有开启supermaster
 
 * 关闭Supermaster后，在后台需要手动杀死Supermaster和Master
  ```bash
   sudo kill -9 `ps -ef|grep "master" |grep -v grep|awk '{print $2}'`
  ```