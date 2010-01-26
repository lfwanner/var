boards=5;
temperatures=9;


average_power=zeros(boards,temperatures);
temperature=zeros(boards,temperatures);


cd output;
files=sortrows(ls);

idx_temperature = 0;
last_board = -1

for i = 1:size(files,1)
  filename = strtrim(files(i,:));
  board = str2num(filename(2));
  tempe = str2num(filename(13:14));
  if(last_board != board)
    idx_temperature = 0;
  end
  idx_temperature = idx_temperature + 1;
  row = board;
  col = idx_temperature;
  
  file = dlmread(filename);
  current = file(:,2)'.*1000;
  voltage = file(:,3)';
  avg_curr = mean(current);  
  if(avg_curr < 0)
    current = -current;
  end 
  power = voltage.*current;
  average_power(row,col) = mean(power);	 
  temperature(row,col) = tempe;
  last_board = board;
end


average_power=average_power./average_power.*average_power;
plot(temperature',average_power')
axis([22, 60, 0.04, .31]);
legend('b1', 'b2','b3','b4','b5',4);
ylabel('Power (mW)');
xlabel('Temperature (°C)')


%dlmwrite('average_power',average_power, '\t')
%dlmwrite('average_dmips',average_dmips, '\t')


