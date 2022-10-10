%% 2022年研究生数学建模竞赛B题——子问题1：排样优化

% 已知条件及题目分析
% 假定：
% 1.只考虑齐头切的切割方式（直线切割、切割方向垂直于板材一条边，并保证每次直线切割板材可分离成两块）；
% 2.切割阶段数不超过3，同一个阶段切割方向相同；
% 3.排样方式为精确排样；
% 4.假定板材板材仅有一种规格且数量充足；
% 排样方案不用考虑锯缝宽度（即切割的缝隙宽度）影响。

% 参数：
% 1)单个批次产品项（item）总数上限 max_item_num = 1000
% 2)单个批次产品项（item）的面积总和上限max_item_area = 250（m2）
% 3)板材长度plate_length = 2440（mm）
% 4)板材宽度plate_width = 1220 (mm)

% 产品项数据格式：
% 序号item_id，材质item_material，数量item_num，长度(mm)item_length，宽度(mm)item_width，订单号item_order

% 数据集：需要进行处理
% 1)数据集A；
% 2)数据集B。

% 排版方案：以板材左底点位坐标原点，以坐标系形式描述产品项的位置分布，还原排版方案
% 板材材质，板材序号，产品id，产品x坐标，产品y坐标，产品x方向长度，产品y方向长度
% 目标函数：
% 最大化板材板材利用率，计算方法：所有产品的总面积/用到板材数量*
% 约束条件：
% 1.在相同栈（stack）里的产品项（item）的宽度（或长度）应该相同；
% 2.最终切割生成的产品项是完整的，非拼接而成。
% 3.不同材质的产品项不能安排在一个板材上；
% 4.板材

% 决策变量：


% 组批方案：输出组批结果（包含每个批次的排版方案），在产品项数据的基础上增加一列批次号属性
% 批次序号，板材材质，板材序号，产品id，产品x坐标，产品y坐标，产品x方向长度，产品y方向长度

clc,clear
%% 加载数据、输入参数

% 参数：
% 1)单个批次产品项（item）总数上限 max_item_num = 1000
max_item_num = 1000;
% 2)单个批次产品项（item）的面积总和上限max_item_area = 250（m2）
max_item_area = 250;
% 3)板材长度plate_length = 2440（mm）
plate_length = 2.440;
% 4)板材宽度plate_width = 1220 (mm)
plate_width = 1.220;

% 导入数据集，需要进行处理
% 1)数据集A；
% 2)数据集B。
load('data_of_2022106B（table）.mat')

% 统一单位为m
dataA.item_length=dataA.item_length/1000;
dataA.item_width=dataA.item_width/1000;
dataB.item_length=dataB.item_length/1000;
dataB.item_width=dataB.item_width/1000;

% 将所有产品项的长宽中，较大的作为新的长度，较小的作为新的宽度
new_length=max(dataA.item_length,dataA.item_width);
new_width=min(dataA.item_length,dataA.item_width);
dataA.item_length=new_length;
dataA.item_width=new_width;

new_length=max(dataB.item_length,dataB.item_width);
new_width=min(dataB.item_length,dataB.item_width);
dataB.item_length=new_length;
dataB.item_width=new_width;

% 增加一个面积列
dataA.item_square_area=dataA.item_length.*dataA.item_width;
dataB.item_square_area=dataB.item_length.*dataB.item_width;



