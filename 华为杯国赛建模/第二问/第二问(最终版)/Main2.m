clear all;
clc

DataIdx = 5;    %1或2或3或4或5

if DataIdx == 1
    Data0 = importdata('dataB1.mat');
elseif DataIdx == 2
    Data0 = importdata('dataB2.mat');
elseif DataIdx == 3
    Data0 = importdata('dataB3.mat');
elseif DataIdx == 4
    Data0 = importdata('dataB4.mat');
else
    Data0 = importdata('dataB5.mat');
end
Data = zeros(size(Data0));
Data(:,1) = table2array(Data0(:,1));
Data(:,2) = double(table2array(Data0(:,2)));
Data(:,3:6) = table2array(Data0(:,3:6));
Data(:,4:5) = Data(:,4:5)./1000;%将单位换算成米
Data(:,7) = Data(:,4).*Data(:,5);

totalNum = size(Data0,1);       %总数据数量
MatMax = max(Data(:,2));        %材料数量
OrderMax = max(Data(:,6));      %订单数量

ItemMax = 1000;
AreaMax = 250;
alpha = 0.5;
beta = 0.5;
board_width = 1.22;
board_length = 2.44;


% 材料索引，行号：材料ID，
% 列：每种材料对应的 1-订单序列list,2-订单数量,3-总原件数量,4-总原件面积,
% 5-订单对应原件数量list,6-订单对应原件面积list
MatInfo = cell(MatMax,6);
% 订单索引，行号：订单ID，
% 列：每个订单号对应的 1-原件IDlist,2-材料IDlist,3-总原件数量,4-总原件面积,
% 5-总材料数量,6-材料对应原件数量list,7-材料对应原件面积list
OrderInfo = cell(OrderMax,7);
for i=1:MatMax
    MatInfo{i,4} = 0;
end
for i=1:OrderMax
    OrderInfo{i,4} = 0;
end
for i=1:totalNum
    MatInfo{Data(i,2),1} = [MatInfo{Data(i,2),1},Data(i,6)];
    MatInfo{Data(i,2),4} = MatInfo{Data(i,2),4} + Data(i,7);
    OrderInfo{Data(i,6),1} = [OrderInfo{Data(i,6),1},i];
    OrderInfo{Data(i,6),2} = [OrderInfo{Data(i,6),2},Data(i,2)];
    OrderInfo{Data(i,6),4} = OrderInfo{Data(i,6),4} + Data(i,7);
end
for i=1:MatMax
    MatInfo{i,3} = length(MatInfo{i,1});
    MatInfo{i,1} = unique(MatInfo{i,1});
    MatInfo{i,2} = length(MatInfo{i,1});
    MatInfo{i,5} = zeros(1,MatInfo{i,2});
    MatInfo{i,6} = zeros(1,MatInfo{i,2});
end
for i=1:OrderMax
    OrderInfo{i,3} = length(OrderInfo{i,2});
    OrderInfo{i,2} = unique(OrderInfo{i,2});
    OrderInfo{i,5} = length(OrderInfo{i,2});
    OrderInfo{i,6} = zeros(1,OrderInfo{i,5});
    OrderInfo{i,7} = zeros(1,OrderInfo{i,5});
end
for i=1:totalNum
    list_1 = MatInfo{Data(i,2),1};
    idx = find(list_1 == Data(i,6));
    MatInfo{Data(i,2),5}(idx) = MatInfo{Data(i,2),5}(idx) + 1;
    MatInfo{Data(i,2),6}(idx) = MatInfo{Data(i,2),6}(idx) + Data(i,7);
    list_2 = OrderInfo{Data(i,6),2};
    idx2 = find(list_2 == Data(i,2));
    OrderInfo{Data(i,6),6}(idx2) = OrderInfo{Data(i,6),6}(idx2) + 1;
    OrderInfo{Data(i,6),7}(idx2) = OrderInfo{Data(i,6),7}(idx2) + Data(i,7);

end


% 批次索引，行号：批次ID，
% 列：每个批次号对应的 1-订单id,2-所有材料ID,3-总原件数量,4-总原件面积,
% 5-总材料数量,6-材料对应原件数量list,7-材料对应原件面积list
GroupInfo = cell(OrderMax,7);
AvailableList = [];
for i=1:OrderMax
    GroupInfo{i,1} = i;
    GroupInfo{i,2} = OrderInfo{i,2};
    GroupInfo{i,3} = OrderInfo{i,3};
    GroupInfo{i,4} = OrderInfo{i,4};
    GroupInfo{i,5} = OrderInfo{i,5};
    GroupInfo{i,6} = OrderInfo{i,6};
    GroupInfo{i,7} = OrderInfo{i,7};
    AvailableList = [AvailableList,i];
end

len = length(AvailableList);
MinI=0;%最小值所在位置i
MinJ=0;%最小值所在位置j
MinD = 1000;%最小值

