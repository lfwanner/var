power = dlmread('output_no_dbgu/average_power');

boards = size(power,1);
modes  = size(power,2); 

power_offset = zeros(boards,modes);
power_relative = zeros(boards,modes);

for i = 1:modes
  power_offset(:,i) = power(:,i) .- mean(power(:,i));
  power_relative(:,i) = 100 * power(:,i) ./ max(power(:,i));  
end

semilogy(power');
legend('b1', 'b2','b3','b4','b5',4);
xlabel('Power Mode');
ylabel('Power (mW)');
print('plots/power_by_mode_log.eps','-deps','-color');

plot(power_offset'); 
legend('b1', 'b2','b3','b4','b5',2);
xlabel('Power Mode');
ylabel('Power offset from mean (mW)');
print('plots/power_offset_by_mode.eps','-deps','-color');


plot(power_relative'); 
legend('b1', 'b2','b3','b4','b5',4);
xlabel('Power Mode');
ylabel('Relative Power (% of max)');
axis([1, 8, 0, 110]);
print('plots/power_relative_by_mode.eps','-deps','-color');


