
#library(gbm)
# library(lattice)
# library(ggplot2)

library(caret)

nt<<-8000
test <- read.table("time_conf_dasize1184.txt",head=FALSE)
# test <- test[,-1]
pre_fuc<-function(){
  # load model
  HModel <-load(file = "HModle.RData")
  ptest <- test
  tmc <-get(HModel)
  pframe<-as.data.frame(ptest)
  pre_res <-predict(tmc,pframe,nt)
  print("---------------")
  print(pre_res)
  # 
  # err<-sum(abs(as.data.frame(pre_res)-test[,1])/test[,1])/nrow(test)
  # accuracy<-err
  # print(accuracy)
  print(mode(tmc))
  print(mode(ptest))
}

pre_fuc()