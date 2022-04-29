#!/bin/bash
### Apulis Platform command for train (CANN Version-20.2)
## Set Ascend Log Level, if u wanna to print on terminal, you should open 'ASCEND_SLOG_PRINT_TO_STDOUT'. 
## refer to link: https://support.huaweicloud.com/Graphdevg-cann202training1/atlasag_25_0077.html
export ASCEND_SLOG_PRINT_TO_STDOUT=0    ## Print log on terminal on(1), off(0)
export ASCEND_GLOBAL_LOG_LEVEL=3        ## Ascend log level. debug(0), info(1), warning(2), error(3)
export TF_CPP_MIN_LOG_LEVEL=2           ## Tensorflow api print Log Config

## Configure Environment for Auto Tune
## refer to link:https://support.huaweicloud.com/developmenttg-cann330alphaXtraining/atlasautotune_16_0014.html
export install_path=/home/HwHiAiUser/Ascend/ascend-toolkit/latest
export PATH=${install_path}/fwkacllib/bin:$PATH
export LD_LIBRARY_PATH=${install_path}/fwkacllib/lib64:$LD_LIBRARY_PATH
export PYTHONPATH=${install_path}/fwkacllib/python/site-packages:$PYTHONPATH

code_dir=$(cd "$(dirname "$0")"; cd ..; pwd)
echo "===>>>Python boot file dir: ${code_dir}"

current_time=`date "+%Y-%m-%d-%H-%M-%S"`

python3.7 ${code_dir}/train.py \
    --chip='npu' \
    --dataset=/data/dataset/storage/flowers/images \
    --result=./log \
    --num_classes=5 \
    --train_step=2 \
    --npu_dump_data=False \
    --npu_dump_graph=False \
    --npu_profiling=False \
    --npu_auto_tune=False 2>&1 | tee ${code_dir}/${current_time}_train_npu.log