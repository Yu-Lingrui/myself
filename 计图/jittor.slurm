#!/bin/bash
#SBATCH -J jittor
#SBATCH -p xahdtest
#SBATCH -N 1
#SBATCH -n 32
#SBATCH --gres=dcu:4

module purge
module  load compiler/devtoolset/7.3.1 
module load  mpi/hpcx/gcc-7.3.1
module load compiler/dtk/22.04.2 
#module load anaconda3/5.2.0  

source /work/home/ac310l39ky/miniconda3/etc/profile.d/conda.sh
conda activate jittor
export LD_LIBRARY_PATH=/work/home/ac310l39ky/miniconda3/envs/jittor/lib:$LD_LIBRARY_PATH

CUDA='0,1,2,3'
N_GPU=4
BATCH=64
DATA=/work/home/ac310l39ky/ImageNetS50
IMAGENETS=/work/home/ac310l39ky/ImageNetS50

DUMP_PATH=./weights/pass50
DUMP_PATH_FINETUNE=${DUMP_PATH}/pixel_attention
DUMP_PATH_SEG=${DUMP_PATH}/pixel_finetuning
QUEUE_LENGTH=2048
QUEUE_LENGTH_PIXELATT=3840
HIDDEN_DIM=512
NUM_PROTOTYPE=500
ARCH=resnet101
NUM_CLASSES=50
EPOCH=200
EPOCH_PIXELATT=20
EPOCH_SEG=20
FREEZE_PROTOTYPES=1001
FREEZE_PROTOTYPES_PIXELATT=0

mkdir -p ${DUMP_PATH_FINETUNE}
mkdir -p ${DUMP_PATH_SEG}

CUDA_VISIBLE_DEVICES=${CUDA} mpirun -np ${N_GPU} --allow-run-as-root python main_pretrain.py \
--arch ${ARCH} \
--data_path ${DATA}/train \
--dump_path ${DUMP_PATH} \
--nmb_crops 2 6 \
--size_crops 224 96 \
--min_scale_crops 0.14 0.05 \
--max_scale_crops 1. 0.14 \
--crops_for_assign 0 1 \
--temperature 0.1 \
--epsilon 0.05 \
--sinkhorn_iterations 3 \
--feat_dim 128 \
--hidden_mlp ${HIDDEN_DIM} \
--nmb_prototypes ${NUM_PROTOTYPE} \
--queue_length ${QUEUE_LENGTH} \
--epoch_queue_starts 15 \
--epochs ${EPOCH} \
--batch_size ${BATCH} \
--base_lr 0.6 \
--final_lr 0.0006  \
--freeze_prototypes_niters ${FREEZE_PROTOTYPES} \
--wd 0.000001 \
--warmup_epochs 0 \
--workers 10 \
--seed 31 \
--shallow 3 \
--weights 1 1
