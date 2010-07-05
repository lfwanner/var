files = sortrows(ls);


for i = 1:size(files,1)
  filename = strtrim(files(i,:));
  file = dlmread(filename);
  current = file(:,2)'.*1000;
  avg_curr = mean(current);  
  if(avg_curr < 0)
    current = -current;
  end 
  voltage = file(:,3)';
  power = voltage.*current;
  average_power(i) = mean(power);	 
end

