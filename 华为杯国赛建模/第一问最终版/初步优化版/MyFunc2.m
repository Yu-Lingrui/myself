%% 采用遗传算法组合条带
%% 函数输入：条带，长度
%% 函数输出：组合后的排样
function plate = MyFunc2(stripe,plate_length)
%% 将stripe（条带）组成可行的排样方式
    % 初始化板材
    plate={};
    
    stripe_num = size(stripe,1);
    length_min = stripe{stripe_num,2};
    startIDX = 0;
    for idx = 1:stripe_num
        if stripe{idx,2}+length_min <= plate_length
            startIDX = idx;
            break;
        end
    end
    calcNum = stripe_num-startIDX;
    
    
    % PSO算法参数设置
    totalNum=calcNum;           %染色体长度=顾客数目
    population_num=200;         %种群大小
    c1 = 2;                     % 加速常数
    c2 = 2;                     % 加速常数
    w = 0.6;                    % 惯性因子 
    Vmax = totalNum;
    Vmin = 1;
    max = 10;
    min = -10;
    Max_gen=100;                 %迭代次数
    VStep=zeros(population_num,totalNum);
    for i=1:population_num
        VStep(i,:)=randperm(totalNum); % 初始化速度
    end
    assignin("base","VStep",VStep);
    N=totalNum;                 %染色体长度=顾客数目

    
    
    population=zeros(population_num,totalNum);
    for i=1:population_num
        population(i,:)=randperm(totalNum);
    end
    for i=1:totalNum
        if mod(i,2)==0
            population(1,i) = totalNum - i/2 + 1;
        else
            population(1,i) = (i+1)/2;
        end
    end
    BEST_VALS = ones(Max_gen,1).*9999;
%     BEST_POP = population(1,:);
    %开始循环迭代
    y=1;%循环计数器
    tic
    while y<=Max_gen
        fixedPop = population;
        newPop = population(1:population_num/10,:);
        for j=1:population_num
            % 速度更新
            for m=1:N
            VStep(j,m) = w*VStep(j,m) + c1*rand*randi([min,max]) + c2*rand*randi([min,max]);
            end
            if VStep(j,:)>Vmax, VStep(j,:)=Vmax; end %限定范围
            if VStep(j,:)<Vmin, VStep(j,:)=Vmin; end
            % 位置更新
            fixedPop(j,:)=fixedPop(j,:)+VStep(j,:);
            %通过unique函数去除数组中相同的元素，然后比较俩个数组的长度
            uniq=N-length(unique(fixedPop(j,:)));
            %如果结果为0.则说明俩个数组中没有相同的元素
            if uniq~=0
            continue;    
            end
            % argsort
            sortPop = sort(fixedPop(j,:));
            for k=1:N
            fixedPop(j,k) = find(sortPop==fixedPop(j,k));
            end
            % 更新
            population(j,:)=fixedPop(j,:);
        end
        %保留菁英
        population(1:population_num/10,:) = newPop;
        %计算目标函数
        new_pop_new=zeros(population_num,totalNum);
        [Result,Groups]=MyDecode2(population,stripe,startIDX,plate_length);
        Total_Dis = Result(:,1);
        [Total_Dissort, index] = sort(Total_Dis);
        % 粒子群更新
        for k=1:population_num
            new_pop_new(k,:)=population(index(k),:);
        end
        population=new_pop_new;
        %迭代次数加一   
        BEST_VALS(y) = Total_Dissort(1);
        BEST_Group = Groups(index(1),:);
        assignin("base","Loss2",BEST_VALS);
        if mod(y,10) == 0
        fprintf('已迭代%d次\n',y);
        end
        y=y+1;

    end
    toc
    BEST_VAL = BEST_VALS(Max_gen);
    
    for stripe_line=1: startIDX-1% 以每一条带为母条带，寻找最匹配的子条带
        plate(end+1,1)={[stripe_line]};
        % 记录板材的长度
%         plate(end,2)={sum([stripe{plate{end,1},2}])};
%         assignin("base","plate",plate);
%         % 记录板材的宽度
%         plate(end,3)={sum([stripe{plate{end,1},3}])};
    end
    for i = 1:BEST_VAL
        group = BEST_Group{i};               
        % 记录板材由哪些条带组成
        plate(end+1,1)={group};
%         % 记录板材的长度
%         plate(end,2)={sum([stripe{plate{end,1},2}])};
%         % 记录板材的宽度
%         plate(end,3)={sum([stripe{plate{end,1},3}])};
    end
%     planToSort = zeros(size(plate,1),1);
%     for i=1:size(plate,1)
%         planToSort(i) = plate{i,2};
%     end
%      [~,index]=sort(planToSort,'descend');
%      plate=plate(index,:);
%     plate = SortStruct(plate,2);
%     while plate{end,2} + plate{end-1,2} < plate_length
%         plsize = size(plate,1);
%         for i=1:plsize
%             if plate{end,2} + plate{i,2} < plate_length
%                 plate{i,1} = [plate{i,1},plate{end,1}];
%                 % 记录板材的长度
%                 plate(i,2)={sum([stripe{plate{i,1},2}])};
%                 % 记录板材的宽度
%                 plate(i,3)={sum([stripe{plate{i,1},3}])};
%                 plate(end,:)=[];
%                 break;
%             end
%         end
%          plate = SortStruct(plate,2);
%     end
%     check_1 = zeros(stripe_num,1);
%     for i=1:size(plate,1)
%        for j=1:size(plate{i,1})
%            check_1(plate{i,1}(j)) = 1;
%        end
%     end
    
end


%解码染色体并计算目标函数值
function [Result,Groups]=MyDecode2(Total,stripe,startIDX,plate_length)
Result = zeros(size(Total,1),1);
num = size(Total,2);
Groups = cell(size(Total,1),num);
for idx = 1:size(Total,1)
    current = Total(idx,:);
    score = 1;
    total_len = 0;
    for i=1:num
        idx2 = current(i) + startIDX;
        len = stripe{idx2,2};
        if total_len + len > plate_length
            score = score + 1;
            total_len = len;
            
        else
            total_len = total_len + len;            
        end
        Groups{idx,score} = [Groups{idx,score},idx2];
    end   
    
    Result(idx) = score;    
end
end

function result = SortStruct(struct,idx)
planToSort = zeros(size(struct,idx),1);
    for i=1:size(struct,1)
        planToSort(i) = struct{i,2};
    end
     [~,index]=sort(planToSort,'descend');
     result=struct(index,:);
end
