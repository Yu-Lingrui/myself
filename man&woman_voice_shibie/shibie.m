%男女声基因频率识别：
function   pd=shibie(s_address)


waveFile = s_address;
% fs = 16000
% y = cut(s_address);
[y, fs] = audioread(waveFile);
time=(1:length(y))/fs;
frameSize=floor(40*fs/1000);     %帧长40ms 一共640个点   floor不大于x的最大整数
startIndex=round(7000);         %起始序号
endIndex=startIndex+frameSize-1; %结束序号
frame = y(startIndex:endIndex);  %取出该帧
frameSize=length(frame);
frame2=frame.*hamming(length(frame));  % 加hamming窗
rwy = rceps(frame2);                   % 求倒谱
ylen=length(rwy);
cepstrum=rwy(1:ylen/2); %基音检测
LF=floor(fs/500);     %设置基音搜索的范围  点数
HF=floor(fs/70);      %设置基音搜索的范围  点数
cn=cepstrum(LF:HF);   %求倒谱
[mx_cep,ind]=max(cn); %设置门限，找到峰值位置
if mx_cep > 0.08 && ind >LF  
    a= fs/(LF+ind);
else
    a=0;
end
figure(2);
plot(time, y); title(waveFile); axis tight
ylim=get(gca, 'ylim');
line([time(startIndex), time(startIndex)], ylim, 'color', 'r');
line([time(endIndex), time(endIndex)], ylim, 'color', 'r');
title('语音波形');
figure(3);
subplot(2,1,1);
plot(frame);
title('取出帧的波形');
subplot(2,1,2);
plot(cepstrum);
title('倒谱图');

[x,sr]=audioread(s_address);
meen=mean(x);
x= x - meen;
updRate=floor(20*sr/1000);          %每20ms更新
fRate=floor(40*sr/1000);            %40ms一帧
n_samples=length(x);
nFrames=floor(n_samples/updRate)-1; %帧数
k=1;
pitch=zeros(1,nFrames);
f0=zeros(1,nFrames);
LF=floor(sr/500);
HF=floor(sr/70);
m=1;
avgF0=0;
for t=1:nFrames
       yin=x(k:k+fRate-1);
       cn1=rceps(yin);
       cn=cn1(LF:HF);
       [mx_cep ind]=max(cn);
       if mx_cep > 0.08 & ind >LF
              a= sr/(LF+ind);
       else
              a=0;
       end
       f0(t)=a;
       if t>2 & nFrames>3   %中值滤波对基音轨迹图进行平滑
              z=f0(t-2:t);
              md=median(z);
              pitch(t-2)=md;
              if md > 0
                     avgF0=avgF0+md;
                     m=m+1;
              end
       else
              if nFrames<=3
              pitch(t)=a;
              avgF0=avgF0+a;
              m=m+1;
       end
     end
   k=k+updRate;
end
figure(4)
subplot(211);
plot((1:length(x))/sr, x);
ylabel('幅度');
xlabel('时间');
subplot(212);
xt=1:nFrames;
xt=20*xt;
plot(xt,pitch)
xlim([0,3]);
axis([xt(1) xt(nFrames) 0 max(pitch)+50]);
ylabel('基音频率/HZ');
xlabel('时间');

Mypitch = max(pitch);
if Mypitch>220
    pd = ['声音测试:    Woman  ', num2str(Mypitch)];
elseif Mypitch<200
    pd = ['声音测试:    Man  ', num2str(Mypitch)];
else pd = ['声音测试:   Sorry  ', num2str(Mypitch)];
end