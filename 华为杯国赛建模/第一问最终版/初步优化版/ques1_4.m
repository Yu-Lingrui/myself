%% 2022���о�����ѧ��ģ����B�⡪��������1�������Ż�

% ��֪��������Ŀ����
% �ٶ���
% 1.ֻ������ͷ�е��иʽ��ֱ���и�и��ֱ�ڰ��һ���ߣ�����֤ÿ��ֱ���и��Ŀɷ�������飩��
% 2.�и�׶���������3��ͬһ���׶��и����ͬ��
% 3.������ʽΪ��ȷ������
% 4.�ٶ���İ�Ľ���һ�ֹ�����������㣻
% �����������ÿ��Ǿ���ȣ����и�ķ�϶��ȣ�Ӱ�졣

% ������
% 1)�������β�Ʒ�item���������� max_item_num = 1000
% 2)�������β�Ʒ�item��������ܺ�����max_item_area = 250��m2��
% 3)��ĳ���plate_length = 2440��mm��
% 4)��Ŀ��plate_width = 1220 (mm)

% ��Ʒ�����ݸ�ʽ��
% ���item_id������item_material������item_num������(mm)item_length�����(mm)item_width��������item_order

% ���ݼ�����Ҫ���д���
% 1)���ݼ�A��
% 2)���ݼ�B��

% �Ű淽�����԰����׵�λ����ԭ�㣬������ϵ��ʽ������Ʒ���λ�÷ֲ�����ԭ�Ű淽��
% ��Ĳ��ʣ������ţ���Ʒid����Ʒx���꣬��Ʒy���꣬��Ʒx���򳤶ȣ���Ʒy���򳤶�
% Ŀ�꺯����
% ��󻯰�İ�������ʣ����㷽�������в�Ʒ�������/�õ��������*
% Լ��������
% 1.����ͬջ��stack����Ĳ�Ʒ�item���Ŀ�ȣ��򳤶ȣ�Ӧ����ͬ��
% 2.�����и����ɵĲ�Ʒ���������ģ���ƴ�Ӷ��ɡ�
% 3.��ͬ���ʵĲ�Ʒ��ܰ�����һ������ϣ�
% 4.���

% ���߱�����


% ��������������������������ÿ�����ε��Ű淽�������ڲ�Ʒ�����ݵĻ���������һ�����κ�����
% ������ţ���Ĳ��ʣ������ţ���Ʒid����Ʒx���꣬��Ʒy���꣬��Ʒx���򳤶ȣ���Ʒy���򳤶�

clc,clear
%% �������ݡ��������

% ������
% 1)�������β�Ʒ�item���������� max_item_num = 1000
max_item_num = 1000;
% 2)�������β�Ʒ�item��������ܺ�����max_item_area = 250��m2��
max_item_area = 250;
% 3)��ĳ���plate_length = 2440��mm��
plate_length = 2.440;
% 4)��Ŀ��plate_width = 1220 (mm)
plate_width = 1.220;

% �������ݼ�����Ҫ���д���
% 1)���ݼ�A��
% 2)���ݼ�B��
load('data_of_2022106B��table��.mat')

% ͳһ��λΪm
dataA.item_length=dataA.item_length/1000;
dataA.item_width=dataA.item_width/1000;
dataB.item_length=dataB.item_length/1000;
dataB.item_width=dataB.item_width/1000;

% �����в�Ʒ��ĳ����У��ϴ����Ϊ�µĳ��ȣ���С����Ϊ�µĿ��
new_length=max(dataA.item_length,dataA.item_width);
new_width=min(dataA.item_length,dataA.item_width);
dataA.item_length=new_length;
dataA.item_width=new_width;

new_length=max(dataB.item_length,dataB.item_width);
new_width=min(dataB.item_length,dataB.item_width);
dataB.item_length=new_length;
dataB.item_width=new_width;

% ����һ�������
dataA.item_square_area=dataA.item_length.*dataA.item_width;
dataB.item_square_area=dataB.item_length.*dataB.item_width;



