%% 条带生成函数

function stripe=stripe_create(plate_width,plate_length,rectangles)

% 初始化条带记录
stripe={};

result.config=[];
result=repmat(result,1,length(rectangles));

% 初始化产品项，判断产品项的长边，按照先宽后长的顺序保存数据
remaining=rectangles;
for ii=1:size(remaining,1)
    if (remaining(ii,1)>remaining(ii,2))
        remaining(ii,:)=remaining(ii,[2 1]);
    end
end

% 将产品项按面积从大到小排序
[~,sorted_indices] = sort(remaining(:,1).*remaining(:,2),'descend');sorted_indices=sorted_indices';
remainingS=remaining(sorted_indices,:);

while ~isempty(sorted_indices)
    
    % 初始化已排样产品项记录
    already_sampling=[];
    
    % 初始化条带的左下角坐标
    x= 0;y=0;
    
    % 初始化条带剩余可用宽度和高度（板材竖着放，所以条带是宽和高）
    w=0;h=0;
    
    % 初始化当前板材被横切掉的高（板材竖着放）
    H = 0;
    
    % 在所有的剩余产品项remaining中寻找（产品项长>板材宽，且产品项长+被切掉的板材的高<板材高）
    % 或者（产品项长<板材宽，且产品项宽+被切掉的板材的高<板材高）的面积最大的产品项
    % 即在板材竖着放的情况下，寻找可以横放或竖放的面积最大的产品项
    Ind0=find((remaining(sorted_indices,2)>plate_width & H+remaining(sorted_indices,2)<plate_length) ...
        | (remaining(sorted_indices,2)<plate_width & H+remaining(sorted_indices,1)<plate_length),1);
    
    if (isempty(Ind0))
        break;
    end
    
    % 获取满足上述条件的最大的产品项
    idx = sorted_indices(Ind0);
    
    % 最大的产品项的宽和长，r(2)是长，r(1)是宽
    r = remaining(idx,:); 
    if (r(2)>plate_width)  % 产品项长>板材宽
            result(idx).config = [x, y, r(1), r(2)]; % 将产品项竖排，记录产品项左下角坐标和，宽，长数据
            % 更新条带可用部分的左下角坐标
            x= r(1);y=H;
            % 更新条带剩余可用宽度和高度（板材竖着放，所以条带是宽和高）
            w=plate_width - r(1);h=r(2);
            % 更新板材被横切掉的高（板材竖着放）
            H = H + r(2); 
    else % 产品项长<=板材宽
            result(idx).config = [x, y, r(2), r(1)]; % 将产品项横排，记录产品项左下角坐标和，长，宽数据
            % 更新条带可用部分的左下角坐标
            x= r(2);y=H;
            % 更新条带剩余可用宽度和高度（板材竖着放，所以条带是宽和高）
            w=plate_width - r(2);h=r(1);
            % 更新板材被横切掉的高（板材竖着放）
            H = H + r(1);
    end
    
    % 记录已排样产品项
    already_sampling=[already_sampling;sorted_indices(Ind0)];
    
    % 删除已完成排版的产品项
    sorted_indices(Ind0)=[];
    
    %% 调用条带剩余部分内部排样函数
    % 在条带内寻找可以放置的产品项，直到无法再放置新产品项，并计算相关数据
    [sorted_indices,result,already_sampling]=sampling_in_stripe(x, y, w, h, 0, remaining, sorted_indices, result,0,0, already_sampling);
    
    % 记录现有条带
    stripe(end+1,1)={result(already_sampling)};
    stripe(end,3)={already_sampling};
    stripe(end,2)={H};
    % 在新的板材中生成条带，直到所有产品项均完成排样
    x=0;y=0;

end
%% 条带内排样函数
%% 输入：
% x, y，条带的左下角坐标
% w, h，条带剩余可用宽度和高度（板材竖着放，所以条带是宽和高）
% remaining, 所有产品项的长宽数据
% sorted_indices，原先剩余产品项
% result，原先已排样产品项的数据
% dgcnt，
% dgw，
%% 输出：
% sorted_indices，进行排样后的剩余产品项
% result，进行排样后已排样产品项的数据

function [sorted_indices,result,already_sampling]=sampling_in_stripe(x, y, w, h, D, remaining, sorted_indices, result, dgcnt, dgw,already_sampling)

