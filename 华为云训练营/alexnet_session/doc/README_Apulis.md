依瞳Apulis平台，它就有点类似于Docker化的一个分布式集群训练平台，每个分配账户的用于根据账号和密码进入之后呢，可以登入到Terminal和Jupyter，跟在自己服务器上环境很接近。

在本仓中，开发者可以直接执行命令 `python3 scripts/run_apulis.sh`，其中主要涉及的超参有如下
```
--chip='npu' \                                          ## set chip 
--dataset=/data/dataset/storage/flowers/images \        ## dataset path
--result=./log \                                        ## log save path
--num_classes=5 \                                       ## number of classes
--train_step=2 \                                        ## Total train steps
--npu_dump_graph=False \                                ## compute graph
--npu_dump_data=False \                                 ## dump data for precision
--npu_profiling=False \                                 ## profiling for performance
--npu_auto_tune=False                                   ## auto tune tool 
```