fprintf("正在进行分批次\n");
cc = 0;
%遍历待合并列表并求最小值
while len > 1
    D = ones(len)*1000;
    MinI = 0;%最小值所在位置i
    MinJ = 0;%最小值所在位置j
    MinD = 1000;%最小值

    %生成距离矩阵
    for i=1:len
        for j=1:len
            if j>i
                D(i,j) = CalDis(GroupInfo,AvailableList(i),AvailableList(j),...
                    ItemMax,AreaMax,alpha,beta);
                if D(i,j) < MinD
                    MinD = D(i,j);
                    MinI = i;
                    MinJ = j;
                end
            else
                D(i,j) = D(j,i);
            end
        end
    end
    CombineNum = max(min(10,ceil(len/5)),ceil(len/50));
    D2 = D;
    PosList = zeros(CombineNum,2);
    posIdx2 = [1:1:len];
    for ii = 1:CombineNum
        [val,posi,posj] = MyFindMinValue(D2);
        if val >1
            break;
        end
        PosList(ii,1) = posIdx2(posi);
        PosList(ii,2) = posIdx2(posj);
        D2(posi,:) = [];
        D2(posj,:) = [];
        D2(:,posi) = [];
        D2(:,posj) = [];
        posIdx2(posi) = [];
        posIdx2(posj) = [];
    end
    DeleteIDX = [];
    for jj=1:CombineNum
        MinI = PosList(jj,1);
        MinJ = PosList(jj,2);
        if MinI >=1 && MinJ >= 1
            i1 = AvailableList(MinI);
            j1 = AvailableList(MinJ);
            GroupInfo{i1,1} = [GroupInfo{i1,1} , GroupInfo{j1,1}];
            GroupInfo{i1,2} = [GroupInfo{i1,2} , GroupInfo{j1,2}];
            [G_0,IA,~] = unique(GroupInfo{i1,2});
            GroupInfo{i1,2} = G_0;
            GroupInfo{i1,3} = GroupInfo{i1,3} + GroupInfo{j1,3};
            GroupInfo{i1,4} = GroupInfo{i1,4} + GroupInfo{j1,4};
            GroupInfo{i1,5} = length(G_0);
            GroupInfo{i1,6} = [GroupInfo{i1,6} , GroupInfo{j1,6}];
            GroupInfo{i1,6} = GroupInfo{i1,6}(IA);
            GroupInfo{i1,7} = [GroupInfo{i1,7} , GroupInfo{j1,7}];
            GroupInfo{i1,7} = GroupInfo{i1,7}(IA);
            GroupInfo{j1} = -1;
            DeleteIDX = [DeleteIDX,MinJ];
            %         AvailableList(MinJ)=[];
        end
    end
    %删除无法继续组队的元素
    sumD = sum(D);
    removeIDX = find(sumD == 1000*len);
    removeIDX = [removeIDX,DeleteIDX];
    removeIDX = unique(removeIDX);
    AvailableList(removeIDX)=[];

    len = length(AvailableList);
    if mod(cc,10) == 0
        fprintf("进度%3.2f%%\n",(1-len/length(OrderInfo))*100);
    end
    cc = cc + 1;
end
fprintf("进度100%%\n");


%删除已经合并的批次
for i=OrderMax:-1:1
    if GroupInfo{i,1} == -1
        GroupInfo(i,:) = [];
    end
end
fprintf('分批次完成，共分了%d个批次。\n',length(GroupInfo));



%所有结果变量
%1-批次序号，2-原片材质序号，3-原片序号，4-产品ID
%5-x坐标，6-y坐标，7-x方向长度，8-y方向长度
Results = zeros(length(Data),8);
totalCount_0 = 0;
totalPlant_0 = 0;
totalCount_1 = 0;
totalCount_2 = 0;

plate_length = board_length;
plate_width = board_width;


mkdir('pic');
str_0 = ['pic/dataB' num2str(DataIdx)];
if(exist(str_0,"dir"))
    rmdir(str_0,'s');
    fprintf('检测到已存在之前的图片，即将覆盖');
end
mkdir(str_0);

for i=1:length(GroupInfo) %遍历组
    C_Order = GroupInfo{i,1}; %同组所有订单号
    
    for j=1:length(GroupInfo{i,2}) %遍历同组使用的材质
        items = [];
        for k=1:length(C_Order) %遍历同组订单
            C_Items = OrderInfo{C_Order(k),1};
            for p=1:length(C_Items) %遍历每个订单组件
                if Data(C_Items(p),2) == GroupInfo{i,2}(j)
                    items = [items,C_Items(p)];
                end
            end
        end
        
        ItemNum = length(items);
        PicResults = zeros(ItemNum,8);
        PicCount = 0;
        rect = Data(items,4:5);

        plateId_1=1;

        % 调用条带生成函数构建条带
        stripe=stripe_create(plate_width,plate_length,rect);

        % 调用遗传算法生成排样方案
        plate = MyFunc1(stripe,plate_length);

        %遍历样板
        for ii=1:length(plate)           
            
            Hight = 0;
            for jj=1: length(plate{ii,1}) %遍历每个样板的条带
                stripeIDX = plate{ii,1}(jj);
                for kk=1:length(stripe{stripeIDX,3})
                    itemIDX = items(stripe{stripeIDX,3}(kk));
                    Results(itemIDX,1) = i;
                    Results(itemIDX,2) = j;
                    Results(itemIDX,3) = totalPlant_0;
                    Results(itemIDX,4) = Data(itemIDX,1);
                    tempS = struct2array(stripe{stripeIDX,1}(kk));
                    Results(itemIDX,5) = tempS(1);
                    Results(itemIDX,6) = tempS(2)+Hight;
                    Results(itemIDX,7) = tempS(3);
                    Results(itemIDX,8) = tempS(4);

                    totalCount_0 = totalCount_0 + 1;
                    PicCount = PicCount + 1;
                    PicResults(PicCount,:) = Results(itemIDX,:);
                    PicResults(PicCount,3) = ii;
                end
                Hight = Hight + stripe{stripeIDX,2};
            end
            plateId_1 = plateId_1 + 1;
            totalPlant_0 = totalPlant_0 +1;
            
            
            
        end
        
        
