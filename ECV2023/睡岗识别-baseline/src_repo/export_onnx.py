import torch
import argparse
import os
from dataset import Data_Name_Color
from yolov5s import My_YOLO as my_yolov5s
from yolov5l import My_YOLO as my_yolov5l
from yolov5m import My_YOLO as my_yolov5m
from yolov5x import My_YOLO as my_yolov5x

if __name__ == "__main__":
    device = 'cuda' if torch.cuda.is_available() else 'cpu'
    parser = argparse.ArgumentParser()
    parser.add_argument('--imgsize', type=int, default=640, help='image size')
    parser.add_argument('--pth', type=str, default='pascal_voc.pth', help='train pth path')
    parser.add_argument('--onnx_path', type=str, required=True, help='save onnx path')
    parser.add_argument('--net_type', default='yolov5s', choices=['yolov5s', 'yolov5l', 'yolov5m', 'yolov5x'])
    parser.add_argument('--labels_txt', default='label.txt', help='labels.txt')
    args = parser.parse_args()

    data_color = Data_Name_Color(args.labels_txt)
    # Set up model
    anchors = [[10, 13, 16, 30, 33, 23], [30, 61, 62, 45, 59, 119], [116, 90, 156, 198, 373, 326]]
    if args.net_type == 'yolov5s':
        net = my_yolov5s(data_color.num_classes, anchors=anchors, export_onnx=True)
    elif args.net_type == 'yolov5l':
        net = my_yolov5l(data_color.num_classes, anchors=anchors, export_onnx=True)
    elif args.net_type == 'yolov5m':
        net = my_yolov5m(data_color.num_classes, anchors=anchors, export_onnx=True)
    else:
        net = my_yolov5x(data_color.num_classes, anchors=anchors, export_onnx=True)
    print('net use', args.net_type)
    # net.load_state_dict(torch.load(args.pth, map_location=device))
    own_state = net.state_dict()
    param = torch.load(args.pth, map_location=device)
    for name in param:
        own_state[name.replace('module.', '')].copy_(param[name])
    net.eval()

    inputs = torch.randn(1, 3, args.imgsize, args.imgsize)

    # Update model
#     for k, m in net.named_modules():
#         m._non_persistent_buffers_set = set()  # pytorch 1.6.0 compatibility
#         if isinstance(m, Conv):  # assign export-friendly activations
#             if isinstance(m.act, nn.Hardswish):
#                 m.act = Hardswish()
#             elif isinstance(m.act, nn.SiLU):
#                 m.act = SiLU()
    with torch.no_grad():
        # out = net(inputs)
        torch.onnx.export(net, inputs, args.onnx_path, opset_version=11, input_names=['images'], output_names=['out'])
    print('convert', args.onnx_path, 'to onnx finish!!!')
