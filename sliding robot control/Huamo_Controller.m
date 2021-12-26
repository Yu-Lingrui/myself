function [sys,xO,str,ts] = Huamo_Controller(t,x,u,flag) %初始化函数
switch flag
case 0
[sys , xO , str, ts] = mdllnitializeSizes;  %调用初始化子函数
case 3
sys = mdlOutputs(t,x,u);
case {2,4,9}
sys=[]; %终止仿真子函数
otherwise
error(['Unhandled flag = ',num2str(flag)]);
end
function [sys,xO, str,ts] = mdllnitializeSizes %初始化子函数
sizes = simsizes;
sizes.NumOutputs=2;  %输出变量个数
sizes.NumInputs =9;  %输入变量个数
sizes.DirFeedthrough = 1; %输入信号是否在输出端出现
sizes.NumSampleTimes = 1; % at least one sample time is needed
sys = simsizes(sizes); 
xO = [];  %初始值
str = [];
ts = [0 0]; %[0 0]用于连续系统，[-1 0]表示继承其前的采样时间设置
function sys = mdlOutputs(t,x,u) %计算输出子函数
vr = 1.0;
wr = 1.0;
xe = u(1);
ye = u(2);
te = u(3);
delta1 = 0.02; 
delta2 = 0.02;
k1 = 6.0; 
k2 = 6.0; 
s1 = xe;
s2 = te + atan(vr * ye);
Q = vr/ (1 + vr^2 * ye^2);
q(2) = (wr + Q* vr* sin(te) + k2 * s2/(abs(s2) + delta2))/(1 + Q* xe);
w = q(2);
q(1)=ye*w+vr*cos(te)+k1*s1/(abs(s1)+delta1);
sys(1)=q(1);
sys(2)=q(2);