function [sys,xO,str,ts] = Robot_Plant(t,x,u,flag)
switch flag
case 0
[sys,xO,str,ts] = mdlInitializeSizes; 
case 1
sys = mdlDerivatives(t,x,u);  %调用计算微分子函数
case 3
sys = mdlOutputs(t,x,u);      %计算输出子函数
case {2,4,9}
sys = [];
otherwise
error(['Unhandled flag = ',num2str(flag)]);
end
function [sys,x0,str,ts] = mdlInitializeSizes %初始化子函数
sizes = simsizes;
sizes.NumContStates = 3;   %连续状态变量个数
sizes.NumDiscStates = 0;  %离散状态变量个数
sizes. NumOutputs = 9;     %输出变量个数
sizes.NumInputs = 2;       %输入变量个数
sizes.DirFeedthrough = 1;  %输入信号是否在输出端出现
sizes.NumSampleTimes = 1; % at least one sample time is needed
sys = simsizes(sizes); 
x0 = [3;0;0];   %初始值
str = [];
ts = [0 0];   %[0 0]用于连续系统，[-1 0]表示继承其前的采样时间设置
function sys = mdlDerivatives(t,x,u) % Time-varying model %计算微分子函数，微分方程求解
xe = x(1);
ye = x(2);
the = x(3);
v = u(1);
w = u(2);
vr = 1.0;
wr = 1.0;
sys(1) = ye*w-v+vr*cos(the);
sys(2) = -xe*w+vr*sin(the);
sys(3) = wr-w;
function sys = mdlOutputs(t,x,u)   %计算输出子函数
xe = x(1);
ye = x(2);
the = x(3);
vr = 1.0;
wr = 1.0;
r= vr/wr;
xr = r*cos(t);
yr = r*sin(t);
thr = wr*t;
th = thr-x(3);
M1 = [cos(th) sin(th);-sin(th) cos(th)];
M2 = [xr;yr]-inv(M1)*[xe;ye];
xp = M2(1);yp = M2(2);
sys(1)=x(1);sys(2)=x(2);sys(3)=x(3); 
sys(4)=xp;sys(5)=yp;sys(6)=th; 
sys(7)=xr;sys(8)=yr;sys(9)=thr;