import subprocess as sp
from random import sample
import pandas as pd
import numpy as np
import matplotlib as plt

print("**This script performs evaluation of AI playing style of Clobber.**\n**The results are the average number of turns per game**")

# Parameters
speler = 0
boardSize = ['6', '6']
n = 40
file = './cmake-build-debug/KIPO_3_Clobber'
playingTypes = ['alphaBeta', 'minimax']

# n random seeds
seeds = [str(x) for x in sample(range(100000, 100000000), n)]

# Results storage {random, smart, smarter}
results = np.empty((n, len(playingTypes)))

# Play tetris
for playingType in playingTypes:
    for seed in seeds:
        output = sp.check_output([file, boardSize[0], boardSize[1], seed, playingType], universal_newlines=True)
        results[seeds.index(seed)][files.index(file)] = int((output.splitlines()[-2]).split(' ')[-1])

# Calculate avarage
result = [(results.count(speler[:][0])/n)*100, (results.count(speler[:][1])/n)*100]

# Display results
print("For n = " + str(n))
print()

pd.DataFrame(result, columns=playingTypes).plot.bar()

plt.pyplot.show()