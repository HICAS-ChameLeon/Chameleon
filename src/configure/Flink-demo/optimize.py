# coding: utf-8
import tensorflow as tf
import numpy as np
import pandas as pd
from sklearn import preprocessing
import math
import genes, collectingData#, util

tf.set_random_seed(1)
np.random.seed(1)



algorithm = './data/demo.csv'
data = pd.read_csv(algorithm)

fea = data.iloc[:, 0:-1]

events_name = fea.columns
X = np.array(fea,dtype=np.float32)


# a = data.sort_values("p99_latency", axis=0, ascending=True, inplace=False)[0]

'''min_max_scaler data'''
minMax = preprocessing.MinMaxScaler()
mac = minMax.fit_transform(X)
max = minMax.data_max_
min = minMax.data_min_
ver = pd.read_csv('./data/best-conf.csv')
conf = ver.iloc[:,0:-1]
conf_perf = ver.iloc[:,[-1]]
conf = np.array(conf)
conf_perf = np.array(conf_perf)
best_conf = (conf - min)/(max-min)
# print(best_conf)

def gans(real_conf):
    # Hyper Parameters
    BATCH_SIZE = 200
    LR_G = 0.0001  # learning rate for generator
    LR_D = 0.0001  # learning rate for discriminator
    N_IDEAS = 15  # think of this as number of ideas for generating an configuration (Generator)

    tf.reset_default_graph()
    with tf.variable_scope('Generator'):
        G_in = tf.placeholder(tf.float32, [None, N_IDEAS])  # random ideas (could from normal distribution)
        G_l1 = tf.layers.dense(G_in, 128, tf.nn.relu)
        G_l2 = tf.layers.dense(G_l1, 64, tf.nn.relu)
        # G_l2 = tf.layers.batch_normalization(G_l2,momentum=0.8, training=True)
        # print(G_l1)
        G_out = tf.layers.dense(G_l2, 28)  # making a painting from these random ideas
        # G_out = tf.layers.batch_normalization(G_out, momentum=0.8, training=True)

    with tf.variable_scope('Discriminator'):
        real_art = tf.placeholder(tf.float32, [None, 28],
                                  name='real_in')  # receive configuration from the famous expert
        D_l0 = tf.layers.dense(real_art, 128, tf.nn.relu, name='l')
        # print(D_l0)
        prob_expert0 = tf.layers.dense(D_l0, 1, tf.nn.sigmoid,
                                       name='out')  # probability that the configuration is made by expert
        # reuse layers for generator
        D_l1 = tf.layers.dense(G_out, 128, tf.nn.relu, name='l',
                               reuse=True)  # receive configuration from a newbie like G
        prob_expert1 = tf.layers.dense(D_l1, 1, tf.nn.sigmoid, name='out',
                                       reuse=True)  # probability that the configuration is made by expert
    #
    D_loss = -tf.reduce_mean(tf.log(prob_expert0) + tf.log(1 - prob_expert1))
    G_loss = tf.reduce_mean(tf.log(1 - prob_expert1))

    train_vars = tf.trainable_variables()

    g_vars = [var for var in train_vars if var.name.startswith("Generator")]
    d_vars = [var for var in train_vars if var.name.startswith("discriminator")]

    # save variables of generator
    saver = tf.train.Saver(var_list=g_vars)

    train_D = tf.train.AdamOptimizer(LR_D).minimize(
        D_loss, var_list=tf.get_collection(tf.GraphKeys.TRAINABLE_VARIABLES, scope='Discriminator'))
    train_G = tf.train.AdamOptimizer(LR_G).minimize(
        G_loss, var_list=tf.get_collection(tf.GraphKeys.TRAINABLE_VARIABLES, scope='Generator'))

    sess = tf.Session()
    sess.run(tf.global_variables_initializer())

    for step in range(8000):
        expert_confs = real_conf  # real configurations with good performance
        # print(expert_confs)
        G_ideas = np.random.uniform(0, 1, (BATCH_SIZE, N_IDEAS))

        # print(G_ideas)
        G_confs, pa0, Dl = sess.run([G_out, prob_expert0, D_loss, train_D, train_G],  # train and get results
                                    {G_in: G_ideas, real_art: expert_confs})[:3]
        # if step % 5 == 0:
        #     d_loss = sess.run(D_loss, {G_in: G_ideas, real_art: expert_confs})
        #     g_loss = sess.run(G_loss, {G_in: G_ideas, real_art: expert_confs})
        #     print(str(step) + ': gloss:' + str(g_loss) + '  dloss：' + str(d_loss) + '  dscore：' + str(g_loss + d_loss))
        # saver.save(sess,'checkpoints/d.ckpt')
        # saver.save(sess, 'checkpoint/generator.ckpt')

    G_ideas = np.random.uniform(0, 1, (BATCH_SIZE, N_IDEAS))
    samples = sess.run(G_out, {G_in: G_ideas, real_art: real_conf})

    return samples

