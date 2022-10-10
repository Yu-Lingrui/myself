function result=MyArrangeFunc(rectangles,t_width,t_length)

result.config=[];
result=repmat(result,1,length(rectangles));
rect_st=rectangles;
for ii=1:size(rect_st,1)
    if (rect_st(ii,1)>rect_st(ii,2))
        rect_st(ii,:)=rect_st(ii,[2 1]);
    end
end

[~,idx2] = sort(rect_st(:,1).*rect_st(:,2),'descend');
idx2=idx2';
x= 0;y=0;w=0;h=0;H = 0;

while ~isempty(idx2)
    Ind0=find((rect_st(idx2,2)>t_width & H+rect_st(idx2,2)<t_length) ...
        | (rect_st(idx2,2)<t_width & H+rect_st(idx2,1)<t_length),1);
    if (isempty(Ind0))
        break;
    end
    idx = idx2(Ind0);
    
    idx2(Ind0)=[];
    r = rect_st(idx,:); 
    if (r(2)>t_width)  
            result(idx).config = [x, y, r(1), r(2)];
            x= r(1);y=H;w=t_width - r(1);h=r(2);H = H + r(2);
    else 
            result(idx).config = [x, y, r(2), r(1)];
            x= r(2);y=H;w=t_width - r(2);h=r(1);H = H + r(1); 
    end
    [idx2,result]=MyArrangeFunc2(x, y, w, h, 0, rect_st, idx2, result,0,0);
    
    x=0;y=H;
end
function [indices,result]=MyArrangeFunc2(x, y, w, h, D, rm, indices, result,dc,dgw)

dc=dc+1;
p1 = 6.1;

for idx=indices
    if (dc==1)
        for j =1:D + 2
            if p1 > 1 && rm(idx,mod(j-1,2)+1) == w && rm(idx,mod(j,2)+1)  == h
                p1=1;
                orientation= j;
                best =idx;
                break;
            elseif p1 > 2 && rm(idx,mod(j-1,2)+1) < w && rm(idx,mod(j,2)+1)  == h
                p1=2;
                orientation= j;
                best =idx; 
            elseif p1 > 3 && rm(idx,mod(j-1,2)+1) == w && rm(idx,mod(j,2)+1)  > 0.75*h && rm(idx,mod(j,2)+1)  <h
                p1=3;
                orientation= j;
                best =idx;
            elseif p1 >= 4 && rm(idx,mod(j-1,2)+1) <= w && rm(idx,mod(j,2)+1)  < h
                whz=((rm(idx,mod(j-1,2)+1)/w*rm(idx,mod(j,2)+1)/h)+rm(idx,mod(j,2)+1)/h)/2;
                if (5-whz<p1)
                    p1=5-whz;
                    orientation= j;
                    best =idx;
                end
            elseif p1 > 5
                p1=5;
                orientation= j;
                best =idx;
            end
        end
    else
        
        for j =1:D + 2
            if p1 > 1 && rm(idx,mod(j-1,2)+1) == dgw && dgw==w  && rm(idx,mod(j,2)+1)  == h
                p1=1;
                orientation= j;
                best =idx;
                break;
            elseif p1 > 3 && rm(idx,mod(j-1,2)+1) == dgw && dgw==w  && rm(idx,mod(j,2)+1)  < h
                p1=3;
                orientation= j;
                best =idx; 
             elseif p1 > 2 && rm(idx,mod(j-1,2)+1) ==dgw && rm(idx,mod(j,2)+1)  == h
                p1=2;
                orientation= j;
                best =idx;
            elseif p1 > 4 && rm(idx,mod(j-1,2)+1)==dgw && rm(idx,mod(j,2)+1)  < h
                p1=4;
                orientation= j;
                best =idx;
            elseif p1 > 5
                p1=5;
                orientation= j;
                best =idx;
            end
        end
    end
end

if (p1<5)
    if orientation == 1
        omega=rm(best,1); d=rm(best,2);
    else
        omega=rm(best,2); d=rm(best,1);
    end
    result(best).config = [x, y, omega, d];
    if (dc==1)
        dgw=omega;
    end
    indices(indices==best)=[];
    if p1 == 3
        [indices,result]=MyArrangeFunc2(x, y + d, w, h - d, D, rm, indices, result,dc,dgw);
    elseif p1 == 2
        [indices,result]=MyArrangeFunc2(x + omega, y, w - omega, h, D, rm, indices, result,0,0);
    elseif p1 >=4 && p1<5
        min_w = inf;
        min_h = inf;
        for idx=indices
            min_w = min(min_w, rm(idx,1));
            min_h = min(min_h, rm(idx,2));
        end
        min_w = min(min_h, min_w);
        min_h = min_w;
        if w - omega < min_w
            [indices,result]=MyArrangeFunc2(x, y + d, w, h - d, D, rm, indices, result,dc,dgw);
        elseif h - d < min_h
            [indices,result]=MyArrangeFunc2(x + omega, y, w - omega, h, D, rm, indices, result,0,0);
        elseif omega < min_w
            [indices,result]=MyArrangeFunc2(x + omega, y, w - omega, d, D, rm, indices, result,dc,dgw);
            [indices,result]=MyArrangeFunc2(x, y + d, w, h - d, D, rm, indices, result,dc,dgw);
        else
            [indices,result]=MyArrangeFunc2(x, y + d, omega, h - d, D, rm, indices, result,dc,dgw);
            [indices,result]=MyArrangeFunc2(x + omega, y, w - omega, h, D, rm, indices, result,0,0);
        end
    end
end

