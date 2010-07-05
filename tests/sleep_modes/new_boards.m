f1 = dlmread('output/b1-4-sleep32khz');
f2 = dlmread('output/b2-4-sleep32khz');
f3 = dlmread('output/b3-4-sleep32khz');
f4 = dlmread('output/b4-4-sleep32khz');
f5 = dlmread('output/b5-4-sleep32khz');
f6 = dlmread('output/b6-4-sleep32khz');
f7 = dlmread('output/b7-4-sleep32khz');
f8 = dlmread('output/b8-4-sleep32khz');
f9 = dlmread('output/b9-4-sleep32khz');
f10 = dlmread('output/b10-4-sleep32khz');

p(1) = mean(f1(:,2).*f1(:,3));
p(2) = mean(f2(:,2).*f2(:,3));
p(3) = mean(f3(:,2).*f3(:,3));
p(4) = mean(f4(:,2).*f4(:,3));
p(5) = mean(f5(:,2).*f5(:,3));
p(6) = mean(f6(:,2).*f6(:,3));
p(7) = mean(f7(:,2).*f7(:,3));
p(8) = mean(f8(:,2).*f8(:,3));
p(9) = mean(f9(:,2).*f9(:,3));
p(10) = mean(f10(:,2).*f10(:,3));

p=p*1000;

barh(p);
axis([0, 0.25, 0, 11]);
ylabel('Board #'); 
xlabel('Power (mW)'); 
print('sleep_avg.pdf');


