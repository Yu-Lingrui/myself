ModelArts平台上提供了三种训练开发环境，在这里主要讲解使用Pycharm开发工具进行部署训练。Modelarts上用户不能直接执行shell命令启动训练脚本，需要通过py文件作为启动文件。

比如本仓中的启动文件boot_modelarts.py，其实是通过python的os.system接口调用shell命令，执行shell脚本run_1p.sh文件，run_1p.sh文件是用来执行train.py训练脚本。如何使用Pycharm在Modelarts平台上训练，更详细的介绍请参看[内置镜像版](https://gitee.com/ascend/modelzoo/wikis/ModelArts%E4%B8%8A%E4%BD%BF%E7%94%A8NPU%E7%8E%AF%E5%A2%83%E8%AE%AD%E7%BB%83%E7%A4%BA%E4%BE%8B?sort_id=3155028)和[自定义镜像版](https://gitee.com/ascend/modelzoo/wikis/ModelArts%E8%87%AA%E5%AE%9A%E4%B9%89NPU%E8%AE%AD%E7%BB%83%E7%8E%AF%E5%A2%83%E9%95%9C%E5%83%8F%E6%89%8B%E5%86%8C%E3%80%90%E5%9F%BA%E7%A1%80%E7%89%88%E3%80%91?sort_id=3205360)这两篇Wiki.

其中主要涉及的超参有如下
```
--chip='npu' \                                          ## set chip 
--dataset=/data/dataset/storage/flowers/images \        ## dataset path
--result=./log \                                        ## log save path
--obs_dir=${obs_url}                                    ## obs directory to store result
--num_classes=5 \                                       ## number of classes
--train_step=2 \                                        ## Total train steps
--npu_dump_graph=False \                                ## compute graph
--npu_dump_data=False \                                 ## dump data for precision
--npu_profiling=False \                                 ## profiling for performance
--npu_auto_tune=False                                   ## auto tune tool 
```