def find2pow(n):
    i = 1
    while math.pow(2,i) < n:
        i = i+1
    sub1 = math.pow(2,i) - n
    sub2 = math.pow(2,i-1) - n
    if sub1 > sub2:
        return i-1
    else:
        return i

def real2ind(value):
    stdInput = (value - min) / (max-min)
    return stdInput

def seq2real(best_ind, fitness):
    # real_fitness=fitness*ystd+ymean
    real_fitness=fitness
    # real_ind=best_ind
    real_ind = best_ind * (max-min) + min
    return real_ind,real_fitness

def is2pow(num):
    i = 1
    num = abs(num)
    while(math.pow(2,i) < num):
        i = i + 1
    upper = math.pow(2,i)
    lower = math.pow(2,i-1)
    relsult = lower
    if num+(upper-lower)/2 < upper:
        relsult = upper
    return relsult

def parseConfValue(conf):
    # print(conf)
    c = []
    conf[0] = int(round(conf[0]))
    c.append(int(conf[0]))
    conf[1] = int(round(conf[1]))
    c.append(int(conf[1]))
    conf[2] = int(round(conf[2]))
    ### slots
    if conf[2] > 4:
        # print(conf[2])
        conf[2] = 4
    c.append(int(conf[2]))
    conf[3] = int(round(conf[3]))
    c.append(int(conf[3]))
    conf[4] = round(conf[4], 1)
    c.append(conf[4])
    conf[5] = int(round(is2pow(conf[5])))
    c.append(int(conf[5]))
    conf[6] = round(conf[6], 1)
    c.append(conf[6])
    conf[7] = int(round(conf[7]))
    c.append(int(conf[7]))
    conf[8] = int(round(conf[8]))
    c.append(int(conf[8]))
    conf[9] = int(round(conf[9]))
    c.append(int(conf[9]))
    conf[10] = int(round(conf[10]))
    c.append(int(conf[10]))
    conf[11] = int(round(conf[11]))
    c.append(int(conf[11]))
    conf[12] = int(round(conf[12]))
    #### net.sendReceiveBufferSize
    if conf[12] > 11772379:
        conf[12] = 11772379
    if conf[12] < 5886189:
        conf[12] = 5886189
    c.append(int(conf[12]))
    conf[13] = int(round(conf[13]))
    c.append(int(conf[13]))
    conf[14] = int(round(conf[14]))
    c.append(int(conf[14]))
    conf[15] = int(round(conf[15]))
    c.append(int(conf[15]))
    conf[16] = int(round(conf[16]))
    c.append(int(conf[16]))
    conf[17] = int(round(conf[17]))
    c.append(int(conf[17]))
    conf[18] = int(round(conf[18]))
    c.append(int(conf[18]))
    conf[19] = int(round(conf[19]))
    c.append(int(conf[19]))
    conf[20] = int(round(conf[20]))
    c.append(int(conf[20]))
    conf[21] = int(round(conf[21]))
    c.append(int(conf[21]))
    conf[22] = int(round(conf[22]))
    c.append(int(conf[22]))
    conf[23] = int(round(conf[23]))
    c.append(int(conf[23]))
    conf[24] = int(round(conf[24]))
    c.append(int(conf[24]))
    conf[25] = int(round(conf[25]))
    c.append(int(conf[25]))
    conf[26] = round(conf[26],1)
    c.append(conf[26])
    conf[27] = int(round(conf[27]))
    ##### parallelism.default
    if conf[-1] > conf[2] * 10:
        conf[-1] = conf[2] * 10
    c.append(int(conf[27]))
    # print(c)
    return c

# slaves = util.getSlaves()
#
import csv
if __name__ == '__main__':
    # samples = []
    # for i in best_conf:
    #     i = i.reshape(1,28)
    #     sam = gans(i)
    #     samples.extend(sam)
    data = pd.read_csv('./data/intermediate.csv')
    samples = np.array(data)
    # print(samples)
    best_ind, best_ind.fitness.values = genes.main(samples)
    real_ind, real_fitness = seq2real(best_ind, best_ind.fitness.values)
    real_ind = parseConfValue(real_ind)
    # 输出最优的配置参数组合
    print(real_ind)
    # 写到配置文件中
    # collectingData.write2Configure(real_ind)

    