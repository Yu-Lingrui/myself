%绘图
close all;clc;
figure(1); 
plot(out.xe,'r');
xlabel('time(s)') ;ylabel('x error');title('X轴方向误差');
figure(2);
plot(out.ye,'r');
xlabel('time(s)');ylabel('y error');title('Y轴方向误差');
figure(3);
plot(out.te,'r'); 
xlabel('time(s');ylabel('angle error');title('航向角方向误差');
figure(4);
plot(out.t,out.th(:,1),'b');
hold on; 
plot(out.t,out.th(:,2),'r');
xlabel('time(s)');ylabel('ideal and practical angle');title('航向角的位置跟踪');
figure(5);
plot(out.p(:,3),out.p(:,4),'r');
xlabel('xr');ylabel('yr');
hold on; 
plot(out.p(:,1),out.p(:,2),'b'); 
xlabel('xl');ylabel('x2');title('圆轨迹的位置的位置跟踪');
figure(6);
plot(out.t,out.q(:,1),'r');
xlabel('time(s)');ylabel('Controller input v');title('控制输入信号v');
figure(7);
plot(out.t,out.q(:,2),'r');
xlabel('time(s)');ylabel('Controller input w');title('控制输入信号w');