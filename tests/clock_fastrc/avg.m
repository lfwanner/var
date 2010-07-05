boards=5;
multipliers=8;
cd output;

average_power=zeros(boards,multipliers);

xtal_base = 12;
xtal_increment = 2;

files=sortrows(ls);

col = 1;

frequencies = [0.125, 0.250, 0.500, 1, 2, 4, 8, 12];

for i = 1:length(files)
  filename = strtrim(files(i,:));
  f_board = str2num(filename(2));
  f_freq =  str2num(filename(4:end));
  file = dlmread(filename);
  current = file(:,2)'.*1000;
  voltage = file(:,3)';
  avg_curr = mean(current);
  if(avg_curr < 0)
    current = -current;
  end
  power = voltage.*current;
  average_power(f_board,col) = mean(power);
  col = col + 1;
  if(col > 8)
    col = 1;
  end
end

dlmwrite('average_power',average_power, '\t')
system('mv b* raw/')

plot(frequencies, average_power');
legend('b1', 'b2','b3','b4','b5',4);
xlabel('Core Frequency (MHz)');
ylabel('Power (mW)');
print('../plots/power_by_clock.eps','-deps','-color');


relative_power =zeros(boards,multipliers);
offset_power =zeros(boards,multipliers);
	 
	 
for i = 1:multipliers	 
	 relative_power(:,i) = 100*average_power(:,i)./max(average_power(:,i));
	 offset_power(:,i) = average_power(:,i) - mean(average_power(:,i));
	 
end	 
	 

plot(frequencies, relative_power');
legend('b1', 'b2','b3','b4','b5',4);
axis([ 0 12 80 105 ])
xlabel('Core Frequency (MHz)');
ylabel('Relative Power (% of max)');
print('../plots/relative_power_by_clock.eps','-deps','-color');

plot(frequencies, offset_power');
legend('b1', 'b2','b3','b4','b5',4);
xlabel('Core Frequency (MHz)');
ylabel('Power offset from mean (mW)');
print('../plots/offset_power_by_clock.eps','-deps','-color');
	 
	 
	 

