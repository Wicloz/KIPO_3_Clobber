import subprocess as sp
from random import sample
import pandas as pd
import matplotlib as plt
from multiprocessing import Pool

# parameters
ourPlayer = 0
boardSize = ['6', '6']
playingTypes = ['minimax', 'alphabeta', 'avgmax']
n = 20
fileLocation = '../cmake-build-debug/KIPO_3_Clobber'
threadCount = 8

# functions
def runAnalysis(args):
    output = sp.check_output([fileLocation, boardSize[0], boardSize[1], args[0], args[1]], universal_newlines=True)
    print('#', end='', flush=True)
    return {
        'seed': args[0],
        'type': args[1],
        'result': int((output.splitlines()[-2]).split(' ')[-1]),
    }

if __name__ == '__main__':
    # n random seeds
    seeds = [str(x) for x in sample(range(0, 2147483647), n)]
    inputs = [(seed, type) for seed in seeds for type in playingTypes]

    # results storage
    results = [0 for x in range(0, len(playingTypes))]

    # play Clobber
    pool = Pool(threadCount)
    print('Analysing - ', end='', flush=True)

    for output in pool.map(runAnalysis, inputs):
        if output['result'] == ourPlayer:
            results[playingTypes.index(output['type'])] += 1

    print(' - Done!')

    # display results
    df = pd.DataFrame(results, index=playingTypes, columns=['wincount'])
    print(df)

    plot = df.plot.bar()
    plt.pyplot.show()

    plot.get_figure().savefig('results.svg', format='svg')
    plot.get_figure().savefig('results.pdf', format='pdf')
