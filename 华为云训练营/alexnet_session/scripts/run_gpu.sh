#!/bin/bash
#set env
### GPU Platform command for train
export CUDA_VISIBLE_DEVICES=0

current_time=`date "+%Y-%m-%d-%H-%M-%S"`

python3.7 train.py \
        --dataset=/root/Flowers-Data-Set \
        --result=./log \
        --chip='gpu' \
        --num_classes=5 \
        --train_step=2 2>&1 | tee ${current_time}_train_gpu.log