%% 采用遗传算法组合条带
%% 函数输入：条带，长度
%% 函数输出：组合后的排样
function plate = MyFunc1(stripe,plate_length)
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
    
    %遗传算法相关
    population_num=200;          %种群大小
    Max_gen= 100;                %迭代次数
    Pc=0.8;                     %交叉概率
    Pm=0.1;                     %变异概率
    totalNum=calcNum;                    %染色体长度=顾客数目
    
    
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
        fixedPop = population(1:population_num/10,:);
        %交叉
        [new_pop_intercross]=Crossing(population_num,population,Pc);
        %变异
        [new_pop_mutation]=Mutation(new_pop_intercross,Pm);
        %保留菁英
        new_pop_mutation(1:population_num/10,:) = fixedPop;
        
        %计算目标函数
        [Result,Groups]=MyDecode2(new_pop_mutation,stripe,startIDX,plate_length);
        Total_Dis = Result(:,1);
        
        %更新种群
        new_pop_new=zeros(population_num,totalNum);
        [Total_Dissort, index] = sort(Total_Dis);
        for k=1:population_num
            new_pop_new(k,:)=new_pop_mutation(index(k),:);
        end
        population=new_pop_new;
        %迭代次数加一
        
        BEST_VALS(y) = Total_Dissort(1);
        BEST_Group = Groups(index(1),:);
        assignin("base","Loss1",BEST_VALS);
        if mod(y,10) == 0
        fprintf('已迭代%d次\n',y);
        end
        y=y+1;
    end
    toc
    BEST_VAL = BEST_VALS(Max_gen);
    
    for stripe_line=1: startIDX% 以每一条带为母条带，寻找最匹配的子条带
        plate(end+1,1)={[stripe_line]};
        % 记录板材的长度
%         plate(end,2)={stripe{stripe_line,2}};
%         % 记录板材的宽度
%         plate(end,3)={max([stripe{plate{end,1},3}])};
%         plate(end,4)={length([plate{end,1}])};
    end
    for i = 1:BEST_VAL
        group = BEST_Group{i};               
        % 记录板材由哪些条带组成
        plate(end+1,1)={group};
        % 记录板材的长度
%         plate(end,2)={sum([stripe{plate{end,1},2}])};
%         % 记录板材的宽度
%         plate(end,3)={max([stripe{plate{end,1},3}])};
%         plate(end,4)={length([plate{end,1}])};
    end
    
    
%     for stripe_line=1: startIDX% 以每一条带为母条带，寻找最匹配的子条带
%         plate(end+1,1)={[stripe_line]};
%         % 记录板材的长度
%         plate(end,2)={sum([stripe{plate{end,1},2}])};
%         % 记录板材的宽度
%         plate(end,3)={max([stripe{plate{end,1},3}])};
%         plate(end,4)={length([plate{end,1}])};
%     end
%     for i = 1:BEST_VAL
%         group = BEST_Group{i};               
%         % 记录板材由哪些条带组成
%         plate(end+1,1)={group};
%         % 记录板材的长度
%         plate(end,2)={sum([stripe{plate{end,1},2}])};
%         % 记录板材的宽度
%         plate(end,3)={max([stripe{plate{end,1},3}])};
%         plate(end,4)={length([plate{end,1}])};
%     end
%     
       
%     planToSort = zeros(size(plate,1),1);
%     for i=1:size(plate,1)
%         planToSort(i) = plate{i,2};
%     end
%      [~,index]=sort(planToSort,'descend');
%      plate=plate(index,:);
%         plate = SortStruct(plate,2);
%         while plate{end,2} + plate{end-1,2} < plate_length
%             plsize = size(plate,1);
%             for i=1:plsize
%                 if plate{end,2} + plate{i,2} < plate_length
%                     plate{i,1} = [plate{i,1},plate{end,1}];
%                     % 记录板材的长度
%                     plate(i,2)={sum([stripe{plate{i,1},2}])};
%                     % 记录板材的宽度
%                     plate(i,3)={max([stripe{plate{i,1},3}])};
%                     plate(i,4)={length([plate{i,1}])};
%                     plate(end,:)=[];
%                     break;
%                 end
%             end
%              plate = SortStruct(plate,2);
%         end
    
    
end

function [newPops]=Crossing(population_num,population,Pc)
pl=randperm(population_num);
num=population_num/2;
c3=zeros(2,num);
pool=[];
newPops=population;
for kj=1:num
    c3(1,kj)=pl(2*kj-1);
    c3(2,kj)=pl(2*kj);
end%生成“配对池c3”
%%判断“配对池c3”每一对个体的随机数是否小于交叉概率Pc
rd=rand(1,num);
for kj=1:num
    if rd(kj)<Pc
        pool=[pool,c3(:,kj)];
    end
end
%%判断配对池每一对个体的随机数是否小于交叉概率Pc,若小于，保存到“产子池pool”
pool_num=size(pool,2);
for kj=1:pool_num
    c1=population(pool(1,kj),:);
    c2=population(pool(2,kj),:);
    [new_c1,new_c2]=MyCross(c1,c2);
    newPops(pool(1,kj),:)=new_c1;
    newPops(pool(2,kj),:)=new_c2;
end
end

%交叉两个染色体
function [A,B]=MyCross(A,B)
A_1=A;
B_1=B;
r=randperm(length(A));
c=min(r(1,1:2));
d=max(r(1,1:2));
for i=c:d
    A(i)=0;
end
B_1(ismember(B_1,A))=[];
A(1,c:d)=B_1;
for i=c:d
    B(i)=0;
end
A_1(ismember(A_1,B))=[];
B(1,c:d)=A_1;
end

%变异算法
function [Muts]=Mutation(targets,Pm)
n=size(targets,2);
Cross_Pop_num=size(targets,1);
r0=randperm(Cross_Pop_num);
PmCount = ceil(Pm*Cross_Pop_num);
Muts = [targets;zeros(PmCount,n)];
for j=1:PmCount
    A=targets(r0(j),:);
    A_1=A;    
    r=rand(1,n);
    Pe=find(r<Pm);%小于概率则触发变异
    sum_Pe=size(Pe,2);
    for i=1:sum_Pe
        c=A(Pe(i));
        A_1(Pe(i))=A_1(r==max(r));
        A_1(r==max(r))=c;
        Muts(Cross_Pop_num+j,:) = A_1;
    end
end
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