%         totalPlant_0 = totalPlant_0 + plateId_1;
        totalCount_1 = totalCount_1 + 1;
%         
%         range1 = find(Results(:,1)==i);
%         Results_2= Results(range1,:);
%         range2 = find(Results_2(:,2)==j);
%         DRAW_PIC = Results(range2,:);
        DRAW_PIC = PicResults;
        %绘制图像展示结果
        clf;
        for ppp = 1:ceil(plateId_1/20)
            IDXStart = ppp;
            figure(10);
            clf;
            for idx = 1:20
                layIDX = idx + (IDXStart-1)*20;
                Rang = find(DRAW_PIC(:,3)==layIDX);
                if length(Rang) <=0
                    break;
                end
                subplot(4,5,idx);
                hold on;
                
                %set(rectangle('Position', [0,0,2440,1220]), 'EdgeColor', [1, 0, 0]) ;
                set(rectangle('Position', [0,0,1220,2440]), 'EdgeColor', [0, 1, 0] ,'Linewidth', 1.5) ;
                for iiii =1:length(Rang)
                    temp = DRAW_PIC(Rang(iiii),:);
                    %     X = [temp(4):temp(6)];
                    %     Y = [temp(5):temp(7)];
                    %     fill(X,Y,'c');
                    rect_H = rectangle('Position', [temp(5)*1000, temp(6)*1000, temp(7)*1000, temp(8)*1000]);
                    %sets the edge to be green
                    set(rect_H, 'EdgeColor', [0, 0, 1], 'Linewidth', 1, 'FaceColor',[iiii/length(Rang)  0  0])
                end
            end
            frame = getframe(gcf);
            im = frame2im(frame);

            imwrite(im,sprintf(['pic/dataB%d'  '/%d_%d_%d.jpg'],DataIdx,i,j,ppp));
            
        end
    end
    precent0 = 100*totalCount_0/length(Data);
    fprintf('已排样%d批次，总进度%3.2f%%\n',i,precent0);
    
    
    
    
end


totalAreaFull = sum(Data(:,7));
precent = 100*totalAreaFull/(totalPlant_0*board_width*board_length);
fprintf('总共使用%d块原片,总利用率%3.2f%%\n',totalPlant_0,precent);




%写入csv文件，输出结果
if DataIdx == 1
    saveFileName = 'sum_order_B1.csv';
elseif DataIdx == 2
    saveFileName = 'sum_order_B2.csv';
elseif DataIdx == 3
    saveFileName = 'sum_order_B3.csv';
elseif DataIdx == 4
    saveFileName = 'sum_order_B4.csv';
else
    saveFileName = 'sum_order_B5.csv';
end

fid = fopen(saveFileName,'w+','n','UTF-8');
% fprintf(fid,'批次序号,原片材质,原片序号,产品id,产品x坐标,产品y坐标,产品x方向长度,产品y方向长度\n');
for i=1:size(Results,1)
    fprintf(fid,'%d,' + string(table2array(Data0(i,2))) + ',%d,%d,%f,%f,%f,%f\n',...
        Results(i,1),Results(i,3),Results(i,4),...
        Results(i,5)*1000,Results(i,6)*1000,Results(i,7)*1000,Results(i,8)*1000);
end
fclose(fid);

fprintf('数据已经保存!\n');








function [Result] = CalDis(g,i1,i2,I_max,A_max,alpha,beta)
if g{i1,3}+g{i2,3}>I_max
    Result = 1000;
    return;
end
if g{i1,4}+g{i2,4}>A_max
    Result = 1000;
    return;
end
[C,a1,~] = intersect(g{i1,2},g{i2,2});
% areap = g{i1,2}(a1);
areaV = g{i1,7}(a1);
R1 = sum(areaV)./max(sum(g{i1,7}),sum(g{i2,7}));
R2 = length(C)/max(g{i1,5},g{i2,5});
Result = alpha*R1 + beta*R2;
Result = 1-Result;
end

function [val,posi,posj] = MyFindMinValue(D)

val = min(min(D));
[posi0,posj0] = find(D == val);
posi= posi0(1);
posj = posj0(1);
end
