#!usr/bin/env python3.6
from rpy2.robjects import r
import random
import time
from deap import tools
from deap import base, creator
from collections import Sequence
from itertools import repeat
import math
from HG_autoDAC import enCodein,deCodein,preByhm,selectPop,writeConf


creator.create("FitnessMin", base.Fitness, weights=(-1.0,))
creator.create("Individual", list, fitness=creator.FitnessMin)


IND_SIZE = 34  # Number of chromosomes
toolbox = base.Toolbox()
toolbox.register("attr_float", random.uniform, 0, 1)
toolbox.register("individual", tools.initRepeat, creator.Individual, toolbox.attr_float, n=IND_SIZE)
toolbox.register("population", tools.initRepeat, list, toolbox.individual)


# Operators
# difine evaluate function
# Note that a comma is a must

def evaluate(individual):
    datasize = 60  # G
    indivList = individual
    deList = deCodein(indivList)
    pre_res = preByhm(deList,datasize)
    # print(pre_res)
    return pre_res[0][0],

LOW=0.0
UP=1.0

conflen = 34
importance = []
for i in range(conflen):
    importance.append(10/34)


def mutate(individual,low,up,indpbs):
    """Mutate an individual by replacing attributes, with probability *indpb*,
      by a integer uniformly drawn between *low* and *up* inclusively.

      :param individual: :term:`Sequence <sequence>` individual to be mutated.
      :param low: The lower bound or a :term:`python:sequence` of
                  of lower bounds of the range from wich to draw the new
                  integer.
      :param up: The upper bound or a :term:`python:sequence` of
                 of upper bounds of the range from wich to draw the new
                 integer.
      :param indpbs: Independent probability for each attribute to be mutated.
      :returns: A tuple of one individual.
      """
    size = len(individual)
    if not isinstance(low, Sequence):
        low = repeat(low, size)
    elif len(low) < size:
        raise IndexError("low must be at least the size of individual: %d < %d" % (len(low), size))
    if not isinstance(up, Sequence):
        up = repeat(up, size)
    elif len(up) < size:
        raise IndexError("up must be at least the size of individual: %d < %d" % (len(up), size))
    for i, xl, xu, indpb in zip(range(size), low, up,indpbs):
        if random.random() < indpb:
            tmp = random.uniform(xl, xu)/3
            if individual[i]+tmp <=1:
                individual[i] = individual[i]+tmp
    return individual,

LOW=0.0
UP=1.0

# def mut_Gaussian(individual, mu, sigma, indpb):
#     """This function applies a gaussian mutation of mean *mu* and standard
#     deviation *sigma* on the input individual. This mutation expects a
#     :term:`sequence` individual composed of real valued attributes.
#     The *indpb* argument is the probability of each attribute to be mutated.
#
#     :param individual: Individual to be mutated.
#     :param mu: Mean or :term:`python:sequence` of means for the
#                gaussian addition mutation.
#     :param sigma: Standard deviation or :term:`python:sequence` of
#                   standard deviations for the gaussian addition mutation.
#     :param indpb: Independent probability for each attribute to be mutated.
#     :returns: A tuple of one individual.
#
#     This function uses the :func:`~random.random` and :func:`~random.gauss`
#     functions from the python base :mod:`random` module.
#     """
#     size = len(individual)
#     if not isinstance(mu, Sequence):
#         mu = repeat(mu, size)
#     elif len(mu) < size:
#         raise IndexError("mu must be at least the size of individual: %d < %d" % (len(mu), size))
#     if not isinstance(sigma, Sequence):
#         sigma = repeat(sigma, size)
#     elif len(sigma) < size:
#         raise IndexError("sigma must be at least the size of individual: %d < %d" % (len(sigma), size))
#
#     for i, m, s in zip(range(size), mu, sigma):
#         if random.random() < indpb:
#             i = 0
#             rand = 0
#             flag = False
#             for i in range(5):
#                 rand = random.gauss(m, s)
#                 if ((-2 * s < rand) and (rand < 2 * s)):
#                     flag = True
#                     break
#
#             if not flag:
#                 rand = 0
#
#             rand = (rand/(4*s))/2
#
#             tmp = individual[i] + rand
#             if (0 <= tmp ) and (tmp <= 1):
#                 individual[i] = tmp
#
#     return individual,







# use tools in deap to creat our application
toolbox.register("mate", tools.cxTwoPoint) # mate:交叉
# toolbox.register("mutate", tools.mutGaussian, mu=0, sigma=1, indpb=0.1) # mutate : 变异
# toolbox.register("mutate", mut_Gaussian, mu=0, sigma=1, indpb=0.3) # mutate : 变异
toolbox.register("mutate",mutate,low=LOW,up=UP, indpbs=importance)
toolbox.register("select", tools.selTournament, tournsize=3) # select : 选择保留的最佳个体
toolbox.register("evaluate", evaluate)  # commit our evaluate







# Algorithms
def gaSearch(popSize,NGEN):
    # create an initial population of 300 individuals (where
    # each individual is a list of integers)

    # initPop = selectPop(popSize)
    pop = toolbox.population(n=popSize)
    # for i in initPop:
    #     pop.append(creator.Individual(i))

    CXPB, MUTPB= 0.5, 0.4

    '''
    # CXPB  is the probability with which two individuals
    #       are crossed
    #
    # MUTPB is the probability for mutating an individual
    #
    # NGEN  is the number of generations for which the
    #       evolution runs
    '''

    # Evaluate the entire population
    fitnesses = map(toolbox.evaluate, pop)
    for ind, fit in zip(pop, fitnesses):
        ind.fitness.values = fit

    print("  Evaluated %i individuals" % len(pop))  # 这时候，pop的长度还是300呢
    print("-- Iterative %i times --" % NGEN)

    for g in range(NGEN):
        if g % 10 == 0:
            print("-- Generation %i --" % g)
        # Select the next generation individuals
        offspring = toolbox.select(pop, len(pop))
        # Clone the selected individuals
        offspring = list(map(toolbox.clone, offspring))
        # Change map to list,The documentation on the official website is wrong

        # Apply crossover and mutation on the offspring
        for child1, child2 in zip(offspring[::2], offspring[1::2]):
            if random.random() < CXPB:
                toolbox.mate(child1, child2)
                del child1.fitness.values
                del child2.fitness.values

        for mutant in offspring:
            if random.random() < MUTPB:
                toolbox.mutate(mutant)
                del mutant.fitness.values

        # Evaluate the individuals with an invalid fitness
        invalid_ind = [ind for ind in offspring if not ind.fitness.valid]
        fitnesses = map(toolbox.evaluate, invalid_ind)
        for ind, fit in zip(invalid_ind, fitnesses):
            ind.fitness.values = fit

        # The population is entirely replaced by the offspring
        pop[:] = offspring

    print("-- End of (successful) evolution --")

    best_ind = tools.selBest(pop, 1)[0]
    # print("pop", len(pop))
    # print(len(pop[1]))


    return best_ind, best_ind.fitness.values  # return the result:Last individual,The Return of Evaluate function




def main():
    t1 = time.clock()
    popSize = 500     # the size of population
    NGEN = 70        # the num of generations

    best_ind, best_ind.fitness.values = gaSearch(popSize,NGEN)
    # print(pop, best_ind, best_ind.fitness.values)
    # print("pop",pop)
    print("best_ind",best_ind)
    print("best_ind.fitness.values",best_ind.fitness.values)
    outList = deCodein(best_ind)
    print(outList)
    writeConf(outList)

    t2 = time.clock()

    print("cost time :",t2-t1)

main()
