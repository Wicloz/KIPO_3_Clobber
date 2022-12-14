import math
import subprocess as sp
import random
import pandas as pd
import matplotlib as plt
from multiprocessing import Pool

# parameters
players = ['0', '1']
boardSize = ['5', '5']
playingTypes = ['alphabeta', 'minimax', 'avgmax']
n = 10
fileLocation = '../cmake-build-debug/KIPO_3_Clobber'
threadCount = 8

# functions
def runAnalysis(args):
    output = sp.check_output([fileLocation, boardSize[0], boardSize[1], args[0], args[1], args[2]], universal_newlines=True)
    print('#', end='', flush=True)
    return {
        'seed': args[0],
        'type': args[1],
        'player': args[2],
        'result': (output.splitlines()[-2]).split(' ')[-1],
    }

if __name__ == '__main__':
    # constant random seed
    random.seed(42)

    # n random seeds
    seeds = [str(x) for x in random.sample(range(0, 2147483647), n)]
    inputs = [(seed, type, player) for seed in seeds for type in playingTypes for player in players]

    # results storage
    results = [[0, 0] for x in range(0, len(playingTypes))]

    # play Clobber
    pool = Pool(threadCount)
    print('Analysing - ', end='', flush=True)

    for output in pool.map(runAnalysis, inputs):
        if output['result'] == output['player']:
            results[playingTypes.index(output['type'])][0] += 1
        else:
            results[playingTypes.index(output['type'])][1] += 1

    print(' - Done!')

    # display results
    df = pd.DataFrame(results, index=playingTypes, columns=['Gewonnen', 'Verloren'])
    print(df)

    realn = n * len(players)
    ystep = int(math.ceil(realn / 10))
    plot = df.plot.bar(stacked=True, yticks=range(0, realn + ystep, ystep), title='Succes ratio op een ' + str(boardSize[0]) + 'x' + str(boardSize[1]) + ' bord met nEerste=nTweede=' + str(n))
    plot.set_ylabel('Spellen Gespeeld')
    plot.set_xlabel('Gebruikt Algoritme')
    plt.pyplot.show()

    plot.get_figure().savefig('results.svg', format='svg')
    plot.get_figure().savefig('results.pdf', format='pdf')
