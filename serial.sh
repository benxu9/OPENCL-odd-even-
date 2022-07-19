#!/bin/bash
# Configure the resources required
#SBATCH -n 1                # number of cores
#SBATCH --time=00:01:00     # time allocation, which has the format DD:HH:MM
#SBATCH --gres=gpu:1        # generic resource required (1 GPU)

echo ~~~~~ Running code 10 times ~~~~~
for i in {1..10}
do
    echo $i of 10
    ./serial
done