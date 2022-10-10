clear;clc;close all;warning off;

% 参数：
% 1)板材长度plate_length = 2440（mm）
plate_length = 2440;
% 2)板材宽度plate_width = 1220 (mm)
plate_width = 1220;

% 导入A组数据
testcase={'dataA1.csv','dataA2.csv','dataA3.csv','dataA4.csv'};

% 取1，2，3，4
dataIDX=4;

% Aic组数据文件名
datafilename=testcase{dataIDX};

% 输出排版方案文件名
outfilename={'cut_program_A1.csv','cut_program_A2.csv','cut_program_A3.csv','cut_program_A4.csv'};

% 读取Aic组数据
[numericData, textData, rawData]=xlsread(datafilename);

% 初始化产品项数据
Item=[];

% Aic组数据行数，即产品项数量ItemNum
ItemNum=size(rawData,1)-1;
for ii=1:ItemNum
    Item(ii).id=rawData{ii+1,1};
    Item(ii).mate=rawData{ii+1,2};
    Item(ii).num=rawData{ii+1,3};
    Item(ii).len=rawData{ii+1,4};
    Item(ii).width=rawData{ii+1,5};
    Item(ii).order=str2num(rawData{ii+1,6}(6:end));
    Item(ii).Sel=0;
end

rectid=[1:ItemNum];
% 初始化结果，config产品项左下角坐标和长宽数据，platecnt为板材序号
result=[];
result.config=[];
result.platecnt=-1;
result=repmat(result,1,length(Item));

% 将产品项初始化为矩形
rectangles=[[Item.len]' [Item.width]'];

platecnt=0;

% 调用条带生成函数构建条带
stripe=stripe_create(plate_width,plate_length,rectangles);


% 调用遗传算法生成排样方案
plate = MyFunc1(stripe,plate_length);


%最终结果，1-材质，2-原片序号，3-产品id，4-x坐标，5-y坐标，6-长，7-宽
Result = zeros(ItemNum,7);

%遍历样板
for i=1:length(plate)
    Hight = 0;
    for j=1: length(plate{i,1}) %遍历每个样板的条带
        stripeIDX = plate{i,1}(j);
        for k=1:length(stripe{stripeIDX,3})
            itemIDX = stripe{stripeIDX,3}(k);
            Result(itemIDX,1) = dataIDX;
            Result(itemIDX,2) = i;
            Result(itemIDX,3) = Item(itemIDX).id;
            tempS = struct2array(stripe{stripeIDX,1}(k));
            Result(itemIDX,4) = tempS(1);
            Result(itemIDX,5) = tempS(2)+Hight;
            Result(itemIDX,6) = tempS(3);
            Result(itemIDX,7) = tempS(4);
        end        
        Hight = Hight + stripe{stripeIDX,2};
    end         
end

BEST_VAL = length(plate)
fprintf('数据集%s总共使用%d块板 \n\n',BEST_VAL);
BEST_LAYOUT = Result;
precent=sum(rectangles(:,1).*rectangles(:,2))/(BEST_VAL*plate_width*plate_length)*100;
fprintf('数据集%s的板材利用率为：%f%%\n',datafilename,precent);


%绘制图像展示结果
clf;
for ppp = 1:ceil(BEST_VAL/20)
    IDXStart = ppp;
    figure(ppp*20-19);
    for idx = 1:20        
        layIDX = idx + (IDXStart-1)*20;
        Rang = find(BEST_LAYOUT(:,2)==layIDX);
        if length(Rang) <=0
            break;
        end
        subplot(4,5,idx);
        hold on;

        %set(rectangle('Position', [0,0,2440,1220]), 'EdgeColor', [1, 0, 0]) ;
        set(rectangle('Position', [0,0,1220,2440]), 'EdgeColor', [0, 1, 0] ,'Linewidth', 1.5) ;
        for i =1:length(Rang)
            temp = BEST_LAYOUT(Rang(i),:);
            %     X = [temp(4):temp(6)];
            %     Y = [temp(5):temp(7)];
            %     fill(X,Y,'c');
            rect_H = rectangle('Position', [temp(4), temp(5), temp(6), temp(7)]);
            %sets the edge to be green
             set(rect_H, 'EdgeColor', [0, 0, 1], 'Linewidth', 1, 'FaceColor',[i/length(Rang)  0  0])
        end
    end
    
    
end

%写入csv文件，输出结果

fid = fopen(outfilename{dataIDX},'w+','n','UTF-8');

for i=1:ItemNum
    fprintf(fid,string(rawData{i+1,2}) + ',%d,%d,%f,%f,%f,%f\n',...
        BEST_LAYOUT(i,2),BEST_LAYOUT(i,3),...
        BEST_LAYOUT(i,4),BEST_LAYOUT(i,5),BEST_LAYOUT(i,6),BEST_LAYOUT(i,7));
end
fclose(fid);
fprintf('数据已经保存到cut_program_A%d.csv',dataIDX);