%% 第一步：对每一种板材进行排样，采取规则排样和遗传算法排样
% 提取每种板材需加工的产品项数据
material_kind=unique(dataA.item_material);
All_Layout_plan={};
DataIdx = 4;% 顺序FMB-0215S，NBSY-0218SD，YW10-0218S，ZQB-0218S
% for i=1:length(material_kind) 
    % material_kind(i)号板材需加工的产品项数据
    data=dataA(dataA.item_material==material_kind(DataIdx),:);
    
    %     % 绘图
    %     plot(data.item_length,data.item_width,'r.')
    %     xlabel('产品长/m')
    %     ylabel('产品宽/m')
    %     title(strcat(dataA_material_code(material_kind(i),1),'号板材需加工产品的长宽展示'),'FontSize',12)
    %     % 坐标区域修饰
    %     ax=gca;hold on;
    %     ax.DataAspectRatio=[1,1,1];
    %     ax.XGrid='on';
    %     ax.YGrid='on';
    %     ax.XColor=[.2,.2,.2];
    %     ax.YColor=[.2,.2,.2];
    %     ax.GridLineStyle='-.';
    %     ax.GridAlpha=.1;
    %     ax.LineWidth=1.1;
    %     ax.FontSize=11;
    %     axis([0,2.44,0,1.22])
    %     set(gca,'xtick',[0:0.4:2,2.44]);
    %     set(gca,'ytick',[0:0.4:1,1.22]);
    %     saveas(gcf,strcat(dataA_material_code(material_kind(i),1),'号板材需加工产品的长宽展示.png'))
    %     close all
    
    % 按照面积由大到小排序
    [~,index]=sort(data.item_square_area,'descend');
    data=data(index,:);
    
    %     % 将相同长宽的归为一类
    %     item_square_area=unique(data.item_square_area);
    %     % 面积降序排列
    %     item_square_area=sort(item_square_area,'descend');
    %     for j=1:size(data,1)
    %         mid_var=find(item_square_area==data.item_square_area(j));
    %         data.item_kind(j)=mid_var;
    %     end
    
    % 将相同长宽的产品项归为一类
    for j=1:size(data,1)
        % 面积相同并且长和宽是否一致，则分为一类
        if j==1
            data.item_kind(j)=1;
        elseif data.item_length(j)==data.item_length(j-1)
            data.item_kind(j)=data.item_kind(j-1);
        else
            data.item_kind(j)=data.item_kind(j-1)+1;
        end
    end
    
    %% 将所有产品项item（产品项）组成栈stack（栈）

    % 计算每一类产品项item（产品项）的数量和其长度、宽度，后期可以将长、宽在相近范围内的产品项归为一类
    for j=1:max(data.item_kind)
        mid_var=find(data.item_kind==j,1);
        item_kind.length(j,1)=data.item_length(mid_var);
        item_kind.width(j,1)=data.item_width(mid_var); % 先考虑产品长轴与板材平行的情况
        item_kind.num(j,1)=length(find(data.item_kind==j));
        item_kind.item_id{j,1}=data.item_id(find(data.item_kind==j));
        
        %         % 将所有产品项item（产品项）组成栈stack（栈）
        %         k=min(item_kind.num(j),floor(plate_length/item_kind.length(j)));
        %         % 记录所有可能栈的长度，宽度，由j类产品项item组成，由k个该类产品项item组成，能组成几个相同的栈
        %         stack=[stack;k*item_kind.length(j),item_kind.width(j),j,k,floor(item_kind.num(j)/k)];
        
    end
    item_kind=struct2table(item_kind);
    
    % 初始化栈
    stack=[];
    % 记录原始产品项id
    stack_item_id={};
    
    % 记录该类产品项已经用于组栈的数量
    item_num_limit=zeros(size(item_kind.num(:,1)));
    for j=1:max(data.item_kind)
        while item_num_limit(j,1)<item_kind.num(j,1) % 已使用过的产品项的数量小于该类产品项的总数量
            % 剩余未组栈的产品项数量
            item_num_remain=item_kind.num(j,1)-item_num_limit(j,1);
            
            k=min(item_num_remain,floor(plate_length/item_kind.length(j)));
            % 记录所有可能栈的长度，宽度，由j类产品项item组成，由k个该类产品项item组成，能组成几个相同的栈
            same_stack=floor(item_num_remain/k);
            stack=[stack;k*item_kind.length(j),item_kind.width(j),j,k,same_stack];
            
            
            
            end_line=size(stack_item_id,1);
            for p=1:same_stack
                mid_var1=item_num_limit(j,1);
                mid_var2=item_kind.item_id{stack(end,3)};
                
                colume=mid_var1+(p-1)*k+1:mid_var1+p*k;

                stack_item_id(end_line+1,p)={mid_var2(colume)};
            end
            
            
            item_num_limit(j,1)=item_num_limit(j,1)+k*same_stack;
        end
    end
    
    
    
    % 将所有栈stack（栈）按照长度由小到大排序
    [~,index]=sort(stack(:,1),'descend');
    stack=stack(index,:);
    stack_item_id=stack_item_id(index,:);
    
    %     % 计算规范长度集Q
    %     Q=stack;
    %     % 3H排样方式在长度方向上考虑可能的离散切割位置，称为规范长度。
    %     % 3H排样方式的规范长度为产品项长度/宽度的整数倍。
    
    %% 将所有栈stack（栈）组成stripe（条带）
    % 约束条件是栈宽度之和不能超过板材宽度
    % 因为长度相差较大的栈组成条带会导致浪费，所以按照栈长度由大到小的顺序依次组成条带
    % 初始化条带
    stripe={};
    
    %     % 组成条带时，按照栈长度由大到小的顺序依次组合，直到达到宽度限制下的条带宽度最大值
    %     for j=1:size(stack,1) % j=栈的行数
    %         for k=stack(j,5):-1:1 % k=该类栈可组合的个数
    %             width=0; % 记录结束条带组合时条带的宽度
    %             m=0; % 记录结束条带组合时stack中的相对行数，从j行开始计数
    %             n=stack(j,5)-k; % 记录结束条带组合时的该stack使用的个数
    %             N=0;
    %             while width<=plate_width
    %                 if j+m>size(stack,1)
    %                     flag=1;
    %                     break
    %                 end
    %                 if n<stack(j+m,5)
    %                     if width+stack(j+m,2)<=plate_width % 条带的宽度在限制内，组合下一个栈
    %                         width=width+stack(j+m,2);
    %                         n=n+1;
    %                         N(m+1)=N(m+1)+1;
    % %                         stack(j+m,2)
    %
    %                     else % 条带不能再组合新的栈，输出条带
    %                         % 记录条带由哪些栈组成，由几个该类产品项item组成
    %                         stripe(end+1,1)={[j:j+m;N]};%[k,stack(j+1:j+m-1,5)',N]]};
    %                         % 记录条带的长度
    %                         stripe(end,2)={stack(j,1)};
    %                         % 记录条带的宽度
    %                         stripe(end,3)={width};
    %                         % 删除最后N为0的数据
    %                         mid_var=find(stripe{end,1}(2,:)==0);
    %                         stripe{end,1}(:,mid_var)=[];
    %                         % 记录条带是由哪些产品项组成，各类产品项的个数，各类产品项的原始产品项序号
    %                         stripe(end,4)={[stack(stripe{end,1}(1,:),3),...
    %                             stack(stripe{end,1}(1,:),4).*stripe{end,1}(2,:)']};
    %                         % 记录各产品项的原始产品项序号
    %                         stripe(end,5)={item_kind.item_id(stack(stripe{end,1}(1,:),3))};
    %
    %                         break
    %                     end
    %                 else
    %                     m=m+1;
    %                     n=0;
    %                     N(m+1)=0;
    %                 end
    %
    %             end
    %         end
    %
    %         if flag==1
    %             break
    %         end
    %
    %     end
    
    % 组成条带时，按照栈长度由大到小的顺序依次组合，直到达到宽度限制下的条带宽度最大值
    stack_line=1;
    end_line=stack_line;
    stack_num=0;
    while end_line<size(stack,1) % stack_line=栈的行数
        while stack_num<stack(stack_line,5) % stack_num=该类栈已组合的个数
            % 初始化条带宽度
            width=0;
            % 初始化该类栈使用的个数
            stack_num_record=0;
            
            while width<=plate_width
                if end_line>size(stack,1)
                    flag=1;
                    
                    % 记录条带由哪些栈组成，由几个该类产品项item组成
                    stripe(end+1,1)={[stack_line:end_line;stack_num_record]};
                    % 记录条带的长度
                    stripe(end,2)={stack(stack_line,1)};
                    % 记录条带的宽度
                    stripe(end,3)={width};
                    % 删除最后stack_num_record为0的数据
                    mid_var=find(stripe{end,1}(2,:)==0);
                    stripe{end,1}(:,mid_var)=[];
                    stack_num_record(mid_var)=[];
                    % 记录条带是由哪些产品项组成，各类产品项的个数
                    stripe(end,4)={[stack(stripe{end,1}(1,:),3),...
                        stack(stripe{end,1}(1,:),4).*stripe{end,1}(2,:)']};
                    % 记录各产品项的原始产品项序号
                    stripe(end,5)={item_kind.item_id(stack(stripe{end,1}(1,:),3))};
                    
                    % 记录组合了多少个产品项
                    stripe(end,6)={sum([stack(stripe{end,1}(1,:),4).*stripe{end,1}(2,:)'])};
                    
                    break
                end
                if stack_num>=stack(end_line,5) % 本类栈已用完
                    
                    end_line=end_line+1;
                    stack_num=0;
                    stack_num_record(end_line-stack_line+1)=0;
                else % 本类栈还未用完
                    if width+stack(end_line,2)<=plate_width % 条带的宽度在限制内，组合下一个栈
                        width=width+stack(end_line,2);
                        stack_num=stack_num+1;
                        stack_num_record(end_line-stack_line+1)=stack_num_record(end_line-stack_line+1)+1;
                        
                    else % 条带不能再组合新的栈，输出条带
                        % 记录条带由哪些栈组成，由几个该类产品项item组成
                        stripe(end+1,1)={[stack_line:end_line;stack_num_record]};
                        % 记录条带的长度
                        stripe(end,2)={stack(stack_line,1)};
                        % 记录条带的宽度
                        stripe(end,3)={width};
                        % 删除最后stack_num_record为0的数据
                        mid_var=find(stripe{end,1}(2,:)==0);
                        stripe{end,1}(:,mid_var)=[];
                        stack_num_record(mid_var)=[];
                        % 记录条带是由哪些产品项组成，各类产品项的个数
                        stripe(end,4)={[stack(stripe{end,1}(1,:),3),...
                            stack(stripe{end,1}(1,:),4).*stripe{end,1}(2,:)']};
                        % 记录各产品项的原始产品项序号
                        stripe(end,5)={item_kind.item_id(stack(stripe{end,1}(1,:),3))};
                        
                        % 记录组合了多少个产品项
                        stripe(end,6)={sum([stack(stripe{end,1}(1,:),4).*stripe{end,1}(2,:)'])};
                        
                        
                        if stack_num==0 % 如果该类栈已用完
                            stack_line=end_line;
                            stack_num=0;
                        else % 如果该类栈未用完，下一个条带接着用
                            stack_line=end_line;
                            stack_num=stack_num;
                        end
                        break
                    end
                    
                end
            end
            if flag==1
                break
            end
        end
        
        if flag==1
            break
        end
        
    end
    %% 将stripe（条带）组成可行的排样方式
%     % 初始化板材
%     plate={};
%     % 初始化条带使用情况
%     stripe_use=ones(size(stripe,1),1);
%     for stripe_line=1:size(stripe,1) % 以每一条带为母条带，寻找最匹配的子条带
%         if stripe_use(stripe_line)==0 % 母条带已经被组合过，则跳过
%             continue
%         end
%         
%         mid_var1=stripe{stripe_line,2}+[stripe{:,2}]';
%         mid_var2=find(mid_var1<=plate_length);
%         
%         % 计算母子条带相加最接近板材长度的组合
%         mid_var3=mid_var1;
%         mid_var3(stripe_line)=0; % 排除母条带本身
%         mid_var3(mid_var3>plate_length)=0; % 排除超出板材长度的条带
%         mid_var3(stripe_use==0)=0; % 排除已使用过的条带
%         
%         if sum(mid_var3>0)>=1 % 有组合起来在板材长度限制内的，选取最接近板材长度的组合，标记为使用，并记录组合
%             % 计算最接近板材长度的组合
%             
%             best=find(mid_var3==max(mid_var3));
%             
%             % 标记为使用，1表示未用，0表示已用
%             stripe_use(stripe_line)=0;
%             stripe_use(best(1))=0;
%             
%             
%             % 记录板材由哪些条带组成
%             plate(end+1,1)={[stripe_line,best(1)]};
%             % 记录板材的长度
%             plate(end,2)={sum([stripe{plate{end,1},2}])};
%             % 记录板材的宽度
%             plate(end,3)={max([stripe{plate{end,1},3}])};
%             
%             % 记录板材组合了多少条带
%             plate(end,4)={length([plate{end,1}])};
%             
%         else % 没有可组合的条带
%             
%             % 标记为使用，1表示未用，0表示已用
%             stripe_use(stripe_line)=0;
%             % 记录板材由哪些条带组成
%             plate(end+1,1)={stripe_line};
%             % 记录板材的长度
%             plate(end,2)={sum([stripe{plate{end,1},2}])};
%             % 记录板材的宽度
%             plate(end,3)={max([stripe{plate{end,1},3}])};
%             
%             % 记录板材组合了多少条带
%             plate(end,4)={length([plate{end,1}])};
%         end
%     end
%     
   plate = MyFunc1(stripe,plate_length);
    %% 根据板材排样方式，计算排版方案数据
    % 板材材质，板材序号，产品id，产品x坐标，产品y坐标，产品x方向长度，产品y方向长度
    
    % 初始化排版方案数据
    Layout_plan=[];
    
    % 板材材质plate_material
    plate_material=material_kind(DataIdx);

    %% 遍历每一个板材，根据板材排样plate得到条带stripe
    for j=1:size(plate,1)
        % 板材序号plate_order
        plate_order=j;
        % 设置板材原点
        x_plate=0;
        y_plate=0;
        
        % 初始化条带左下角坐标
        x_stripe=x_plate;
        y_stripe=y_plate;
        
        % j号板材使用到的条带stripe_plan
        stripe_plan=plate{j,1};

        %% 根据条带stripe得到栈stack
        % 遍历每一个条带
        for k=1:length(stripe_plan)
            % 当前条带长
            stripe_length=stripe{stripe_plan(k),2};
            % 当前条带宽
            stripe_width=stripe{stripe_plan(k),3};
            
            
            % k号条带使用到的栈stack_plan
            mid_var=stripe{stripe_plan(k),1};
            stack_plan=[];
            for m=1:size(mid_var,2)
                stack_plan=[stack_plan;ones(mid_var(2,m),1)*mid_var(1,m)];
            end
            
            %% 根据栈stack得到产品项item
            % 初始化栈左下角坐标
            x_stack=x_stripe;
            y_stack=y_stripe;
            % 遍历每一个栈
            for n=1:length(stack_plan)
                
                % n号栈stack使用到的产品项item_plan
                item_plan=ones(1,stack(stack_plan(n),4)).*stack(stack_plan(n),3);
                
                %% 根据栈中产品项item_plan顺序，计算每一个产品的数据
                % 初始化产品项左下角坐标
                x_item=x_stack;
                y_item=y_stack;
                for q=1:length(item_plan)
                    
                    % 当前的产品项item种类
                    item=item_plan(q);
                    % 当前的产品id 
                    item_id=stack_item_id{stack_plan(n)}(q);
                    % 产品项左下角坐标
                    x_item;
                    y_item;
                    % 当前的产品项长
                    item_length=item_kind.length(item);
                    % 当前的产品项宽
                    item_width=item_kind.width(item);
                    % 板材材质，板材序号，产品id，产品x坐标，产品y坐标，产品x方向长度，产品y方向长度
                    % 记录排版方案中每一个产品项的排版数据
                    Layout_plan=[Layout_plan;plate_material,plate_order,item_id,...
                        x_item,y_item,item_length,item_width];
                    
                    % 计算产品项右下角坐标，由于栈中产品组合的方向是x轴正方向，所以将x_item加上产品项的长
                    x_item=x_item+item_length;
                    y_item=y_stack;
                    
                    
                end
                
                % 计算栈左上角坐标，由于条带中栈组合的方向是y轴正方向，所以将y_stack加上栈的宽度，即产品项的宽
                x_stack=x_stack;
                y_stack=y_stack+item_width;
                
                
            end
            % 计算条带右下角坐标，由于板材中条带组合的方向是x轴正方向，所以将x_stripe加上条带的长度
            x_stripe=x_stripe+stripe_length;
            y_stripe=y_stripe;
        end

    end
%     All_Layout_plan(i)={Layout_plan};
% end

    %% 保存数据
%     save strcat(ques1.mat) 变量名
BEST_VAL = size(plate,1);
BEST_LAYOUT = Layout_plan;
fprintf('需要板材数量%d.csv\n',BEST_VAL);
percent = sum(table2array(data(:,7)))/(BEST_VAL*plate_length*plate_width);
fprintf('板材利用率%.2f\n',percent);

if DataIdx == 1
    saveFileName = 'cut_program_A2.csv';
elseif DataIdx == 2
    saveFileName = 'cut_program_A3.csv';
elseif DataIdx == 3
    saveFileName = 'cut_program_A1.csv';
else
    saveFileName = 'cut_program_A4.csv';
end
        
fid = fopen(saveFileName,'w+','n','UTF-8');
for i=1:size(BEST_LAYOUT,1)
    fprintf(fid,dataA_material_code(DataIdx,1) + ',%d,%d,%d,%d,%d,%d\n',...
        BEST_LAYOUT(i,2),BEST_LAYOUT(i,3),...
        BEST_LAYOUT(i,4),BEST_LAYOUT(i,5),BEST_LAYOUT(i,6),BEST_LAYOUT(i,7));
end
fclose(fid);
fprintf('数据已经保存到cut_program_A%d.csv',DataIdx);

%% 绘制图像

% 
% layIDX = 36;
% Rang = find(BEST_LAYOUT(:,2)==layIDX);
% 
% hold on;
% 
% set(rectangle('Position', [0,0,2440,1220]), 'EdgeColor', [1, 0, 0]) ;
% %set(rectangle('Position', [0,0,1220,2440]), 'EdgeColor', [1, 0, 0]) ;
% for i =1:length(Rang)
%     temp = BEST_LAYOUT(Rang(i),:);
%     %     X = [temp(4):temp(6)];
%     %     Y = [temp(5):temp(7)];
%     %     fill(X,Y,'c');
%     rect_H = rectangle('Position', [temp(4)*1000, temp(5)*1000, temp(6)*1000, temp(7)*1000]);
%     %sets the edge to be green
%     set(rect_H, 'EdgeColor', [0, 1, 0])
% end

%% 绘制图像展示结果
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

        set(rectangle('Position', [0,0,2440,1220]), 'EdgeColor', [0, 1, 0] ,'Linewidth', 1.5) ;
        %set(rectangle('Position', [0,0,1220,2440]), 'EdgeColor', [1, 0, 0]) ;
        for i =1:length(Rang)
            temp = BEST_LAYOUT(Rang(i),:);
            %     X = [temp(4):temp(6)];
            %     Y = [temp(5):temp(7)];
            %     fill(X,Y,'c');
            rect_H = rectangle('Position', [temp(4)*1000, temp(5)*1000, temp(6)*1200, temp(7)*1000]);
            %sets the edge to be green
            set(rect_H, 'EdgeColor', [0, 0, 1], 'Linewidth', 1, 'FaceColor',[i/length(Rang)  0  0])
        end
    end
end




