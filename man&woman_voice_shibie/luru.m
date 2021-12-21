%自己录入一段音频：
fs=16000;
fprintf('testing...\n');

y=audiorecorder(fs, 16, 1);  % 16000Hz  16bit  单声道
recordblocking(y,5);%录制5秒

rbd=get(con_rbd,'value') ;

if (rbd)
    delete('test_record/*.wav');%删除之前的文件
    m=1;%从头开始
end

name=strcat('test_record\',...
           num2str(m),'.wav');%连接字符串
y1 = getaudiodata(y,'int16'); %转格式
audiowrite(name,y1,fs);  %生成音频文件 1.wav
cut(name);
result = shibie(name);
disp(result);
m=m+1;
set(con_text,'string',result);