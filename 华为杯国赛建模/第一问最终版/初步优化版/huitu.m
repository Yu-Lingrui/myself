clc
figure
set(gca,'Fontsize',18);
plot(Loss1,'g--s','Linewidth',1,'MarkerFaceColor','c');
%            线形      线宽             是否填充      颜色
hold on
plot(Loss2,'m--h','Linewidth',1,'MarkerFaceColor','m');
hold on
% plot(Loss3,'b--0','Linewidth',1,'MarkerFaceColor','g');
grid on
hold on
axis([1 100 100 110])
legend('遗传','pso')
title('最优个体适应值-性能指标','fontsize',18);
xlabel('迭代次数','fontsize',18);ylabel('目标损失','fontsize',18);