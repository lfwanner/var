file = dlmread('output/tmp');
current = file(:,2)'(1:end-1);
voltage = file(:,3)'(1:end-1);
ac = mean(current); 
av = mean(voltage); 	
dlmwrite('output/av', av)
dlmwrite('output/ac', ac)


