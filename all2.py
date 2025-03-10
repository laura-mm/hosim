# fiveplots for higher order
import numpy as np
import matplotlib.pyplot as plt
import math
#plt.rc('text', usetex=True)
#plt.rcParams["text.latex.preamble"].append(r'\usepackage{xfrac}')
#plt.rc('xtick',labelsize=20)
#plt.rc('ytick',labelsize=20)
#plt.rc('axes', labelsize=20)
#plt.rc('axes', titlesize=20)
#plt.rc('legend', fontsize=20)
grid = 30
col = ['m', 'r', 'y', 'g', 'c', 'b']
colo = ['m.', 'r.', 'y.', 'g.', 'c.', 'b.']
lab = ['$\gamma = -\frac{1}{p-1}$', '$\gamma = 0$', '$\gamma = 1$']
mark = ['s', 'D', '^', 'v', '<', '>']
#lett = ['$(a)$', '$(b)$', '$(c)$', '$(d)$', '$(e)$', '$(f)$']

# sigma, z1, phi, M, q, help for measfp
# phi, M, q, diversity, dsq, h, for meas

muval = [-4.0, -3.5, -3.0, -2.5, -2.0, -1.5, -1.0, -0.5, 0.0, 0.5, 1.0, 1.5]
gammaval = [-1.0, -0.8, -0.5, 0.0, 0.5, 1.0]
x = np.linspace(-1.0, 1.0, 21)
sigmaval = np.power(10, x)


points = [] # gi, 012, mi
for gi in range(6):
	p = np.loadtxt("points2_{0}.txt".format(gi), delimiter = ",")
	p = p.reshape(12, 3)
	p = np.transpose(p)
	points.append(p)
	

# phi, M, diversity fiveplots
###############################################################

meas = np.loadtxt("allfive2.txt", delimiter=",")
meas = meas.reshape(12, 6, 21, 6)
meas = np.transpose(meas, (0, 1, 3, 2)) # mu, gam, meas, sig




