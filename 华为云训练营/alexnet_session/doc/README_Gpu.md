本仓的代码仍然可以在GPU上很好的执行，只需要你在执行脚本中指定参数chip为gpu即可。开发者可以直接执行命令 `sh scripts/run_gpu.sh`，其中主要涉及的超参有如下
```
--data_url=/home/zhonglin/Flowers-Data-Set \ ## dataset path
--chip='gpu' \                               ## set chip 
--train_url=./log \                          ## log save path
--num_classes=5 \                            ## number of classes
--train_step=6                               ## Total train steps
```