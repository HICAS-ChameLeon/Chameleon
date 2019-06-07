

# # test<-list(130,37,11,36285,1865,92,1,107,
#            876,1,0,1,30,89,1,123,124,0
#            ,1,0.8482439490352061,0.6382083374084615,0,784,68,23,33,448,331,8,15,1,89,3.489364557076994,0.07325905005015731,8,50
# )



library(gbm)
library(caret)

nt<<-8000
test <- read.table("D:/asplos/time_conf_dasize1184.txt",head=FALSE)
# test <- test[,-1]
pre_fuc<-function(){
  # load model
  HModel <-load(file = "HModle.RData")
  ptest <- test
  tmc <-get(HModel)
  pframe<-data.frame(ptest)
  # # print(pframe)
  pre_res <-predict(tmc,pframe[,-1],nt)
   print("---------------")
  print(pre_res)
  # # 
  # # err<-sum(abs(as.data.frame(pre_res)-test[,1])/test[,1])/nrow(test)
  # # accuracy<-err
  # # print(accuracy)
  # print(mode(tmc))
  # print(mode(pframe))
}

pre_fuc()