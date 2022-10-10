clear;clc;close all;warning off;

% ������
% 1)��ĳ���plate_length = 2440��mm��
plate_length = 2440;
% 2)��Ŀ��plate_width = 1220 (mm)
plate_width = 1220;

% ����A������
testcase={'dataA1.csv','dataA2.csv','dataA3.csv','dataA4.csv'};

% ȡ1��2��3��4
dataIDX=4;

% Aic�������ļ���
datafilename=testcase{dataIDX};

% ����Ű淽���ļ���
outfilename={'cut_program_A1.csv','cut_program_A2.csv','cut_program_A3.csv','cut_program_A4.csv'};

% ��ȡAic������
[numericData, textData, rawData]=xlsread(datafilename);

% ��ʼ����Ʒ������
Item=[];

% Aic����������������Ʒ������ItemNum
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
% ��ʼ�������config��Ʒ�����½�����ͳ������ݣ�platecntΪ������
result=[];
result.config=[];
result.platecnt=-1;
result=repmat(result,1,length(Item));

% ����Ʒ���ʼ��Ϊ����
rectangles=[[Item.len]' [Item.width]'];

platecnt=0;

% �����������ɺ�����������
stripe=stripe_create(plate_width,plate_length,rectangles);


% �����Ŵ��㷨������������
plate = MyFunc1(stripe,plate_length);


%���ս����1-���ʣ�2-ԭƬ��ţ�3-��Ʒid��4-x���꣬5-y���꣬6-����7-��
Result = zeros(ItemNum,7);

%��������
for i=1:length(plate)
    Hight = 0;
    for j=1: length(plate{i,1}) %����ÿ�����������
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
fprintf('���ݼ�%s�ܹ�ʹ��%d��� \n\n',BEST_VAL);
BEST_LAYOUT = Result;
precent=sum(rectangles(:,1).*rectangles(:,2))/(BEST_VAL*plate_width*plate_length)*100;
fprintf('���ݼ�%s�İ��������Ϊ��%f%%\n',datafilename,precent);


%����ͼ��չʾ���
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

%д��csv�ļ���������

fid = fopen(outfilename{dataIDX},'w+','n','UTF-8');

for i=1:ItemNum
    fprintf(fid,string(rawData{i+1,2}) + ',%d,%d,%f,%f,%f,%f\n',...
        BEST_LAYOUT(i,2),BEST_LAYOUT(i,3),...
        BEST_LAYOUT(i,4),BEST_LAYOUT(i,5),BEST_LAYOUT(i,6),BEST_LAYOUT(i,7));
end
fclose(fid);
fprintf('�����Ѿ����浽cut_program_A%d.csv',dataIDX);




