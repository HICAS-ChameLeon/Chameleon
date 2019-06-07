import random
import numpy as np
from sklearn.externals import joblib

from deap import base
from deap import creator
from deap import tools

from itertools import repeat
from collections import Sequence

clf=joblib.load('./JDmodel.pkl')

importance = clf.feature_importances_

creator.create("FitnessMin", base.Fitness, weights=(-1.0,))
creator.create("Individual", np.ndarray, fitness=creator.FitnessMin)

IND_SIZE = 28  # 染色体数

toolbox = base.Toolbox()

toolbox.register("attr_float", random.uniform, -1, 1)
# toolbox.register("individual", tools.initRepeat, creator.Individual, toolbox.attr_float, n=IND_SIZE)
toolbox.register("individual", tools.initRepeat, creator.Individual, toolbox.attr_float, n=IND_SIZE)
toolbox.register("population", tools.initRepeat, list, toolbox.individual)

# ind1 = toolbox.individual()
# print(ind1)
# print(ind1.fitness.valid)


def evaluate(individual):
    return clf.predict(individual.reshape(1,-1)),


'''根据每个特征的重要性决定个体的基因是否变异'''
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
    for i, xl, xu, indpb in zip(range(size-1), low, up,indpbs):
        if random.random() < indpb*10:
            individual[i] = individual[i]+random.uniform(xl, xu)/3
    return individual,

LOW=0.0
UP=1.0

toolbox.register("evaluate", evaluate)
toolbox.register("mate", tools.cxTwoPoint)   #交叉
toolbox.register("mutate",mutate,low=LOW,up=UP, indpbs=importance)
toolbox.register("select", tools.selTournament, tournsize=3)


def main(confs):
    random.seed(64)
    NGEN=10
    CXPB,MUTPB=0.5 ,0.2  #交叉概率 变异概率
    pop = toolbox.population(n=0)
    for i in confs:
        pop.append(creator.Individual(i))

    fitnesses = map(toolbox.evaluate, pop)
    # print(fitnesses)

    for ind, fit in zip(pop, fitnesses):
        ind.fitness.values = fit

    print("  Evaluated %i individuals" % len(pop))
    print("-- Iterative %i times --" % NGEN)

    for g in range(NGEN):
        if g % 10 == 0:
            print("-- Generation %i --" % g)
        # Select the next generation individuals
        offspring = toolbox.select(pop, len(pop))
        # Clone the selected individuals
        offspring = list(map(toolbox.clone, offspring))
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
    return best_ind, best_ind.fitness.values  # return the result:Last individual,The Return of Evaluate function

