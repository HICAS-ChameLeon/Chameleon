
source("brt.functions.R")
library(gbm)
library(caret)

# test: create a matrix
dataset<-read.table("D:/dacdata/modelinput/20-30-40-50-60.txt",head=FALSE)
# dataset<-read.table("test.txt",head=FALSE)


# split dataset to 'train' and 'test'
tmpset<-createDataPartition(y=dataset$V36,p=0.8,list=FALSE)
train<-dataset[tmpset, ]  #80%的dataset数据作为训练数据
test<-dataset[-tmpset, ]  #20%的dataset数据作为测试数据
print(nrow(train))
print(nrow(test))



S<<-train
tc<-5
nt<<-5000
lr<<-0.005


# functon: pre_func
# load model
# HModel <-load(file = "HModle.RData")

# predict the test
# pre_fuc<-function(){
#   ptest <- test
#   pre_res <- predict(get(HModel),ptest,nt)
#   print("---------------")
#   print(mode(get(HModel)))
#   print(mode(ptest))
# }

# pre_fuc()



################################################
# Function: 
#       HM(S,tc,nt,lr);
#       HigherOrderProcedure(order)
#       HigherOrderProcedure(S)
################################################


HM<-function(S,tc,nt,lr){
  print("oh my god...lalalla")
  tmc<-list()
  accuracy<-9
  order<-1
  while(accuracy>=0.1){
    print("start...")
    print(accuracy)
    if(order==1){
      result<-FirstOrderProcedure(S)
      TM<-result[[1]]
      accuracy<-result[[2]]
      print(accuracy)
      tmc<-c(tmc,list(TM))
    }else{
      
      #TM<-TM*lr+HigherOrderProcedure(order-1)*lr
      tmc<-c(tmc,HigherOrderProcedure(order-1))
      ml<-length(tmc)
      result<-0
      for(i in 1:ml){
        result<-result+predict(tmc[[i]],test,nt)
        # result<-result+predict(tmc[[i]],S,nt)
      }#end for
      
      # accuracy<-sum((result-test[,1])/test[,1])/nrow(test)
      accuracy<-sum(abs(result-test[,1])/test[,1])/nrow(test)
      cat("order is:",order,"accuracy is",accuracy)
    }#end else
    
    order<-order+1
  }#end while
  
  #FM<-TM
  print("the acc is:")
  print(accuracy)
  return(tmc)
}#end HM func


HigherOrderProcedure<-function(order){
  if(order==1){
    TM<-FirstOrderProcedure(S)
    tmc<-list(TM[[1]])
  }else{
    TM1<-HigherOrderProcedure(order-1)
    TM2<-HigherOrderProcedure(order-1)

     #TM<-TM1*lr+TM2*lr
    tmc<-c(tmc,TM1,TM2)

  }#end else

  return(tmc)


}#end HigherOrderfunc


FirstOrderProcedure<-function(S){

  #sn<-sample(nrow(pr),nrow(pr)*0.8)
  #S<-pr[sn,]
  TM<-gbm.fixed(data=S,gbm.x=2:ncol(S),gbm.y=1,family="laplace",bag.fraction=0.75,learning.rate=lr,tree.complexity=tc,n.trees=nt)
  pred<-predict(TM,test,nt)
  # print("TM-----------")
  # print(mode(TM))
  # print(mode(test))
  residual<-predict(TM,S,nt)-S[,1]
  S<<-data.frame(residual=residual,S[,-1])
  err<-sum(abs(pred-test[,1])/test[,1])/nrow(test)
  accuracy<-err
  # print(accuracy)
  return(list(TM,accuracy))

}#end FirstOrderfunc

tmc<-HM(S,tc,nt,lr)
# save model
save(tmc,file="D:/dacdata/modelout/HModle.RData")

print("end of programe")

