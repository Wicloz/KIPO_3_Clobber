import subprocess as sp
from random import sample
import pandas as pd
import numpy as np
import matplotlib as plt

print("**This script performs evaluation of AI playing style of Clobber.**\n**The results are the average number of turns per game**")

# Parameters
speler = 0
boardSize = ['4', '4']
n = 5
file = './cmake-build-debug/KIPO_3_Clobber'
playingTypes = ['alphaBeta', 'minimax']

# n random seeds
seeds = [str(x) for x in sample(range(100000, 100000000), n)]

# Results storage {random, smart, smarter}
results = np.empty(len(playingTypes))

# Play tetris
for playingType in playingTypes:
    result = np.empty(n)
    for seed in seeds:
        output = sp.check_output([file, boardSize[0], boardSize[1], seed, playingType], universal_newlines=True)
        result[seeds.index(seed)] = int((output.splitlines()[-2]).split(' ')[-1])
    results[playingTypes.index(playingType)] = np.count_nonzero(result==1)


# Display results
print("For n = " + str(n))
print(results)

pd.DataFrame(results, index=playingTypes).plot.bar()

plt.pyplot.show()