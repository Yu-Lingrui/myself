clear all;
clc


Data0 = importdata('dataA1.mat');
totalNum = size(Data0,1);

len_max = 2440;             %长度最大值
hei_max = 1220;             %宽度最大值
matID = 1;                  %当前材料的ID

%% PSO算法参数设置
population_num=200;    %种群大小
c1 = 2;                     % 加速常数
c2 = 2;                     % 加速常数
Vmax = totalNum;
Vmin = 1;
max = 4;
min = -4;
Max_gen=100;                 %迭代次数
N=totalNum;                 %染色体长度=顾客数目
%初始化种群：
population=zeros(population_num,totalNum);
for i=1:population_num
    population(i,:)=randperm(totalNum);
end
BEST_VAL = ones(Max_gen,1).*9999;
BEST_LAYOUT = zeros(totalNum,7);
%开始循环迭代
y=1;%循环计数器
VStep = randperm(totalNum,population_num);                 % 初始化速度
VStep = VStep.';                          
%% 迭代寻优
tic
while y<=Max_gen
    fixedPop = population;
    newPop = population(1:5,:);
    for j=1:population_num
        % 速度更新
        VStep(j,:) = VStep(j,:) + c1*randi([min,max]) + c2*randi([min,max]);
        if VStep(j,:)>Vmax, VStep(j,:)=Vmax; end %限定范围
        if VStep(j,:)<Vmin, VStep(j,:)=Vmin; end
        % 位置更新
        fixedPop(j,:)=fixedPop(j,:)+VStep(j,:);
        for k=1:N
            if fixedPop(j,k)>Vmax, fixedPop(j,k)=Vmax; end
            if fixedPop(j,k)<Vmin, fixedPop(j,k)=Vmin; end
        end
        %通过unique函数去除数组中相同的元素，然后比较俩个数组的长度
        uniq=N-length(unique(fixedPop(j,:)));
        %如果结果为0.则说明俩个数组中的没有相同的元素
        if uniq~=0
        continue;    
        end
        % 更新
        population(j,:)=fixedPop(j,:);
      end
    %保留菁英
    population(1:5,:) = newPop;
    %计算目标函数
    new_pop_new=zeros(population_num,totalNum);
    [Result,Layout]=MyDecode(population,len_max,hei_max,matID,Data0);
    Total_Dis = Result(:,1);
    [Total_Dissort, index] = sort(Total_Dis);
    % 粒子群更新
    for m=1:population_num
        new_pop_new(m,:)=population(index(m),:);
    end
    population=new_pop_new;
    %迭代次数加一   
    BEST_VAL(y) = Total_Dissort(1);
    BEST_LAYOUT = Layout;
    if mod(y,10) == 0
    fprintf('已迭代%d次\n',y);
    end
    y=y+1;
end
toc
figure;
plot(BEST_VAL,'LineWidth',1);
title('优化过程')
xlabel('迭代次数');
ylabel('需要的板材数量');

BEST_VAL(Max_gen)
pre=83.54/BEST_VAL(Max_gen);
fprintf('总的利用率%f\n',pre); 
%绘制图像展示结果
clf;
for ppp = 1:20:BEST_VAL+21
    IDXStart = ceil(ppp/20);
    figure(ppp);
    for idx = 1:20
        
        layIDX = idx + (IDXStart-1)*20;
        Rang = find(BEST_LAYOUT(:,2)==layIDX);
        if length(Rang) <=0
            break;
        end
        subplot(4,5,idx);
        hold on;

        set(rectangle('Position', [0,0,2440,1220]), 'EdgeColor', [1, 0, 0]) ;
        %set(rectangle('Position', [0,0,1220,2440]), 'EdgeColor', [1, 0, 0]) ;
        for i =1:length(Rang)
            temp = BEST_LAYOUT(Rang(i),:);
            %     X = [temp(4):temp(6)];
            %     Y = [temp(5):temp(7)];
            %     fill(X,Y,'c');
            rect_H = rectangle('Position', [temp(4), temp(5), temp(6), temp(7)]);
            %sets the edge to be green
            set(rect_H, 'EdgeColor', [0, 1, 0])
        end
    end
    
    
end

%写入csv文件，输出结果
mats = ['YW10-0218S'];
fid = fopen('cut_program.csv',"w+","n","UTF-8");
for i=1:size(BEST_LAYOUT,1)
    fprintf(fid,'YW10-0218S,%d,%d,%d,%d,%d,%d\n',...
        BEST_LAYOUT(i,1),BEST_LAYOUT(i,2),Data0(BEST_LAYOUT(i,3),1),...
        BEST_LAYOUT(i,4),BEST_LAYOUT(i,5),BEST_LAYOUT(i,6));
end
fclose(fid);
% writetable(BEST_LAYOUT,'cut_program.csv');

%解码染色体并计算目标函数值
function [Result,BestLayout]=MyDecode(Total,len_max,hei_max,matID,Data)
Result = zeros(size(Total,1),1);
num = size(Total,2);
maxScore = 0;
for idx = 1:size(Total,1)
    T = Total(idx,:);
    idx_start = 0;
    score = 0;
    endflag = true;
    Layouts = ones(num,7);
    while  endflag    
