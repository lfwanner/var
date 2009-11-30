boards=5;
cd output;
files=sortrows(ls);
modes=length(files)/boards;

for i = 1:length(files)
  file = dlmread(strtrim(files(i,:)));
  current(i,:) = file(:,2)';
  voltage(i,:) = file(:,3)';
  power(i,:) = voltage(i,:).*current(i,:);
  avg_curr(i) = mean(current(i,:));
  if(avg_curr(i)<0)
  	current(i,:) = -(current(i,:));
  	avg_curr(i) = -(avg_curr(i));
  end
  avg_volt(i) = mean(voltage(i,:));	
  avg_power(i) = mean(power(i,:));	
end

time = file(:,1);
cd ../plots/


for m=1:modes
  filename = strtrim(files(m,:))(3:end);
  for b=0:boards-1
    idx = m+(b*modes);
	curr_plot(b+1,:)=current(idx,:);
	curr_avg_plot(m,b+1)=avg_curr(idx);
  end
  plot(time,curr_plot, 'x');
  axis([min(time), max(time), min(curr_plot(:)), max(curr_plot(:))]);
  title(filename);  xlabel('time (s)'); ylabel('current (A)'); 
  f = strcat(filename,'_current.pdf');
  print(f);
end

for m=1:modes
  subplot(ceil(modes/2),2,m);
  barh(curr_avg_plot(m,:)./max(curr_avg_plot(m,:))*100);
  title(strtrim(files(m,:))(3:end));
end
print('current_avg_relative.eps');

for m=1:modes
  subplot(ceil(modes/2),2,m);
  barh(curr_avg_plot(m,:));
  title(strtrim(files(m,:))(3:end));
end
print('current_avg.eps');




cd ..