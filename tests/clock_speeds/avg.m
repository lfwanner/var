boards=5;
xtals=5;
multipliers=10;
cd output;

average_power=zeros(boards*xtals,multipliers);
average_dmips=zeros(boards*xtals,multipliers);

xtal_base = 12;
xtal_increment = 2;

files=sortrows(ls);

for i = 1:length(files)
  filename = strtrim(files(i,:));
  if filename(1) == 'd'
	  f_board = str2num(filename(7));
	  f_xtal = str2num(filename(8:9));
	  row = (f_board-1)*xtals+(f_xtal-xtal_base)/xtal_increment+1;
	  file = dlmread(filename);
	  mips = file(:,2)';
      average_dmips(row,1:length(mips)) = mips;
  end
  if filename(1) == 'b'
	  f_board = str2num(filename(2));
	  f_xtal = str2num(filename(4:5));
	  f_clock = str2num(filename(7:end));
	  row = (f_board-1)*xtals+(f_xtal-xtal_base)/xtal_increment+1;
	  col = f_clock/f_xtal;
	  file = dlmread(filename);
	  current = file(:,2)'.*1000;
	  voltage = file(:,3)';
	  avg_curr = mean(current);  
	  if(avg_curr < 0)
	  	current = -current;
	  end 
	  power = voltage.*current;
	  average_power(row,col) = mean(power);	   
  end 
end    

dlmwrite('average_power',average_power, '\t')
dlmwrite('average_dmips',average_dmips, '\t')

system('mv b* raw/')
system('mv dm* raw/')
