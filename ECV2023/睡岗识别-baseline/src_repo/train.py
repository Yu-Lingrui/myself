from torch.utils.data import DataLoader
from torch.optim import SGD, Adam, lr_scheduler
import argparse
from dataset import build_ev_dataset
import yaml
import torch
import numpy as np
import operator
from tqdm import tqdm
import os,shutil
import time
import matplotlib.pyplot as plt
from loss_fun import ComputeLoss
from utils import check_img_size, one_cycle
from yolov5s import My_YOLO as my_yolov5s
from yolov5l import My_YOLO as my_yolov5l
from yolov5m import My_YOLO as my_yolov5m
from yolov5x import My_YOLO as my_yolov5x
torch.cuda.set_device(0)
device = 'cuda' if torch.cuda.is_available() else 'cpu'

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--epochs', type=int, default=300)
    parser.add_argument('--labels_txt', default='labels.txt', help='labels.txt')
    parser.add_argument('--save_model', type=str, required=True, help='save weights dirname')
    parser.add_argument('--resume', action='store_true', help='resume training from last.pt')
    parser.add_argument('--augment', action='store_true', help='data augment')
    parser.add_argument('--imgsize', type=int, default=640, help='image size')
    parser.add_argument('--batchsize', type=int, default=16, help='Batch size')
    parser.add_argument('--pretrained', type=str, default='', help='Pretrained parameter file')
    parser.add_argument("--savepth_interval", default=10, type=int)
    parser.add_argument('--net_type', default='yolov5s', choices=['yolov5s', 'yolov5l', 'yolov5m', 'yolov5x'])
    parser.add_argument('--adam', action='store_true', help='use torch.optim.Adam() optimizer')
    parser.add_argument('--input_norm', action='store_true', default=True, help='Input Normliaze')
    parser.add_argument('--keep_ratio', action='store_true', default=True, help='resize image keep height and width ratio')
    parser.add_argument('--linear-lr', action='store_true', help='linear LR')
    parser.add_argument('--label-smoothing', type=float, default=0.0, help='Label smoothing epsilon')
    args = parser.parse_args()
    print(args)
    trainDataSet = build_ev_dataset(args)
    print('have', trainDataSet.num_classes, 'num_classes')
    print("Train dataset size: {}".format(len(trainDataSet)))
    dataloaderTrain = DataLoader(trainDataSet,
                                 batch_size=args.batchsize,
                                 shuffle=True,
                                 num_workers=0,
                                 pin_memory=True,
                                 collate_fn=trainDataSet.collate_fn)

    anchors = [[10, 13, 16, 30, 33, 23], [30, 61, 62, 45, 59, 119], [116, 90, 156, 198, 373, 326]]
    # net = My_YOLO(trainDataSet.num_classes, anchors=anchors)
    if args.net_type=='yolov5s':
        net = my_yolov5s(trainDataSet.num_classes, anchors=anchors)
    elif args.net_type=='yolov5l':
        net = my_yolov5l(trainDataSet.num_classes, anchors=anchors)
    elif args.net_type=='yolov5m':
        net = my_yolov5m(trainDataSet.num_classes, anchors=anchors)
    else:
        net = my_yolov5x(trainDataSet.num_classes, anchors=anchors)
    net.to(device)
    if device!='cpu':
        net = torch.nn.DataParallel(net, device_ids=[0])
    if len(args.pretrained) > 0:
        own_state = net.state_dict()
        pre_param = torch.load(args.pretrained, map_location=device)
        for a, b, namea, nameb in zip(pre_param.values(), own_state.values(), pre_param.keys(), own_state.keys()):
            if operator.eq(a.shape, b.shape):
                own_state[nameb].copy_(a)
        # net.load_state_dict(torch.load(args.pretrained, map_location=device))
    # else:
    #     net.apply(weights_init_normal)

    train_dir = args.save_model if os.path.isabs(args.save_model) else os.path.join(os.getcwd(), args.save_model)
    if not args.resume:
        if os.path.exists(train_dir):
            shutil.rmtree(train_dir)
        os.makedirs(train_dir)

    with open('hyp.scratch.yaml') as f:
        hyp = yaml.safe_load(f)  # load hyps # Hyperparameters
    nl = net.module.yolo_layers.nl
    nc = net.module.yolo_layers.nc
    gs = max(int(net.module.yolo_layers.stride.max()), 32)  # grid size (max stride)
    imgsize = (args.imgsize, args.imgsize)
    imgsz, imgsz_test = [check_img_size(x, gs) for x in imgsize]
    hyp['box'] *= 3. / nl  # scale to layers
    hyp['cls'] *= nc / 80. * 3. / nl  # scale to classes and layers
    hyp['obj'] *= (imgsz / 640) ** 2 * 3. / nl  # scale to image size and layers
    hyp['label_smoothing'] = args.label_smoothing
    net.nc = nc  # attach number of classes to model
    net.hyp = hyp  # attach hyperparameters to model
    net.gr = 1.0  # iou loss ratio (obj_loss = 1.0 or iou)

    starting_step = 0
    best_fitness = float('inf')
    nb, train_logs = len(dataloaderTrain), {}
    if args.adam:
        optimizer = Adam(net.parameters(), lr=hyp['lr0'], betas=(hyp['momentum'], 0.999))  # adjust beta1 to momentum
    else:
        optimizer = SGD(net.parameters(), lr=hyp['lr0'], momentum=hyp['momentum'], nesterov=True)

    # scheduler = lr_scheduler.MultiStepLR(optimizer, milestones=[round(args.epochs * x) for x in[0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9]])
    if args.linear_lr:
        lf = lambda x: (1 - x / (args.epochs - 1)) * (1.0 - hyp['lrf']) + hyp['lrf']  # linear
    else:
        lf = one_cycle(1, hyp['lrf'], args.epochs)  # cosine 1->hyp['lrf']
    scheduler = lr_scheduler.LambdaLR(optimizer, lr_lambda=lf)

    filenameCheckpoint = train_dir + '/last.pth'  ###load pretrain model
    if args.resume and os.path.exists(filenameCheckpoint):
        checkpoint = torch.load(filenameCheckpoint, map_location=device)
        starting_step = checkpoint['step'] + 1
        # scheduler.last_epoch = starting_step
        # checkpoint['optimizer']['param_groups'][0]['lr'] = 0.00000519
        net.load_state_dict(checkpoint['state_dict'])
        optimizer.load_state_dict(checkpoint['optimizer'])
        scheduler.load_state_dict(checkpoint['scheduler_state'])
        accord_type = checkpoint['accord_type']
        best_fitness = checkpoint['best_fitness']
        train_logs = checkpoint['train_logs']
        print("=> Loaded checkpoint at step {})".format(checkpoint['step']))
        del checkpoint
    
    compute_loss = ComputeLoss(net)  # init loss class
    for epoch in range(starting_step, args.epochs):
        net.train()
        pbar = tqdm(enumerate(dataloaderTrain), total=nb)  # progress bar
        all_loss = 0
        start = time.time()
        for i, (inputs,labels) in pbar:
            inputs = inputs.permute(0, 3, 1, 2).to(device)
            labels = labels.to(device)
            pred = net(inputs)
            # loss, loss_items = compute_loss(pred, labels, net)
            loss, loss_items = compute_loss(pred, labels.to(device))
            loss.backward()
            optimizer.step()
            optimizer.zero_grad()
            all_loss += loss.item()
            pbar.set_description('epoch[' + str(epoch)+']')
        end = time.time()
        scheduler.step(epoch=epoch)
        train_logs[epoch] = {'train_loss': all_loss, 'waste_time': end - start}
        if train_logs[epoch]['train_loss'] < best_fitness:
                best_fitness = train_logs[epoch]['train_loss']
                torch.save(net.state_dict(), os.path.join(train_dir, 'best.pth'))
        print('epoch['+str(epoch)+']:', train_logs[epoch])
        if epoch % args.savepth_interval == 0 and epoch > 0:
            torch.save(net.state_dict(), os.path.join(train_dir, 'epoch' + str(epoch) + '.pth'))

        lastinfo = {'step': epoch,
                    'best_fitness': best_fitness,
                    'state_dict': net.state_dict(),
                    'optimizer': optimizer.state_dict(),
                    'scheduler_state': scheduler.state_dict(),
                    'train_logs': train_logs}
        torch.save(lastinfo, filenameCheckpoint)
        torch.cuda.empty_cache()

    plt.clf()
    plt.xlim(0, args.epochs)
    x_axis = np.arange(args.epochs)
    y_axis = [train_logs[i]['train_loss'] for i in x_axis]
    y_axis = np.asarray(y_axis)
    plt.plot(x_axis, y_axis, color='red', label='train loss')
    plt.legend()
    plt.xlabel('epoch')
    plt.ylabel('train_log')
    plt.savefig(os.path.join(train_dir, 'train_log.jpg'), dpi=200)