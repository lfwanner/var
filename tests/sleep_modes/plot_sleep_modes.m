boards=5;
cd output_no_dbgu;
files=sortrows(ls);
modes=length(files)/boards;

for i = 1:length(files)
  file = dlmread(strtrim(files(i,:)));
  current(i,:) = file(:,2)'.*1000;
  voltage(i,:) = file(:,3)';
  avg_curr(i) = mean(current(i,:));
  if(avg_curr(i)<0)
  	current(i,:) = -(current(i,:));
  	avg_curr(i) = -(avg_curr(i));
  end 
  power(i,:) = voltage(i,:).*current(i,:);  
  avg_volt(i) = mean(voltage(i,:));	
  avg_power(i) = mean(power(i,:));	
end

time = file(:,1);


power_matrix=zeros(boards,modes);

for m=1:modes
  for b=0:boards-1
    idx = m+(b*modes);
        power_matrix(b+1,m) = avg_power(idx); 
  end
end


dlmwrite('average_power',power_matrix, '\t')


cd ../plots/








max_curr_range = 1:modes;
min_curr_range = 1:modes;

subplot_idx = [1, 3, 5, 7, 2, 4, 6, 8];


for m=1:modes
  filename = strtrim(files(m,:))(6:end);
  for b=0:boards-1
    idx = m+(b*modes);
	curr_plot(b+1,:)=current(idx,:);
	power_plot(b+1,:)=power(idx,:);
	curr_avg_plot(m,b+1)=avg_curr(idx);	
	power_avg_plot(m,b+1)=avg_power(idx);	
  end
  
  overall_avg_power = mean(power_avg_plot(m,:));

  
    if( overall_avg_power >60)
        max_range(m) = 135;
        min_range(m) = 130;
    else if( overall_avg_power >3)
        max_range(m) = 47;
        min_range(m) = 44;        
    else if( overall_avg_power >0.3)
        max_range(m) = 2.5;
        min_range(m) = 0.0;
    else if( overall_avg_power >0.02)
        max_range(m) = 1.5;
        min_range(m) = -0.5;
    else
        max_range(m) = 2;
        min_range(m) = -0.5;
    end;   end;   end;   end;


  subplot(ceil(modes/2),2,subplot_idx(m));
  plot(time,power_plot, 'x');
  axis([min(time), max(time), min_range(m), max_range(m)]);
  title(filename);  

  if (mod(subplot_idx(m),2) == 1)
      ylabel('Power (mW)'); 
  end  
  
  if (subplot_idx(m) > 6)
    xlabel('Time (s)'); 
  end    
  
  if (subplot_idx(m) < 2)
    legend('b1', 'b2','b3','b4','b5');
  end    
end
print('power.eps','-deps','-color');

for m=1:modes
  subplot(ceil(modes/2),2,subplot_idx(m));
  barh(power_avg_plot(m,:)./max(power_avg_plot(m,:))*100);
  title(strtrim(files(m,:))(6:end));
  
  
  
  if (mod(subplot_idx(m),2) == 1)
    ylabel('Board #');   
  end  
  
  if (subplot_idx(m) > 6)
    xlabel('Relative Power (% of Max)'); 
  end  
  
end
print('power_avg_relative.eps','-deps','-color');

for m=1:modes
  overall_avg_power = mean(power_avg_plot(m,:));
    if( overall_avg_power >3)
        max_range(m) = 150;
        min_range(m) = 10;
    else if( overall_avg_power >0.3)
        max_range(m) = 1.5;
        min_range(m) = 0.0;
    else if( overall_avg_power >0.02)
        max_range(m) = 0.25;
        min_range(m) = 0;
    else
        max_range(m) = 0.025;
        min_range(m) =0;
    end;   end;   end;   

  subplot(ceil(modes/2),2,subplot_idx(m));
  barh(power_avg_plot(m,:));
  axis([0, max_range(m), 0, 6])
  title(strtrim(files(m,:))(6:end));


  if (mod(subplot_idx(m),2) == 1)
    ylabel('Board #');   
  end  
  
  if (subplot_idx(m) > 6)
    xlabel('Power (mW)'); 
  end
  
  
end
print('power_avg.eps','-deps','-color');




cd ..
