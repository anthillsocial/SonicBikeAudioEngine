#!/bin/bash
# Command to monitor memory usage of multiple processes:
#   $ top -d0.2 -p $(pidof SonicBikeAudioEngine | tr ' ' ','), $(pidof systemd | tr ' ' ',')
# Test to help with memory and concurrency tests
PORT=12345
DELAY=1
# Smaller files
declare -a arr=("startup.wav" "9DomskAnimal.wav" "GeeseFly.wav" "LeveNEFUPdemo.wav")
# 18mb file
#declare -a arr=("startup.wav" "FlyingHomeMstN.wav" "FlyingHomeMstN.wav" "FlyingHomeMstN.wav")

# Loop and sens osc messages
n=0
while :
do
    i="${arr[$n]}"
    echo "send_osc $PORT /load $n $i"
    send_osc $PORT /load $n $i 
    send_osc $PORT /play $n 
    sleep $DELAY
    send_osc $PORT /unload $n
    n=$((n+1))
    if [ "$n" = "4" ]; then
      n=0      
    fi
done

#"SerenadeStrHi.wav", "ZooEntry.wav", "FlyingHomeMstN.wav"  # 18mb 
#"LaLaPatr.wav ", "MeesterMuink1.wav", "SmallIntestineC.wav", "Trumpet2.wav"
