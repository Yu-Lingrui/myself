#!/bin/bash
cd /project/train/src_repo
rm -rf /project/train/models/*
# pip install -r requirements.txt -i https://mirrors.aliyun.com/pypi/simple

python train.py --labels_txt=/project/train/models/label.txt --save_model=weights --batchsize=16 --net_type=yolov5s --epochs=10 --pretrained=coco_pretrained/yolov5s.pth

python export_onnx.py --pth=weights/best.pth --onnx_path=/project/train/models/best.onnx --labels_txt=/project/train/models/label.txt --net_type=yolov5s
cp weights/train_log.jpg /project/train/result-graphs/
rm -rf weights
#python -m onnxsim demobilized_weights/best.onnx demobilized_weights/best_sim.onnx