%         n_wbase = 0;%当前条带基础高度
%         n_width = 0;%当前条带最大高度
%         n_length = 0;%当前栈总宽度
%         n_wid2 = 0;%上一个块的高度（叠加用）
%         n_len2 = 0;%上一个块的宽度（叠加用）

        tempValsA = [0,0,0,0,0];
        tempValsB = [0,0,0,0,0];
        flagA = true;
        flagB = true;
        LayoutsA = zeros(num,7);
        LayoutsB = zeros(num,7);
        idx_startA = idx_start;
        idx_startB = idx_start;
        idx_begin = idx_startA + 1;
        
        while flagA
            idx_startA = idx_startA +1;
            if idx_startA > num
                endflag = false;
                break;
            end
            label0 = abs(T(idx_startA));
            temp = Data(label0,:);
            t_length = temp(1,3);
            t_width = temp(1,4);
            if T(idx_startA)<0
                t_length = temp(1,4);
                t_width = temp(1,3);
            end
            [tempValsA,flagA,posA] = JudgeAndAdd(tempValsA,flagA,...
                t_width,t_length,len_max,hei_max,1);
            if flagA
                %LayoutsA(idx_startA,:) = [matID,score,label0,posA(2),posA(1),t_width,t_length];
                LayoutsA(idx_startA,:) = [matID,score,label0,posA(1),posA(2),t_length,t_width];
            end
        end
        
        while flagB
            idx_startB = idx_startB +1;
            if idx_startB > num
                endflag = false;
                break;
            end
            label0 = abs(T(idx_startB));
            temp = Data(label0,:);
            t_length = temp(1,3);
            t_width = temp(1,4);
            if T(idx_startB)<0
                t_length = temp(1,4);
                t_width = temp(1,3);
            end
            [tempValsB,flagB,posB] = JudgeAndAdd(tempValsB,flagB,...
                t_width,t_length,hei_max,len_max,2);
            if flagB
                LayoutsB(idx_startB,:) = [matID,score,label0,posB(2),posB(1),t_width,t_length];
                %LayoutsB(idx_startB,:) = [matID,score,label0,posB(1),posB(2),t_length,t_width];
            end
        end
        
        score = score + 1;
        idx_startA = idx_startA - 1;
        idx_startB = idx_startB - 1;
        if idx_startA > idx_startB
            Layouts(idx_begin:idx_startA,:) = LayoutsA(idx_begin:idx_startA,:);
        else
            Layouts(idx_begin:idx_startB,:) = LayoutsB(idx_begin:idx_startB,:);
        end
        idx_start = max(idx_startA,idx_startB);
    end
    
    Result(idx) = score;    
    if score > maxScore
        maxScore = score;
        BestLayout = Layouts;
    end
end
end

% function [flag,values] = CreateCut(dd,len_max,hei_max,type)
%     values = zeros(size(dd,2),3);
%     flag = true;
%     if type == 1
%         
%     end
% end

function [tempVals,flag,pos] = JudgeAndAdd(tempValsOld,flag,t_w,t_l,l_max,w_max,type)
                                        %n_wb,n_w,n_w2,n_l,n_l2
n_wbase = tempValsOld(1);%n_wb;基础水平线
n_width = tempValsOld(2);%n_w;当前层最大高度
n_wid2 = tempValsOld(3);%n_w2;上一个矩形的高度
n_len2 = tempValsOld(5);%n_l2;上一个矩形的长度
n_length = tempValsOld(4);%当前层总长度
pos = [0,0];
%如果之前放置失败则直接返回。
if flag
    if n_len2 == t_l    %如果新增块的宽度和之前块的一样，则叠加上去
        n_wid2 = n_wid2 + t_w;
        n_width = max(n_width,n_wid2);
        pos = [n_length,n_wbase +n_wid2];
         
    else                %不一样则放到旁边
        n_length = n_length + t_l;
        if n_length > l_max %如果边上放不下了，则底部自动组成条带，新块放到新水平线上
            n_length = t_l;
            n_wbase = n_wbase + n_width;
            n_width = t_w;            
            pos = [0,n_wbase];
            
        else %放到边上，开启另一个栈组合
            n_width = max(n_width,t_w);
            pos = [n_length-t_l,n_wbase];
            
        end
        if n_length > l_max
            flag = false;
            tempVals = [0,0,0,0,0];
            pos = [0,0];
            return;
        end
        n_wid2 = t_w;
        n_len2 = t_l;
    end
    if n_width + n_wbase > w_max
        flag = false;
        tempVals = [0,0,0,0,0];
        pos = [0,0];
        return;
    end
end

tempVals = [n_wbase,n_width,n_wid2,n_length,n_len2];
% if type == 2
%     y0 = l_max-pos(1)-t_l;
%     pos(1) = pos(2);
%     pos(2) = y0;
% end
end