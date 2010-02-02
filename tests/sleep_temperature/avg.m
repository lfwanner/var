boards=5;
temperatures=8;


average_power=zeros(boards,temperatures);
temperature=zeros(boards,temperatures);
average_power_file=zeros(boards,temperatures);

temperature_label = [22, 30, 35, 40, 45, 50, 55, 60];



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
  col_file = find(temperature_label==tempe);
  
  file = dlmread(filename);
  current = file(:,2)'.*1000;
  voltage = file(:,3)';
  avg_curr = mean(current);  
  if(avg_curr < 0)
    current = -current;
  end 
  power = voltage.*current;
  average_power(row,col) = mean(power);	 
  average_power_file(row,col_file) = mean(power);	 
  temperature(row,col) = tempe;
  last_board = board;
end


average_power=average_power./average_power.*average_power;
plot(temperature',average_power')
axis([22, 60, 0.04, .31]);
legend('b1', 'b2','b3','b4','b5',4);
ylabel('Power (mW)');
xlabel('Temperature (°C)');
title('Power consumption, Sleep Mode (32Khz Slow Oscillator)');
print('../temperature.eps');

dlmwrite('avg_sleep_power_temperature', temperature_label)
dlmwrite('avg_sleep_power_temperature', average_power_file, '-append')