%% ��һ������ÿһ�ְ�Ľ�����������ȡ�����������Ŵ��㷨����
% ��ȡÿ�ְ����ӹ��Ĳ�Ʒ������
material_kind=unique(dataA.item_material);
All_Layout_plan={};
DataIdx = 4;% ˳��FMB-0215S��NBSY-0218SD��YW10-0218S��ZQB-0218S
% for i=1:length(material_kind) 
    % material_kind(i)�Ű����ӹ��Ĳ�Ʒ������
    data=dataA(dataA.item_material==material_kind(DataIdx),:);
    
    %     % ��ͼ
    %     plot(data.item_length,data.item_width,'r.')
    %     xlabel('��Ʒ��/m')
    %     ylabel('��Ʒ��/m')
    %     title(strcat(dataA_material_code(material_kind(i),1),'�Ű����ӹ���Ʒ�ĳ���չʾ'),'FontSize',12)
    %     % ������������
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
    %     saveas(gcf,strcat(dataA_material_code(material_kind(i),1),'�Ű����ӹ���Ʒ�ĳ���չʾ.png'))
    %     close all
    
    % ��������ɴ�С����
    [~,index]=sort(data.item_square_area,'descend');
    data=data(index,:);
    
    %     % ����ͬ����Ĺ�Ϊһ��
    %     item_square_area=unique(data.item_square_area);
    %     % �����������
    %     item_square_area=sort(item_square_area,'descend');
    %     for j=1:size(data,1)
    %         mid_var=find(item_square_area==data.item_square_area(j));
    %         data.item_kind(j)=mid_var;
    %     end
    
    % ����ͬ����Ĳ�Ʒ���Ϊһ��
    for j=1:size(data,1)
        % �����ͬ���ҳ��Ϳ��Ƿ�һ�£����Ϊһ��
        if j==1
            data.item_kind(j)=1;
        elseif data.item_length(j)==data.item_length(j-1)
            data.item_kind(j)=data.item_kind(j-1);
        else
            data.item_kind(j)=data.item_kind(j-1)+1;
        end
    end
    
    %% �����в�Ʒ��item����Ʒ����ջstack��ջ��

    % ����ÿһ���Ʒ��item����Ʒ����������䳤�ȡ���ȣ����ڿ��Խ��������������Χ�ڵĲ�Ʒ���Ϊһ��
    for j=1:max(data.item_kind)
        mid_var=find(data.item_kind==j,1);
        item_kind.length(j,1)=data.item_length(mid_var);
        item_kind.width(j,1)=data.item_width(mid_var); % �ȿ��ǲ�Ʒ��������ƽ�е����
        item_kind.num(j,1)=length(find(data.item_kind==j));
        item_kind.item_id{j,1}=data.item_id(find(data.item_kind==j));
        
        %         % �����в�Ʒ��item����Ʒ����ջstack��ջ��
        %         k=min(item_kind.num(j),floor(plate_length/item_kind.length(j)));
        %         % ��¼���п���ջ�ĳ��ȣ���ȣ���j���Ʒ��item��ɣ���k�������Ʒ��item��ɣ�����ɼ�����ͬ��ջ
        %         stack=[stack;k*item_kind.length(j),item_kind.width(j),j,k,floor(item_kind.num(j)/k)];
        
    end
    item_kind=struct2table(item_kind);
    
    % ��ʼ��ջ
    stack=[];
    % ��¼ԭʼ��Ʒ��id
    stack_item_id={};
    
    % ��¼�����Ʒ���Ѿ�������ջ������
    item_num_limit=zeros(size(item_kind.num(:,1)));
    for j=1:max(data.item_kind)
        while item_num_limit(j,1)<item_kind.num(j,1) % ��ʹ�ù��Ĳ�Ʒ�������С�ڸ����Ʒ���������
            % ʣ��δ��ջ�Ĳ�Ʒ������
            item_num_remain=item_kind.num(j,1)-item_num_limit(j,1);
            
            k=min(item_num_remain,floor(plate_length/item_kind.length(j)));
            % ��¼���п���ջ�ĳ��ȣ���ȣ���j���Ʒ��item��ɣ���k�������Ʒ��item��ɣ�����ɼ�����ͬ��ջ
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
    
    
    
    % ������ջstack��ջ�����ճ�����С��������
    [~,index]=sort(stack(:,1),'descend');
    stack=stack(index,:);
    stack_item_id=stack_item_id(index,:);
    
    %     % ����淶���ȼ�Q
    %     Q=stack;
    %     % 3H������ʽ�ڳ��ȷ����Ͽ��ǿ��ܵ���ɢ�и�λ�ã���Ϊ�淶���ȡ�
    %     % 3H������ʽ�Ĺ淶����Ϊ��Ʒ���/��ȵ���������
    
    %% ������ջstack��ջ�����stripe��������
    % Լ��������ջ���֮�Ͳ��ܳ�����Ŀ��
    % ��Ϊ�������ϴ��ջ��������ᵼ���˷ѣ����԰���ջ�����ɴ�С��˳�������������
    % ��ʼ������
    stripe={};
    
    %     % �������ʱ������ջ�����ɴ�С��˳��������ϣ�ֱ���ﵽ��������µ�����������ֵ
    %     for j=1:size(stack,1) % j=ջ������
    %         for k=stack(j,5):-1:1 % k=����ջ����ϵĸ���
    %             width=0; % ��¼�����������ʱ�����Ŀ��
    %             m=0; % ��¼�����������ʱstack�е������������j�п�ʼ����
    %             n=stack(j,5)-k; % ��¼�����������ʱ�ĸ�stackʹ�õĸ���
    %             N=0;
    %             while width<=plate_width
    %                 if j+m>size(stack,1)
    %                     flag=1;
    %                     break
    %                 end
    %                 if n<stack(j+m,5)
    %                     if width+stack(j+m,2)<=plate_width % �����Ŀ���������ڣ������һ��ջ
    %                         width=width+stack(j+m,2);
    %                         n=n+1;
    %                         N(m+1)=N(m+1)+1;
    % %                         stack(j+m,2)
    %
    %                     else % ��������������µ�ջ���������
    %                         % ��¼��������Щջ��ɣ��ɼ��������Ʒ��item���
    %                         stripe(end+1,1)={[j:j+m;N]};%[k,stack(j+1:j+m-1,5)',N]]};
    %                         % ��¼�����ĳ���
    %                         stripe(end,2)={stack(j,1)};
    %                         % ��¼�����Ŀ��
    %                         stripe(end,3)={width};
    %                         % ɾ�����NΪ0������
    %                         mid_var=find(stripe{end,1}(2,:)==0);
    %                         stripe{end,1}(:,mid_var)=[];
    %                         % ��¼����������Щ��Ʒ����ɣ������Ʒ��ĸ����������Ʒ���ԭʼ��Ʒ�����
    %                         stripe(end,4)={[stack(stripe{end,1}(1,:),3),...
    %                             stack(stripe{end,1}(1,:),4).*stripe{end,1}(2,:)']};
    %                         % ��¼����Ʒ���ԭʼ��Ʒ�����
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
    
    % �������ʱ������ջ�����ɴ�С��˳��������ϣ�ֱ���ﵽ��������µ�����������ֵ
    stack_line=1;
    end_line=stack_line;
    stack_num=0;
    while end_line<size(stack,1) % stack_line=ջ������
        while stack_num<stack(stack_line,5) % stack_num=����ջ����ϵĸ���
            % ��ʼ���������
            width=0;
            % ��ʼ������ջʹ�õĸ���
            stack_num_record=0;
            
            while width<=plate_width
                if end_line>size(stack,1)
                    flag=1;
                    
                    % ��¼��������Щջ��ɣ��ɼ��������Ʒ��item���
                    stripe(end+1,1)={[stack_line:end_line;stack_num_record]};
                    % ��¼�����ĳ���
                    stripe(end,2)={stack(stack_line,1)};
                    % ��¼�����Ŀ��
                    stripe(end,3)={width};
                    % ɾ�����stack_num_recordΪ0������
                    mid_var=find(stripe{end,1}(2,:)==0);
                    stripe{end,1}(:,mid_var)=[];
                    stack_num_record(mid_var)=[];
                    % ��¼����������Щ��Ʒ����ɣ������Ʒ��ĸ���
                    stripe(end,4)={[stack(stripe{end,1}(1,:),3),...
                        stack(stripe{end,1}(1,:),4).*stripe{end,1}(2,:)']};
                    % ��¼����Ʒ���ԭʼ��Ʒ�����
                    stripe(end,5)={item_kind.item_id(stack(stripe{end,1}(1,:),3))};
                    
                    % ��¼����˶��ٸ���Ʒ��
                    stripe(end,6)={sum([stack(stripe{end,1}(1,:),4).*stripe{end,1}(2,:)'])};
                    
                    break
                end
                if stack_num>=stack(end_line,5) % ����ջ������
                    
                    end_line=end_line+1;
                    stack_num=0;
                    stack_num_record(end_line-stack_line+1)=0;
                else % ����ջ��δ����
                    if width+stack(end_line,2)<=plate_width % �����Ŀ���������ڣ������һ��ջ
                        width=width+stack(end_line,2);
                        stack_num=stack_num+1;
                        stack_num_record(end_line-stack_line+1)=stack_num_record(end_line-stack_line+1)+1;
                        
                    else % ��������������µ�ջ���������
                        % ��¼��������Щջ��ɣ��ɼ��������Ʒ��item���
                        stripe(end+1,1)={[stack_line:end_line;stack_num_record]};
                        % ��¼�����ĳ���
                        stripe(end,2)={stack(stack_line,1)};
                        % ��¼�����Ŀ��
                        stripe(end,3)={width};
                        % ɾ�����stack_num_recordΪ0������
                        mid_var=find(stripe{end,1}(2,:)==0);
                        stripe{end,1}(:,mid_var)=[];
                        stack_num_record(mid_var)=[];
                        % ��¼����������Щ��Ʒ����ɣ������Ʒ��ĸ���
                        stripe(end,4)={[stack(stripe{end,1}(1,:),3),...
                            stack(stripe{end,1}(1,:),4).*stripe{end,1}(2,:)']};
                        % ��¼����Ʒ���ԭʼ��Ʒ�����
                        stripe(end,5)={item_kind.item_id(stack(stripe{end,1}(1,:),3))};
                        
                        % ��¼����˶��ٸ���Ʒ��
                        stripe(end,6)={sum([stack(stripe{end,1}(1,:),4).*stripe{end,1}(2,:)'])};
                        
                        
                        if stack_num==0 % �������ջ������
                            stack_line=end_line;
                            stack_num=0;
                        else % �������ջδ���꣬��һ������������
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
    %% ��stripe����������ɿ��е�������ʽ
%     % ��ʼ�����
%     plate={};
%     % ��ʼ������ʹ�����
%     stripe_use=ones(size(stripe,1),1);
%     for stripe_line=1:size(stripe,1) % ��ÿһ����Ϊĸ������Ѱ����ƥ���������
%         if stripe_use(stripe_line)==0 % ĸ�����Ѿ�����Ϲ���������
%             continue
%         end
%         
%         mid_var1=stripe{stripe_line,2}+[stripe{:,2}]';
%         mid_var2=find(mid_var1<=plate_length);
%         
%         % ����ĸ�����������ӽ���ĳ��ȵ����
%         mid_var3=mid_var1;
%         mid_var3(stripe_line)=0; % �ų�ĸ��������
%         mid_var3(mid_var3>plate_length)=0; % �ų�������ĳ��ȵ�����
%         mid_var3(stripe_use==0)=0; % �ų���ʹ�ù�������
%         
%         if sum(mid_var3>0)>=1 % ����������ڰ�ĳ��������ڵģ�ѡȡ��ӽ���ĳ��ȵ���ϣ����Ϊʹ�ã�����¼���
%             % ������ӽ���ĳ��ȵ����
%             
%             best=find(mid_var3==max(mid_var3));
%             
%             % ���Ϊʹ�ã�1��ʾδ�ã�0��ʾ����
%             stripe_use(stripe_line)=0;
%             stripe_use(best(1))=0;
%             
%             
%             % ��¼�������Щ�������
%             plate(end+1,1)={[stripe_line,best(1)]};
%             % ��¼��ĵĳ���
%             plate(end,2)={sum([stripe{plate{end,1},2}])};
%             % ��¼��ĵĿ��
%             plate(end,3)={max([stripe{plate{end,1},3}])};
%             
%             % ��¼�������˶�������
%             plate(end,4)={length([plate{end,1}])};
%             
%         else % û�п���ϵ�����
%             
%             % ���Ϊʹ�ã�1��ʾδ�ã�0��ʾ����
%             stripe_use(stripe_line)=0;
%             % ��¼�������Щ�������
%             plate(end+1,1)={stripe_line};
%             % ��¼��ĵĳ���
%             plate(end,2)={sum([stripe{plate{end,1},2}])};
%             % ��¼��ĵĿ��
%             plate(end,3)={max([stripe{plate{end,1},3}])};
%             
%             % ��¼�������˶�������
%             plate(end,4)={length([plate{end,1}])};
%         end
%     end
%     
   plate = MyFunc1(stripe,plate_length);
    %% ���ݰ��������ʽ�������Ű淽������
    % ��Ĳ��ʣ������ţ���Ʒid����Ʒx���꣬��Ʒy���꣬��Ʒx���򳤶ȣ���Ʒy���򳤶�
    
    % ��ʼ���Ű淽������
    Layout_plan=[];
    
    % ��Ĳ���plate_material
    plate_material=material_kind(DataIdx);

    %% ����ÿһ����ģ����ݰ������plate�õ�����stripe
    for j=1:size(plate,1)
        % ������plate_order
        plate_order=j;
        % ���ð��ԭ��
        x_plate=0;
        y_plate=0;
        
        % ��ʼ���������½�����
        x_stripe=x_plate;
        y_stripe=y_plate;
        
        % j�Ű��ʹ�õ�������stripe_plan
        stripe_plan=plate{j,1};

        %% ��������stripe�õ�ջstack
        % ����ÿһ������
        for k=1:length(stripe_plan)
            % ��ǰ������
            stripe_length=stripe{stripe_plan(k),2};
            % ��ǰ������
            stripe_width=stripe{stripe_plan(k),3};
            
            
            % k������ʹ�õ���ջstack_plan
            mid_var=stripe{stripe_plan(k),1};
            stack_plan=[];
            for m=1:size(mid_var,2)
                stack_plan=[stack_plan;ones(mid_var(2,m),1)*mid_var(1,m)];
            end
            
            %% ����ջstack�õ���Ʒ��item
            % ��ʼ��ջ���½�����
            x_stack=x_stripe;
            y_stack=y_stripe;
            % ����ÿһ��ջ
            for n=1:length(stack_plan)
                
                % n��ջstackʹ�õ��Ĳ�Ʒ��item_plan
                item_plan=ones(1,stack(stack_plan(n),4)).*stack(stack_plan(n),3);
                
                %% ����ջ�в�Ʒ��item_plan˳�򣬼���ÿһ����Ʒ������
                % ��ʼ����Ʒ�����½�����
                x_item=x_stack;
                y_item=y_stack;
                for q=1:length(item_plan)
                    
                    % ��ǰ�Ĳ�Ʒ��item����
                    item=item_plan(q);
                    % ��ǰ�Ĳ�Ʒid 
                    item_id=stack_item_id{stack_plan(n)}(q);
                    % ��Ʒ�����½�����
                    x_item;
                    y_item;
                    % ��ǰ�Ĳ�Ʒ�
                    item_length=item_kind.length(item);
                    % ��ǰ�Ĳ�Ʒ���
                    item_width=item_kind.width(item);
                    % ��Ĳ��ʣ������ţ���Ʒid����Ʒx���꣬��Ʒy���꣬��Ʒx���򳤶ȣ���Ʒy���򳤶�
                    % ��¼�Ű淽����ÿһ����Ʒ����Ű�����
                    Layout_plan=[Layout_plan;plate_material,plate_order,item_id,...
                        x_item,y_item,item_length,item_width];
                    
                    % �����Ʒ�����½����꣬����ջ�в�Ʒ��ϵķ�����x�����������Խ�x_item���ϲ�Ʒ��ĳ�
                    x_item=x_item+item_length;
                    y_item=y_stack;
                    
                    
                end
                
                % ����ջ���Ͻ����꣬����������ջ��ϵķ�����y�����������Խ�y_stack����ջ�Ŀ�ȣ�����Ʒ��Ŀ�
                x_stack=x_stack;
                y_stack=y_stack+item_width;
                
                
            end
            % �����������½����꣬���ڰ����������ϵķ�����x�����������Խ�x_stripe���������ĳ���
            x_stripe=x_stripe+stripe_length;
            y_stripe=y_stripe;
        end

    end
%     All_Layout_plan(i)={Layout_plan};
% end

    %% ��������
%     save strcat(ques1.mat) ������
BEST_VAL = size(plate,1);
BEST_LAYOUT = Layout_plan;
fprintf('��Ҫ�������%d.csv\n',BEST_VAL);
percent = sum(table2array(data(:,7)))/(BEST_VAL*plate_length*plate_width);
fprintf('���������%.2f\n',percent);

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
fprintf('�����Ѿ����浽cut_program_A%d.csv',DataIdx);

%% ����ͼ��

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

%% ����ͼ��չʾ���
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