for mi in range (3):

	mi += 9
	
	plt.figure(mi + 1, figsize = (6, 12))
	#plt.subplots(5, 1, figsize=(6,15))

	
	measfp = []
	for gi in range(6):
		g = np.loadtxt("measfp2_{0}_{1}.txt".format(mi, gi), delimiter = ",")
		g = g.reshape(len(g)//6, 6)
		g = np.transpose(g)
		measfp.append(g)

	plt.subplot(5, 1, 1) # phi
	for gi in range(6):
		plt.semilogx(measfp[gi][0], measfp[gi][2], col[gi])
		plt.semilogx(sigmaval, meas[mi][gi][0], colo[gi], label = "g = {0}".format(gammaval[gi]))             
		plt.axvline(x = points[gi][0][mi], linestyle = '--', color = col[gi])
		plt.axvline(x = points[gi][1][mi], linestyle = ':', color = col[gi])
		plt.axvline(x = points[gi][2][mi], linestyle = ':', color = col[gi])
	plt.xlim([10**(-1.0), 10**1.0])
	plt.ylabel("phi")
	plt.title("mu = {0}".format(muval[mi]))

	plt.subplot(5, 1, 2) # M
	for gi in range(6):
		plt.semilogx(measfp[gi][0], measfp[gi][3], col[gi])
		plt.semilogx(sigmaval, meas[mi][gi][1], colo[gi], label = "g = {0}".format(gammaval[gi]))
		plt.axvline(x = points[gi][0][mi], linestyle = '--', color = col[gi])
		plt.axvline(x = points[gi][1][mi], linestyle = ':', color = col[gi])
		plt.axvline(x = points[gi][2][mi], linestyle = ':', color = col[gi])
	plt.xlim([10**(-1.0), 10**1.0])
	plt.ylim([0, 100])
	plt.ylabel("M")
	
	
	plt.subplot(5, 1, 3) # diversity
	for gi in range(6):
		plt.semilogx(measfp[gi][0], np.divide(measfp[gi][3]*measfp[gi][3], measfp[gi][4]), col[gi], label = "gamma = {0}".format(gammaval[gi]))
		plt.semilogx(sigmaval, meas[mi][gi][3], colo[gi])
		plt.axvline(x = points[gi][0][mi], linestyle = '--', color = col[gi])
		plt.axvline(x = points[gi][1][mi], linestyle = ':', color = col[gi])
		plt.axvline(x = points[gi][2][mi], linestyle = ':', color = col[gi])
	plt.xlim([10**(-1.0), 10**1.0])
	plt.xlabel("sigma")
	plt.ylabel("diversity")
	
	
	plt.subplot(5, 1, 4) # dsqaured distance between 2 trajectories
	for gi in range(6):
		plt.semilogx(sigmaval, meas[mi][gi][4], col[gi])
		plt.axvline(x = points[gi][0][mi], linestyle = '--', color = col[gi])
		plt.axvline(x = points[gi][1][mi], linestyle = ':', color = col[gi])
		plt.axvline(x = points[gi][2][mi], linestyle = ':', color = col[gi])
	plt.xlim([10**(-1.0), 10**1.0])
	plt.ylim([0, 100])
	plt.xlabel("sigma")
	plt.ylabel("d squared")
	
	plt.subplot(5, 1, 5) # h variance over last 1% of trajectory
	for gi in range(6):
		plt.semilogx(sigmaval, meas[mi][gi][5], col[gi], label = "gamma = {0}".format(gammaval[gi]))
		plt.axvline(x = points[gi][0][mi], linestyle = '--', color = col[gi])
		plt.axvline(x = points[gi][1][mi], linestyle = ':', color = col[gi])
		plt.axvline(x = points[gi][2][mi], linestyle = ':', color = col[gi])
	plt.xlim([10**(-1.0), 10**1.0])
	if (mi == 10 or mi == 11):
		plt.ylim([0, 0.01])
	plt.xlabel("sigma")
	plt.ylabel("h")
	plt.legend(bbox_to_anchor=(0, -1.2, 1, 0.2), loc='lower left', ncol=3, mode="expand", borderaxespad=0)

	plt.subplots_adjust(bottom = 0.2, top = 0.96)
	
	plt.savefig("five2_{0}.pdf".format(mi))


	
"""

# bunin plots with coloured scatter graph
######################################################

colour = np.loadtxt("allcolour2Tr.txt", delimiter=",")
colour = colour.reshape(12, 6, 21, 3)
colour = np.transpose(colour, (1, 0, 2, 3)) # gam, mu, sig, colour

for gi in range(6):
	
	#gi = 2


	
	plt.figure(13 + gi)
	plt.title("gamma = {0}".format(gammaval[gi]))
	
	#print(len(muval))
	#print(len(sigmaval))
	#print (colour[gi].shape)
	
	#plt.scatter(muval, points[gi][0], marker = 'x', color = 'k')
	#plt.scatter(muval, points[gi][1], marker = 'o', color = 'k')
	#plt.scatter(muval, points[gi][2], marker = '.', color = 'k')
	
	for mi in range(12):
		for si in range(21):
			plt.scatter(muval[mi], sigmaval[si], c=colour[gi][mi][si].reshape(1,-1), marker = 's', edgecolor = 'k')#, size = 5, linewidth = 1)
	
	z = np.loadtxt("bunin2_{0}.txt".format(gi), delimiter = ",")
	z = z.reshape(len(z)//2, 2)
	z = np.transpose(z)
	plt.semilogy(z[0], z[1], 'b')
	
	m = np.loadtxt("mu2_{0}.txt".format(gi), delimiter = ",")
	m = m.reshape(len(m)//2, 2)
	m = np.transpose(m)
	plt.semilogy(m[0], m[1], 'r')
	
	plt.axhline(y = 0, linestyle = '--', color = 'k')
	plt.ylim([10**(-1.2), 10**1.2])
	plt.xlim([-4.5, 2])
	plt.xlabel("mu")
	plt.ylabel("sigma")
	plt.savefig("bunin2_{0}.pdf".format(gi))
"""

plt.show()


