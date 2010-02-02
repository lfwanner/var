boards=5;
temperatures=6;
multipliers=10;
cd output;

average_power=zeros(boards*multipliers,temperatures);
temperature=zeros(boards*multipliers,temperatures);

delta_over_mu=zeros(boards*multipliers,temperatures);

xtal_base = 12;

files=sortrows(ls);

for i = 1:length(files)-1
  filename = strtrim(files(i,:));
  if filename(1) == 'b'
	  f_board = str2num(filename(2));
	  idx_  = findstr (filename, "_");
	  f_clock = str2num(filename(idx_(1)+1:idx_(2)-1));
	  f_tempe = str2num(filename(idx_(2)+1:end));
	  
	  row=f_board*f_clock/xtal_base;
	  col=min(find(average_power(row,:)==0));

	  file = dlmread(filename);
	  current = file(:,2)'.*1000;
	  voltage = file(:,3)';
	  avg_curr = mean(current);  
	  if(avg_curr < 0)
	  	current = -current;
	  end 
	  power = voltage.*current;
	  average_power(row,col) = mean(power);	 
	  temperature(row,col) = f_tempe;  
  end 
end  

for i = 1:boards*multipliers
	delta_over_mu(i,:) = (average_power(i,:)-mean(average_power(i,:)))./mean(average_power(i,:));
end  

dlmwrite('average_power',average_power, '\t')
dlmwrite('average_dmips',average_dmips, '\t')




system('mv b* raw/')
system('mv dm* raw/')