dgcnt=dgcnt+1;
% 初始化优先级
priority = 6.1;
% 按产品项面积由大到小遍历每一个产品项
for idx=sorted_indices
    if (dgcnt==1)
        for j =1:D + 2
            if priority > 1 && remaining(idx,mod(j-1,2)+1) == w && remaining(idx,mod(j,2)+1)  == h
                priority=1;
                orientation= j;
                best =idx;
                break;
            elseif priority > 2 && remaining(idx,mod(j-1,2)+1) < w && remaining(idx,mod(j,2)+1)  == h
                priority=2;
                orientation= j;
                best =idx; 
            elseif priority > 3 && remaining(idx,mod(j-1,2)+1) == w && remaining(idx,mod(j,2)+1)  > 0.75*h && remaining(idx,mod(j,2)+1)  <h
                priority=3;
                orientation= j;
                best =idx;
            elseif priority >= 4 && remaining(idx,mod(j-1,2)+1) <= w && remaining(idx,mod(j,2)+1)  < h
                whz=((remaining(idx,mod(j-1,2)+1)/w*remaining(idx,mod(j,2)+1)/h)+remaining(idx,mod(j,2)+1)/h)/2;
                if (5-whz<priority)
                    priority=5-whz;
                    orientation= j;
                    best =idx;
                end
            elseif priority > 5
                priority=5;
                orientation= j;
                best =idx;
            end
        end
    else
        
        for j =1:D + 2
            if priority > 1 && remaining(idx,mod(j-1,2)+1) == dgw && dgw==w  && remaining(idx,mod(j,2)+1)  == h
                priority=1;
                orientation= j;
                best =idx;
                break;
            elseif priority > 3 && remaining(idx,mod(j-1,2)+1) == dgw && dgw==w  && remaining(idx,mod(j,2)+1)  < h
                priority=3;
                orientation= j;
                best =idx; 
             elseif priority > 2 && remaining(idx,mod(j-1,2)+1) ==dgw && remaining(idx,mod(j,2)+1)  == h
                priority=2;
                orientation= j;
                best =idx;
            elseif priority > 4 && remaining(idx,mod(j-1,2)+1)==dgw && remaining(idx,mod(j,2)+1)  < h
                priority=4;
                orientation= j;
                best =idx;
            elseif priority > 5
                priority=5;
                orientation= j;
                best =idx;
            end
        end
    end
end

if (priority<5)
    if orientation == 1
        omega=remaining(best,1); d=remaining(best,2);
    else
        omega=remaining(best,2); d=remaining(best,1);
    end
    result(best).config = [x, y, omega, d];
    if (dgcnt==1)
        dgw=omega;
    end
    
    % 记录已排样产品项
    already_sampling=[already_sampling;sorted_indices(sorted_indices==best)];
    
    % 删除已完成排版的产品项
    sorted_indices(sorted_indices==best)=[];
    
    
    
    if priority == 3
        [sorted_indices,result,already_sampling]=sampling_in_stripe(x, y + d, w, h - d, D, remaining, sorted_indices, result,dgcnt,dgw,already_sampling);
    elseif priority == 2
        [sorted_indices,result,already_sampling]=sampling_in_stripe(x + omega, y, w - omega, h, D, remaining, sorted_indices, result,0,0,already_sampling);
    elseif priority >=4 && priority<5
        min_w = inf;
        min_h = inf;
        for idx=sorted_indices
            min_w = min(min_w, remaining(idx,1));
            min_h = min(min_h, remaining(idx,2));
        end
        min_w = min(min_h, min_w);
        min_h = min_w;
        if w - omega < min_w
            [sorted_indices,result,already_sampling]=sampling_in_stripe(x, y + d, w, h - d, D, remaining, sorted_indices, result,dgcnt,dgw,already_sampling);
        elseif h - d < min_h
            [sorted_indices,result,already_sampling]=sampling_in_stripe(x + omega, y, w - omega, h, D, remaining, sorted_indices, result,0,0,already_sampling);
        elseif omega < min_w
            [sorted_indices,result,already_sampling]=sampling_in_stripe(x + omega, y, w - omega, d, D, remaining, sorted_indices, result,dgcnt,dgw,already_sampling);
            [sorted_indices,result,already_sampling]=sampling_in_stripe(x, y + d, w, h - d, D, remaining, sorted_indices, result,dgcnt,dgw,already_sampling);
        else
            [sorted_indices,result,already_sampling]=sampling_in_stripe(x, y + d, omega, h - d, D, remaining, sorted_indices, result,dgcnt,dgw,already_sampling);
            [sorted_indices,result,already_sampling]=sampling_in_stripe(x + omega, y, w - omega, h, D, remaining, sorted_indices, result,0,0,already_sampling);
        end
    end
end

