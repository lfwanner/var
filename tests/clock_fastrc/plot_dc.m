sleep_power = dlmread('output_no_dbgu/average_power');

# XIN = 12Mhz, Clock = 36Mhz
active = sleep_power(:,8);
# Sleep @ 32kHz
sleep = sleep_power(:,4);

lifetime=500:100:50000;
max_power=5.4*3.3*1000./lifetime;

clear dc;e
for i = 1:length(lifetime)
  dc(:,i) = 100*(max_power(i) .- sleep) ./  (active - sleep);
end

plot(lifetime(:,20:end),dc(:,20:end)');
xlabel('Lifetime (hours)');
ylabel('Duty Cycle (%)');
title('Active @ 36 Mhz, Sleep @ 32kHz, 5.4 A, 3.3 V');
print('plots/dc_by_lifetime.eps','-deps','-color');

for i = 1:length(lifetime)
  rdc(:,i) = 100*dc(:,i)./max(dc(:,i));
end
plot(lifetime,rdc');
xlabel('Lifetime (hours)');
ylabel('Relative Duty Cycle (% of max)');
title('Active \@ 48 Mhz, Sleep \@ 32kHz, Battery: 5.4 A, 3.3 V');
print('plots/dc_relative_by_lifetime.eps','-deps','-color');

