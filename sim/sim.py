import numpy as np
from scipy import interpolate

class Battery:

	C = np.linspace(0,32000,num=9);
	V = [1.9,2.2,2.4,2.5,2.55,2.6,2.7,2.8,3.2];

	def __init__(self, cc = C, vc = V):
		self.totalCapacity = max(cc);
		self.remainingCapacity = self.totalCapacity;
		self.cc = cc;
		self.vc = vc;
		self.voltageP = np.polyfit(self.cc, self.vc,3);
		print self.voltageP
		self.capacityP =  np.polyfit(self.vc, self.cc,3);


	def drawPower(self, power, time):
		self.remainingCapacity -= power*time;
		
	def plot(self,x,y,p):
		xx = np.linspace(min(x),max(x),num=100);
		yy = np.polyval(p,xx);	
		import matplotlib.pyplot as plt
		plt.plot(x, y, 'o', xx, yy, '-');
		plt.show();
				
	def plotVoltage(self):
		self.plot(self.cc,self.vc,self.voltageP);

	def plotCapacity(self):
		self.plot(self.vc,self.cc,self.capacityP);
		
		
bat = Battery()
bat.plotVoltage();
#bat.plotCapacity();
