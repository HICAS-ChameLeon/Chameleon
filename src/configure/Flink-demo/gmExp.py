# coding: utf-8
import pandas as pd
import numpy as np
import tensorflow as tf
from sklearn import preprocessing
import math, csv, random
# import collectingData,util

def gans(real_conf):

    # Hyper Parameters
    BATCH_SIZE = 12
    LR_G = 0.0001  # learning rate for generator
    LR_D = 0.0001  # learning rate for discriminator
    N_IDEAS = 20

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
        real_art = tf.placeholder(tf.float32, [None, 28], name='real_in')  
        D_l0 = tf.layers.dense(real_art, 128, tf.nn.relu, name='l')
        # print(D_l0)
        prob_artist0 = tf.layers.dense(D_l0, 1, tf.nn.sigmoid,
                                       name='out')  
        # reuse layers for generator
        # print(prob_artist0)
        D_l1 = tf.layers.dense(G_out, 128, tf.nn.relu, name='l', reuse=True)  # receive art work from a newbie like G
        prob_artist1 = tf.layers.dense(D_l1, 1, tf.nn.sigmoid, name='out',
                                       reuse=True)  # probability that the art work is made by expert
    #
    D_loss = -tf.reduce_mean(tf.log(prob_artist0) + tf.log(1 - prob_artist1))
    G_loss = tf.reduce_mean(tf.log(1 - prob_artist1))

    train_vars = tf.trainable_variables()

    # generator中的tensor
    g_vars = [var for var in train_vars if var.name.startswith("Generator")]
    # discriminator中的tensor
    d_vars = [var for var in train_vars if var.name.startswith("discriminator")]

    # 保存生成器变量
    saver = tf.train.Saver(var_list=g_vars)

    train_D = tf.train.AdamOptimizer(LR_D).minimize(
        D_loss, var_list=tf.get_collection(tf.GraphKeys.TRAINABLE_VARIABLES, scope='Discriminator'))
    train_G = tf.train.AdamOptimizer(LR_G).minimize(
        G_loss, var_list=tf.get_collection(tf.GraphKeys.TRAINABLE_VARIABLES, scope='Generator'))

    sess = tf.Session()
    sess.run(tf.global_variables_initializer())


    for step in range(6000):
        artist_paintings = real_conf
        # print(artist_paintings)
        G_ideas = np.random.uniform(0, 1, (BATCH_SIZE, N_IDEAS))

        # print(G_ideas)
        G_paintings, pa0, Dl = sess.run([G_out, prob_artist0, D_loss, train_D, train_G],  # train and get results
                                        {G_in: G_ideas, real_art: artist_paintings})[:3]
        # if step % 5 == 0:
        #     d_loss = sess.run(D_loss, {G_in: G_ideas, real_art: artist_paintings})
        #     g_loss = sess.run(G_loss, {G_in: G_ideas, real_art: artist_paintings})
        #     print(str(step) + ': gloss:' + str(g_loss) + '  dloss：' + str(d_loss) + '  dscore：' + str(g_loss + d_loss))
    # saver.save(sess,'checkpoints/d.ckpt')
        
    G_ideas = np.random.uniform(0, 1, (BATCH_SIZE, N_IDEAS))
    samples = sess.run(G_out, {G_in: G_ideas, real_art: real_conf})
    return samples

def seq2real(best_ind, maxV, minV):
    real_ind = best_ind * (maxV - minV) + minV
    return real_ind

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


def runGmConfs(slaves):
    algorithm = './data/gmData.csv'
    data = pd.read_csv(algorithm)
    events_name = data.columns
    # print(events_name)

    a = data.sort_values("p99_latency", axis=0, ascending=True, inplace=False)
    ### delect row: latency = 0
    a = a[~a[events_name[-1]].isin([0])]
    fea = a.iloc[:, :-1]
    p99_latency = a.iloc[:, [-1]]
    X = np.array(fea, dtype=np.float32)
    p99_latency = np.array(p99_latency, dtype=np.float32)

    topn = random.randint(0, 25)
    a = []
    a.extend(X[topn])
    a = parseConfValue(a)
    a.extend(p99_latency[topn])
    # print(a)

    f1 = open('./data/gmData.csv', 'a+')
    cf1 = csv.writer(f1, delimiter=',', lineterminator='\n')
    cf1.writerow([0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0])
    cf1.writerow(a)
    f1.close()

    '''将数据进行归一化'''
    minMax = preprocessing.MinMaxScaler()
    mac = minMax.fit_transform(X)
    real_conf = mac[topn].reshape(1, 28)
    max = minMax.data_max_
    min = minMax.data_min_

    tf.set_random_seed(1)
    np.random.seed(1)
    # print(real_conf)
    sample = gans(real_conf)
    ### run with configuration generated by generation model
    for i in sample:
        i = seq2real(i,max,min)
        conf = parseConfValue(i)
        f1 = open('./data/gmData.csv', 'a+')
        cf1 = csv.writer(f1, delimiter=',', lineterminator='\n')
        cf1.writerow(conf)
        f1.close()
        # collectingData.collectingDataa('./data/gmData.csv', slaves, conf)


if __name__ == '__main__':
    # slaves = util.getSlaves()
    # for i in range(20):
    #     runGmConfs(slaves)
    runGmConfs(22)