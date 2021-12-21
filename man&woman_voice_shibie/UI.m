%启动程序，UI界面：
clear;clc;close all;
global m;
m=1;

set(0,'defaultfigurecolor','w');
%归一化图形界面
con_car=figure('position',[400 200 680 380],...
               'numbertitle','off',...
               'name','Man or Woman');
set(con_car,'defaultuicontrolunits','normalized');

rbd=0;
% 重新测试按钮
con_rbd=uicontrol('Style','radiobutton',...
               'Position',[0.15  0.62  0.15  0.05],...
               'Value',rbd,...   rbd的值为0或1，选中为1，未选中为0
               'String','重新测试','backgroundcolor',get(gcf,'color'));

% 关闭按钮
con_close=uicontrol('style','pushbutton','position',[0.5 0.6 0.2 0.1],...
            'string','关闭','callback','close');


% 测试按钮
con_test=uicontrol('style','pushbutton','position',[0.3 0.6 0.2 0.1],...
           'string','测试');       % [left bottom width height]

% 显示字符串‘请一直说话’和测试结果
con_text=uicontrol('style','text','position',[0.3 0.1 0.4 0.4],...
         'FontSize',30,'string','请一直说话','backgroundcolor',get(gcf,'color'));

% 点击测试按钮，开始录入进行测试
set(con_test,'callback','luru');