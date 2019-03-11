import numpy as np
import pandas as pd
import pickle
from sklearn import preprocessing
from sklearn import metrics
from sklearn.ensemble import GradientBoostingRegressor,RandomForestRegressor
from sklearn.linear_model import Ridge, LogisticRegression
from sklearn.model_selection import train_test_split,GridSearchCV,RandomizedSearchCV
import matplotlib.pyplot as plt
from sklearn.gaussian_process import GaussianProcessRegressor
from sklearn.gaussian_process.kernels import RBF, WhiteKernel
from sklearn.externals import joblib
from sklearn.svm import SVR

def runGBRT():
    algorithm1 = './data/initData.csv'
    data = pd.read_csv(algorithm1)

    fea = data.iloc[:, 1:-1]
    perf = data.iloc[:, [-1]]

    events_name = fea.columns


    X=np.array(fea)
    y=np.array(perf)
    # # # print(fea)
    scalerx = preprocessing.MinMaxScaler()
    X=scalerx.fit_transform(X)


    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=51)

    '''random forests'''
    rf={'n_estimators': 700}
    # clf=RandomForestRegressor(**rf)

    '''sgbrt'''
    params = {'n_estimators': 3000, 'max_depth': 18, 'min_samples_split': 2,
              'learning_rate': 0.01, 'loss': 'ls','subsample':0.75}  #latency
    clf = GradientBoostingRegressor(**params)

    '''SVM'''
    # clf=SVR(kernel='rbf', C=2000, gamma='auto')

    '''高斯基函数线性组合回归'''
    kernel = 1.0 * RBF(length_scale=0.7, length_scale_bounds=(1e-5, 1e5)) \
             + WhiteKernel(noise_level=1.0, noise_level_bounds=(1e-5, 1e4))
    # clf = GaussianProcessRegressor(kernel=kernel, alpha=0.5, n_restarts_optimizer=10)

    clf.fit(X_train, y_train)
    predicted = clf.predict(X_test)

    # mse = metrics.mean_squared_error(y_test, predicted)
    # evs = metrics.explained_variance_score(y_test, predicted)
    # r2s=metrics.r2_score(y_test, predicted)
    importances = clf.feature_importances_
    indices = np.argsort(-importances)

    err=0.0
    l=0
    for m,n in zip(y_test,predicted):
        err=np.abs((m[0]-n)/m[0])+err
        l=l+1
        print(m[0], n)
    Err=err/l
    err = np.mean(np.abs(y_test - predicted) / y_test)
    # print(Err)
    # print(err)
    # print(importances)
    # for i in range(len(indices)):
    #     print(events_name[indices[i]])

    # 保存模型
    # output = 'JDmodel' + '.pkl'
    # joblib.dump(clf,output)


if __name__ == '__main__':
    runGBRT()
