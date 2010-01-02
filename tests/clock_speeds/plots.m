power = dlmread('output/average_power');
dmips = dlmread('output/average_dmips'); 

mpw = dmips./power;
power = power + (mpw-mpw);
dmips = dmips + (mpw-mpw);

boards=5;
xtals=5;
xtal_speeds = [12, 14, 16, 18, 20];
multipliers = 1:10;

clock=zeros(boards*xtals,10);

for b = 1:boards
  for x = 1:xtals
    clock((b-1)*xtals+x,:) = xtal_speeds(x)*multipliers;
  end
end

for b = 1:boards
    subplot(3,2,b);
	start = (b-1)*xtals+1; finish=b*xtals;
	xx = clock(start:finish,:)';
	yy = power(start:finish,:)';
	plot(xx,yy);
	axis([12, 128, 0, 250]);
	legend('xin=12Mhz', 'xin=14Mhz','xin=16Mhz','xin=18Mhz','xin=20Mhz',4);
	if(b > 3)	
	  xlabel('Core Frequency (MHz)');
	end  
	if(mod(b,2) == 1)
	  ylabel('Power (mW)');
	end  
	title(strcat('b',int2str(b)));
	print('plots/power_by_board.eps','-deps','-color');
end




order = [1, 6, 11, 16, 21, 2, 7, 12, 17, 22, 3, 8, 13, 18, 23, 4, 9, 14, 19, 24, 5, 10, 15, 20, 25];

bmpw = zeros(25,10);
bpower = zeros(25,10);
bdmips = zeros(25,10);
bclock = zeros(25,10);

for i = 1:25
  bclock(order(i),:) = clock(i,:);
  bmpw(order(i),:) = mpw(i,:);
  bpower(order(i),:) = power(i,:);
  bdmips(order(i),:) = dmips(i,:);    
end


for x = 1:xtals
    subplot(3,2,x);
	start = (x-1)*xtals+1; finish=x*xtals;
	xx = bclock(start:finish,:)';
	yy = bpower(start:finish,:)';
	plot(xx,yy);
	axis([12, 128, 0, 250]);
	legend('b1', 'b2','b3','b4','b5',4);
	if(x > 3)	
	  xlabel('Core Frequency (MHz)');
	end  
	if(mod(x,2) == 1)
	  ylabel('Power (mW)');
	end  
	title(strcat('xin=',int2str(xtal_speeds(x))))
	print('plots/power_by_xin.eps','-deps','-color');
end


rbmpw = zeros(25,10);
rbpower = zeros(25,10);
rbdmips = zeros(25,10);
rbclock = zeros(25,10);

for b = 1:boards
  for c = 1:10
    start = (b-1)*5+1;
    finish = start+4;
    r = bpower(start:finish,c)(finite(bpower(start:finish,c)));
    max_c = max(r);
    if (max_c != NaN)
       rbpower(start:finish,c) = bpower(start:finish,c)./max_c*100;
	end
  end
end

rbpower = rbpower + (bmpw-bmpw);

for x = 1:xtals
    subplot(3,2,x);
	start = (x-1)*xtals+1; finish=x*xtals;
	xx = bclock(start:finish,:)';
	yy = rbpower(start:finish,:)';
	plot(xx,yy);
	axis([12, 128, 0, 110]);
	legend('b1', 'b2','b3','b4','b5',4);
	if(x > 3)	
	  xlabel('Core Frequency (MHz)');
	end  
	if(mod(x,2) == 1)
	  ylabel('Power (% of greatest power)');
	end  
	title(strcat('xin=',int2str(xtal_speeds(x))))
	print('plots/power_relative_by_xin.eps','-deps','-color');
end
