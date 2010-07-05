sleep_power = dlmread('output/average_power');

# XIN = 12Mhz, Clock = 36Mhz
active = sleep_power(:,8);
# Sleep @ 32kHz
sleep = sleep_power(:,4);

lifetime=10000:100:30000;
max_power=5.4*3.3*1000./lifetime;

clear dc;
for i = 1:length(lifetime)
  dc(:,i) = 100*(max_power(i) .- sleep) ./  (active - sleep);
end

plot(lifetime(:,20:end),dc(:,20:end)');
xlabel('Lifetime (hours)');
ylabel('Duty Cycle (%)');
title('Active @ 36 MHz, Sleep @ 32kHz, 5.4 A, 3.3 V');
legend('Board 1','Board 2','Board 3','Board 4','Board 5')
print('plots/dc_by_lifetime.eps','-deps','-color');

for i = 1:length(lifetime)
  rdc(:,i) = 100*dc(:,i)./max(dc(:,i));
end
plot(lifetime,rdc', "linewidth", 4);
xlabel('Lifetime (hours)');
ylabel('Relative Duty Cycle (% of max)');
axis([10000,30000,60,101])
title('Active @ 36 MHz, Sleep @ 32kHz);
legend('Board 1','Board 2','Board 3','Board 4','Board 5',3)
print('plots/dc_relative_by_lifetime.eps','-deps','-color','-F:16');

