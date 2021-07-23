import xlrd
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
from mpl_toolkits import mplot3d
import csv

cor = {
	0: 'b',
	1: 'k',
	2: 'r',
	3: 'g',
	4: 'c',
	5: 'sienna',
	6: 'grey',
	7: 'y',
	8: 'violet',
	9: 'coral',
	10:'darkblue',
	11:'darkgreen',
	12:'indigo',
	13:'maroon',
	14:'navy',
	15:'olive',
	16:'orangered',
	17:'sienna',
	18:'violet',
}
font = {'family' : 'serif', 'size'   : 10}
matplotlib.rc('font', **font)
fig = plt.figure()
ax = plt.axes(projection='3d')

cormap = range(19)
cormap = list(cormap)
initi = 0
fx = []
fy = []
ft = []
fl = []
def turn(x,y):
	pi = 3.1415926
	zH = 400.0;
	zpi = 3.14159;
	zal = 52.0;
	za = zal*pi / 180.0;
	zx = np.arctan(np.tan(za) - 404.49 / 400.0);
	zb = za - zx;
	zbl = zb * 180.0 /pi;
	zabl = zal + zbl;
	zr = zH*(np.tan(zb + za) - np.tan(za - zb));
	zfocal = 27.0 / 100.0 / 0.3048 / 10.0;
	zhs = zfocal * np.tan(zb) * 2;
	zws = zhs * 4096.0 / 2160.0;
	zWS = zws;
	zHS = zhs;
	zXS0 = zWS / 2.0;
	zYS0 = zHS / 2.0;
	zFF = zfocal;
	zHH = zH;
	zAL = za;
	zBE = zb;

	WI = 4096; HI = 2160;
	XS0 = zWS / 2; YS0 = zHS / 2;
	xp = x*zWS / WI - XS0;
	yp = y*zHS / HI - YS0;
	cs = np.cos(zAL)
	ss = np.sin(zAL)
	t = zHH / cs; fy = zFF / cs;
	xm = 1
	ym = 1
	qy = int(t*yp/(fy + ss * yp) * ym);
	qx = int(xp * (t - ss * qy) / zFF * xm);
	mult = 2
	return qx*1.5,qy*3

with open('lanenumbered.csv') as csvDataFile:
	csvReader = csv.reader(csvDataFile)
	tt = []
	xx = []
	yy = []
	ln = -1
	cn = -1
	for row in csvReader:
		cnum = int(row[0])
		time = float(row[1])
		cx = int(row[2])
		cy = int(row[3])
		lane = int(row[4])
		if ln == -1:
			ln = lane
			cn = cnum
			qx,qy = turn(cx,cy)
			tt.append(time/25)
			xx.append(qx)
			yy.append(qy)
		if lane != ln or cnum != cn:
			fx.append(xx)
			fy.append(yy)
			ft.append(tt)
			fl.append(ln)
			tt = []
			xx = []
			yy = []
			ln = lane
			cn = cnum
		elif cy > 2160*0.45 and cx > 4096*0.4 and (cy-2160*0.45)*0.4 < 700 and (cx-4096*0.4)*0.4 < 800:
			tt.append(time/25)
			qx,qy = turn(cx,cy)
			xx.append(qx)
			yy.append(qy)

for i in range(len(fx)):
	bb = 0
	for j in range(len(fx[i]) - 1):
		if len(fx[i]) < 100 or fl[i] == 0:
			bb = 1
			break
	if bb:
		continue
	ax.plot(fx[i],fy[i],ft[i],linewidth = 2, linestyle = '-', color = cor[fl[i]],label = 'lane ' + str(fl[i]) if fl[i] in cormap else "")
	if fl[i] in cormap:
		cormap.remove(fl[i])
plt.xlim(0,800)
plt.ylim(0,800)
plt.legend(loc=2)
plt.xlabel("X (ft)")
plt.ylabel("Y (ft)")
ax.set_zlabel("T (s)")
#plt.xticks(np.arange(0, 600, step=200))
#plt.yticks(np.arange(0, 300, step=100))
#ax.set_zticks(np.arange(0, 200, step=50))
#plt.zlabel("Y (ft)")
plt.show()
plt.savefig('lanenumberedtra1.png')
