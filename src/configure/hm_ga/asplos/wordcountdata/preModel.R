

library(gbm)
library(caret)



nt<<-3600

# load model
HModel <-load(file = "D:/asplos/HModle.RData")
tmc <-get(HModel)

# test <- test[,-1]
preFuc<-function(inList){
  
  test<-inList
  test<-c(unlist(test))
  test<-data.frame(t(test))
  names(test)<-c("V1","V2","V3","V4","V5","V6","V7","V8"
                 ,"V9","V10","V11","V12","V13","V14","V15"
                 ,"V16","V17","V18","V19","V20","V21","V22"
                 ,"V23","V24","V25","V26","V27","V28","V29"
                 ,"V30","V31","V32","V33","V34","V35","V36")


  pre_res <-predict(tmc,test,nt)
  return (pre_res)
}
 

# pre_res<-preFuc()
#  print(pre_res)

# 
# inList <<-list(103.031,37,11,36285,1865,92,1,
#             107,876,1,0,1,30,89,1,123,124,
#             0,1,0.8482439490352061,
#             0.6382083374084615,0,784,68,23,
#             33,448,331,8,15,1,89,3.489364557076994,
#             0.07325905005015731,8,50)
# 
# addtest<-c(103.031,37,11,36285,1865,92,1,
#            107,876,1,0,1,30,89,1,123,124,
#            0,1,0.8482439490352061,
#            0.6382083374084615,0,784,68,23,
#            33,448,331,8,15,1,89,3.489364557076994,
#            0.07325905005015731,8,52)

# argtest <- function(inList){
#   print(inList)
#   inList<- data.frame(inList)
#   print(dim(inList))
#   
#   test <- list(1,2,3,4)
#   test<-data.matrix(test) # 结果同上
#   print(test)
#   test<-data.frame(t(test))
#   print(test)
#   names(test)<-c("V1","V2","V3")
#   print(test)
#   print(dim(test))
#   print(mode(test))
#   
# 
#   
#   return
# }

#argtest(inList)
 



#print("---------------")
# print(pre_res)
# print(dim(test))
# print(mode(pre_res))


#err<-sum(abs(as.data.frame(pre_res)-test[,1])/test[,1])/nrow(test)
#accuracy<-err
#print(accuracy)
# print(mode(tmc))
# print(mode(pframe))
 

# print(test)
# test<-rbind(test,addtest)
# print(dim(test))

#print(test)
#print(mode(test(V2))


#test <- read.table("D:/asplos/data/test1.txt",head=FALSE)
# test <-list(103.031,37,11,36285,1865,92,1,
#               107,876,1,0,1,30,89,1,123,124,
#               0,1,0.8482439490352061,
#               0.6382083374084615,0,784,68,23,
#               33,448,331,8,15,1,89,3.489364557076994,
#               0.07325905005015731,8,50)


# test <- data.matrix(test)
# print(test)
# print(dim(test))
 