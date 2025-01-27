#include <iostream>
#include <sstream>
#include <string>
#include <random>
#include <fstream>
#include <algorithm>
#include <omp.h>
#include <iomanip>
#include <Eigen/Dense>
//#include <eigen3/Eigen/Dense>
using namespace Eigen;
using namespace std;

IOFormat pp(20, DontAlignCols, ",", ",", "", "", "", "");
IOFormat cc(6, DontAlignCols, ",", ",", "", "", "", "");

string carryon;
bool info = false;
double p;
double gama;
double mu;
double k;

vector<double> muval;
vector<double> gammaval;
vector<double> sigmaval;

// first plan - to be able to find fixed point values of M, q, X for given values of sigma, mu, gamma, for a fixed value of p
// but I think what we have at the moment is a map from z1, mu, gamma to the rest.
// not sure if this is the best way to find it but I can look at that later
// for multiple values of p can we use vector notation?

double phi(double z1)
{
	return (1.0 - erf(z1/sqrt(2.0)))/2.0;
}
// this becomes zero when z1 = infinity, which means sigma = 0

double second(double z1)
{
	return (((z1*z1) + 1.0)*(1.0 - erf(z1/sqrt(2.0)))/2.0) - (z1*exp(-z1*z1/2.0)/sqrt(2.0*M_PI));
	// this is the second order integral, this is never zero, im not sure if this is true!
}

double X(double z1)
{
	double pphi = phi(z1);
	return (gama*(p - 1.0)*pphi*pphi/second(z1)) + pphi;
}
// this is zero when phi = 0, so when sigma = 0, also for some gamma that probably is not in the range?

double help(double z1)
{
	//return phi(z1)/X(z1);
	double sec = second(z1);
	return sec/(sec + (gama*(p - 1.0)*phi(z1))); // may be better because doesnt rely on other parameters
}
// this is undefined for sigma = 0

double first(double z1)
{
	return (z1*(erf(z1/sqrt(2.0)) - 1.0)/2.0) + (exp(-z1*z1/2.0)/sqrt(2.0*M_PI));
}
// this is the first order integral, this can not be zero



double Mgrad(double z1, double MM, double small)
{
	double Mcoeff = help(z1)*z1/first(z1); // may be better because doesnt rely on other parameters
	double grad = (Mcoeff*(MM + small)) + (mu*pow((MM + small), p - 1.0)) + k;
	cout << "grad1 " << grad << endl;
	grad -= (Mcoeff*(MM - small)) + (mu*pow((MM - small), p - 1.0)) + k;
	cout << "grad2 " << grad << endl;
	return grad/(2.0*small);
}

double M(double Mstart, double z1) //this is a solution to a p-order polynomial, so will probably do newton-raphson?
{
	double Mcoeff = help(z1)*z1/first(z1); // may be better because doesnt rely on other parameters
	double MM = Mstart; // maybe we dont need this? I think this should be set to k
	
	if (info == true)
	{
	ofstream mm; mm.open("zz.txt");
	ofstream ab; ab.open("abs.txt");

	cout << "now plotting function for M for z1 = " << z1 << endl;
	
	for (int i = 0; i <= 400000; i++)
	{
		double m = (0.1*(double)i);
		mm << m;
		ab << (Mcoeff*m) + (mu*pow(m, p - 1.0)) + k;
		if (i != 400000) {mm << ", "; ab << ", ";}
	}
	mm.close(); ab.close();
	cout << "finished plotting M" << endl;
	}
	
	if (p == 2.0) return -k/((z1*help(z1)/first(z1)) + mu); // this could give -infinity
	if (p == 3.0)
	{
		if (mu == 0.0) return -k*first(z1)/(z1*help(z1));
		else
		{
			//if (pow(z1*help(z1)/first(z1), 2.0) < 4.0*mu*k) return 1.0/0.0;
			return ((z1*help(z1)/first(z1)) + sqrt(pow(z1*help(z1)/first(z1), 2.0) - (4.0*mu*k)))/(-2.0*mu);
		}
	}
	

	double small = pow(10.0, -6.0); // dont know about this?
	double y = (Mcoeff*MM) + (mu*pow(MM, p - 1.0)) + k;
	double grad;
	//if (info == true) {cout << "MM " << MM << ", y " << y << endl;}
	double ynew;
	double Mnew;
	while (abs(y) > 0.0)
	{
		if (info == true) {cout << "MM " << MM << ", y " << y << endl;}
		grad = Mgrad(z1, MM, small);
		if (grad == 0.0) {cout << "grad = 0" << endl; if (abs(y) < pow(10.0, -5.0)) return MM; else return 1.0/0.0;}
		//if (info == true) {cout << "small " << small << " grad " << grad << endl;}
		Mnew = MM - (y/grad);
		// if (z1 + zz < 0.0) cout << "badd" << endl;
		ynew = (Mcoeff*Mnew) + (mu*pow(Mnew, p - 1.0)) + k;
		if (abs(ynew) >= abs(y) && abs(y) < pow(10.0, -5.0)) break;
		if (abs(ynew) >= abs(y) && y*ynew > 0.0) return 1.0/0.0;
		y = ynew;
		MM = Mnew;
		//if (info == true) {cout << "z1 " << z1 << ", y " << y << endl;}
		if (small > pow(10.0, -9.0)) small /= 10.0;
		cout << "small " << small << endl;
	}
	
	//if (!(abs(sigma(z1, z2)) >= 0.0  && abs(gamma(z1, z2)) >= 0.0)) // found the wrong solution

	return MM;
}


double sigtot(double z1)
{
	return help(z1)*M(k, z1)/first(z1);
}

double q(double z1)
{
	//if (z1 == 1.0/0.0) return pow(M(k, z1), 2.0);
	//if (z1 == 0.0) cout << "z1 = 0 in q function!! :(" << endl;
	//return second(z1)*pow(X(z1)*melp(z1)/(z1*phi(z1)), 2.0);
	return second(z1)*pow(M(k, z1)/first(z1), 2.0);
}

double sigma(double z1)
{
	//if (z1 == 1.0/0.0) return 0;
	//return sqrt(2.0/(p*pow(q(z1), p - 1.0)))*sigtot(z1);
	//return sqrt(2.0/(second(z1)*p*pow(q(z1), p - 2.0)))*help(z1);
	
	
	return help(z1)*sqrt(2.0/(p*pow(second(z1), p - 1.0)))*pow(first(z1)/M(k, z1), p - 2.0); // this one!
	//return help(z1)*sqrt(2.0/(p*pow(second(z1), p - 1.0)));
}

double mud2(double z1) // for p = 2 this is the value of mu where we find divergence
{
	return -z1*help(z1)/first(z1);
}

double mud3(double z1) // for p = 3, value of mu where no solutions for M, not sure if divergence
{
	return pow(z1*help(z1)/first(z1), 2.0)/(4.0*k);
}

double otherM(double z1) // for p = 3 only
{
	return ((z1*help(z1)/first(z1)) - sqrt(pow(z1*help(z1)/first(z1), 2.0) - (4.0*mu*k)))/(-2.0*mu);
}

double othersig(double z1) // other sigma for p = 3
{
	return help(z1)*sqrt(2.0/(p*pow(second(z1), p - 1.0)))*pow(first(z1)/otherM(z1), p - 2.0);
}

///////////////////////////////// end of equation type stuff


double crit_z1() // depends on p only
{	
	double z1 = -10.0;
	double small = pow(10.0, -6.0);
	double y = second(z1) - (phi(z1)*(p - 1.0));
	double grad;
	//if (info == true) {cout << "z1 " << z1 << ", y " << y << endl;}
	double ynew;
	double znew;
	while (abs(y) > 0.0)
	{
		grad = ((second(z1 + small) - (phi(z1 + small)*(p - 1.0))) - (second(z1 - small) - (phi(z1 - small)*(p - 1.0))))/(2.0*small);
		if (grad == 0.0) break;
		//if (info == true) {cout << "small " << small << " grad " << grad << endl;}
		znew = z1 - (y/grad);
		ynew = second(znew) - (phi(znew)*(p - 1.0));
		if (abs(ynew) >= abs(y) && ((ynew*y) > 0.0 || abs(y) < pow(10.0, -5.0))) break;
		y = ynew;
		z1 = znew;
		//if (info == true) {cout << "z1 " << z1 << ", y " << y << endl;}
		if (small > pow(10.0, -10.0)) small /= 10.0;
	}
	return z1;	
}

double maxsig_z1(double zstart) // returns the z1 where sigma reaches a stationary point, depends on mu and gamma
{
	ofstream ab; ab.open("abs.txt");
	int grid = 1000000;
	double small = pow(10.0, -6.0);

	/*cout << "now plotting function for maxsig for mu = " << mu << " gamma = " << gama << endl;
	
	for (int i = 0; i <= grid; i++)
	{
		double z = 100.0*(double)i/(double)grid - 90.0;
		double sigrad = (sigma(z + small) - sigma(z - small))/(2.0*small);
		ab << z << "," << sigma(z) << "," << sigrad;
		if (i != grid) {ab << ",";}
	}
	ab.close();
	cout << "finished plotting z" << endl;*/
	
	double z1 = zstart;
	//double small = pow(10.0, -6.0);
	double y = (sigma(z1 + small) - sigma(z1 - small))/(2.0*small);
	double grad;
	//if (info == true) {cout << "z1 " << z1 << ", y " << y << endl;}
	double ynew;
	double znew;
	while (abs(y) > 0.0)
	{
		grad = (sigma(z1 + small) + sigma(z1 - small) - (2.0*sigma(z1)))/(small*small);
		if (grad == 0.0) break;
		//if (info == true) {cout << "small " << small << " grad " << grad << endl;}
		znew = z1 - (y/grad);
		ynew = (sigma(znew + small) - sigma(znew - small))/(2.0*small);
		if (abs(ynew) >= abs(y) && ((ynew*y) > 0.0 || abs(y) < pow(10.0, -5.0))) break;
		y = ynew;
		z1 = znew;
		//if (info == true) {cout << "z1 " << z1 << ", y " << y << endl;}
		if (small > pow(10.0, -10.0)) small /= 10.0;
	}
	return z1;	
}
	

double div3_z1(double zstart) //I dont know about this!!!
{
	/*ofstream zz; zz.open("zz.txt");
	ofstream ab; ab.open("abs.txt");
	
	int grid = 1000000;
	cout << "now plotting function for zdiv for mu = " << mu << endl;
	
	for (int i = 0; i <= grid; i++)
	{
		double z = 100.0*(double)i/(double)grid - 50.0;
		zz << z;
		ab << pow(z*help(z)/first(z), 2.0)/(4.0*k) - mu << "," << sigma(z);
		if (i != grid) {zz << ", "; ab << ", ";}
	}
	zz.close(); ab.close();
	cout << "finished plotting z" << endl;*/
	double z1 = zstart;
	double small = pow(10.0, -6.0);
	double y = mud3(z1) - mu;
	double grad;
	cout << "z1 " << z1 << ", y " << y << endl;
	double ynew;
	double znew;
	while (abs(y) > 0.0)
	{
		grad = (mud3(z1 + small) - mud3(z1 - small))/(2.0*small);
		if (grad == 0.0) break;
		cout << "small " << small << " grad " << grad << endl;
		znew = z1 - (y/grad);
		cout << "znew = " << znew << ", sigma = " << sigma(znew) << endl;
		if  (!(sigma(znew) >= 0.0)) break;
		ynew = mud3(znew) - mu;
		if (abs(ynew) >= abs(y) && ((ynew*y) > 0.0 || abs(y) < pow(10.0, -5.0))) break;
		y = ynew;
		z1 = znew;
		cout << "z1 " << z1 << ", y " << y << endl;
		if (small > pow(10.0, -10.0)) small /= 10.0;
	}
	if (abs(y) > pow(10.0, -4.0)) return 0.0;
	else return z1;
}


double div2_z1() // for p = 2, depends on mu
{
	if (info == true)
	{
	ofstream zz; zz.open("zz.txt");
	ofstream ab; ab.open("abs.txt");

	cout << "now plotting function for zdiv for mu = " << mu << endl;
	
	for (int i = 0; i <= 400000; i++)
	{
		double z = (10.0*(double)i/400000.0) - 5.0;
		zz << z;
		ab << z*help(z)/first(z) + mu;
		if (i != 400000) {zz << ", "; ab << ", ";}
	}
	zz.close(); ab.close();
	cout << "finished plotting z" << endl;
	}
	
	if (mu == 1.0) return -1.0/0.0;
	
	double z1 = 2.0;
	double small = pow(10.0, -6.0);
	double y = z1*help(z1)/first(z1) + mu;
	double grad;
	double ynew;
	double znew;
	while (abs(y) > 0.0)
	{
		if (info == true) {cout << "z1 " << z1 << ", y " << y << endl;}
		grad = (((z1 + small)*help(z1 + small)/first(z1 + small)) - ((z1 - small)*help(z1 - small)/first(z1 - small)))/(2.0*small);
		//cout << "grad " << grad << endl;
		//cout << "grad1 " << ((z1 + small)*help(z1 + small)/first(z1 + small)) << endl;
		//cout << "z1 + small " << z1 + small << endl;
		//cout << "help(z1 + small) " << help(z1 + small) << endl;
		//cout << "first(z1 + small) " << first(z1 + small) << endl;
		if (grad == 0) {if (abs(y) < pow(10.0, -5.0)) return z1; else return -1.0/0.0;}
		znew = z1 - (y/grad);
		ynew = znew*help(znew)/first(znew) + mu;
		if (info == true) {cout << "znew " << znew << ", ynew " << ynew << endl;}
		//cout << "abs(ynew) " << abs(ynew) << ", abs(y) " << abs(y) << endl;
		if (abs(ynew) >= abs(y) && ((ynew*y) > 0.0 || abs(y) < pow(10.0, -5.0))) break;
		y = ynew;
		z1 = znew;
		if (small > pow(10.0, -10.0)) small /= 10.0;
	}
	
	return z1;
}
	

bool compare(ArrayXd i, ArrayXd j) {return (i(0) < j(0));}
void fiveplot(int grid)
{
	string critstr = "crit_" + to_string((int)p) + "_" + to_string((int)(10*mu)) + ".txt";
	ofstream critfile; critfile.open(critstr);
	// so i think the range of gamma is between -1/(p-1) and 1, but I need to check this!!
	
	double zcrit = crit_z1();
	cout << "crit " << zcrit << endl;
	
	
	
	for (int gi = 0; gi <= 2; gi++)
	{
		//int gi = 1;
		
		string filename = "measfp_" + to_string((int)p) + "_" + to_string((int)(10*mu)) + "_" + to_string(gi) + ".txt";
		ofstream file; file.open(filename);

		gama = ((p - 2.0)*gi*gi/(2.0*(p - 1.0))) + ((4.0 - p)*gi/(2.0*(p - 1.0))) -1.0/(p - 1.0);
		cout << gama << endl;
		critfile << sigma(zcrit); // critical sigma

		vector<ArrayXd> sigmat;
		
		double Mstart = k;
		double sig_prev = -1.0; // next sigma has to be bigger than previous

		for (int i = 0; i <= grid; i++)
		{
			//double z1 = (k - mu)/(((double)i*range/(double)grid) + start); // where did i get this from?
			double z1 = ((double)i*50.0/(double)grid) - 50.0;
			//if (M(Mstart, z1) < 0.0) continue; //{cout << "bad solution 1 " << z1 << endl; continue;}
			//if (q(z1) < 0.0) continue; //{cout << "bad solution 2 " << z1 <<endl; continue;}
			if (sigma(z1) < 0.0) continue; //{cout << "bad solution 3 " << z1 << endl; continue;}
			//if (!(abs(sigma(z1)) >= 0.0)) continue; //{cout << "bad solution 4 " << z1 << endl; continue;}
			if (sigma(z1) > 10.0) continue;
			if (sigma(z1) < sig_prev) break; // this one!
			//if (sig(z1, z2) == 0.0) continue; // this is for histograms only

			ArrayXd entry = ArrayXd::Zero(6); // sigma, z1, phi, M, q, help
			entry(0) = sigma(z1);
			entry(1) = z1;
			entry(2) = phi(z1);
			entry(3) = M(Mstart, z1);
			entry(4) = q(z1);
			entry(5) = help(z1);
			
			if (gama == 0.0) cout << sigma(z1) << ", " << pow(2.0/3.0, 0.5)*z1*-1.0/second(z1) << ", " << M(Mstart, z1) << ", " << -1.0*first(z1)/z1 << endl;
			
			Mstart = entry(3);
			sig_prev = entry(0);
			
			sigmat.push_back(entry);
			//cout << "i " << i << ", z1 " << z1 << ", phi " << phi(z1) << ", M " << entry(3) << ", sig " << sigma(z1) << ", q " << q(z1) << endl;
			//cin >> carryon;
			//info = true;
		}
		sort(sigmat.begin(), sigmat.end(), compare);

		cout << gi << " " << sigmat.size() << " sorted!" << endl;
	
		for (int i = 0; i < sigmat.size(); i++)
		{
			file << sigmat[i].format(cc);
			//cout << sigmat[i].format(c) << endl;
			if (i != sigmat.size()-1) file << ",";
		}
		file.close();
		if (gi != 2) critfile << ",";
	}
	critfile.close();
}

void fiveplot3(int grid, int mi)
{
	mu = muval[mi];
	
	string critstr = "crit3_" + to_string(mi) + ".txt";
	ofstream critfile; critfile.open(critstr);
	
	double zcrit = crit_z1();
	//cout << "crit " << zcrit << endl;
	
	for (int gi = 0; gi < 6; gi++)
	{
		cout << mi << "," << gi << endl;
		
		string filename = "measfp3_" + to_string(mi) + "_" + to_string(gi)+ ".txt";
		ofstream file; file.open(filename);

		gama = gammaval[gi];
		critfile << sigma(zcrit); // critical sigma
	
		double Mstart = k;
		double sig_prev = -1.0; // next sigma has to be bigger than previous

		for (int i = 0; i <= grid; i++)
		{
			double z1 = ((double)i*100.0/(double)grid) - 50.0;
			if (sigma(z1) < 0.0) continue; //{cout << "bad solution 3 " << z1 << endl; continue;}
			//if (!(abs(sigma(z1)) >= 0.0)) continue; //{cout << "bad solution 4 " << z1 << endl; continue;}
			//if (sigma(z1) < sig_prev) break; // this one!
			sig_prev = sigma(z1);
			if (i != 0) file << ",";
			//cout << "i " << i << ", z1 " << z1 << ", phi " << phi(z1) << ", M " << entry(3) << ", sig " << sigma(z1) << ", q " << q(z1) << endl;
			file << sigma(z1) << "," << z1 << "," << phi(z1) << "," << M(0.0, z1) << "," << q(z1) << "," << help(z1);
		}
		file.close();
		if (gi != 5) critfile << ",";
	}
	critfile.close();
}

void phase(int grid) // for gamma against sigma
{
	string filename = "phline_" + to_string((int)p) + "_" + to_string((int)(10*mu)) + ".txt";
	ofstream file; file.open(filename);
	double zcrit = crit_z1();
	for (int ig = 0; ig <= grid; ig++)
	{
		double gi = (double)ig/(double)grid;
		gama = (2.0*(p - 2.0)*gi*gi/(p - 1.0)) + ((4.0 - p)*gi/(p - 1.0)) - 1.0/(p - 1.0);
		file << gama << "," << sigma(zcrit);
		if (ig != grid) file << ",";
	}
	file.close();
}

void bunin2(int grid) // for a phase diagram like bunin
{
	ofstream file; file.open("bunin2_" + to_string((int)(10.0*gama)) + ".txt");
	
	for (int i = 0; i <= grid; i++)
	{
		double z1 = 100.0*(double)i/(double)grid - 50.0;
		if (second(z1)/(phi(z1)*(p - 1.0)) <= -gama) break;
		mu = mud2(z1);
		if (mu < -10.0) break;
		//cout << z1 << ", " << sigma(z1) << ", " << mu << endl;
		if (i != 0) file << ",";
		file << mu << "," << sigma(crit_z1()) << "," << sigma(z1);
		cout << z1 << ", " << mu << "," << sigma(crit_z1()) << "," << sigma(z1) << endl;
	}
	
	
	/*for (int i = 0; i <= grid; i ++)
	{
		mu = (6.0*double(i)/(double)grid) - 5.0;
		file << mu << "," << sigma(crit_z1()) << "," << sigma(div2_z1());
		cout << mu << "," << sigma(crit_z1()) << "," << sigma(div2_z1()) << endl;
		//if (z1/second(z1) > prev) cout << z1 << ", " << prev << endl << endl;
		//prev = z1/second(z1);
		if (i != grid) file << ",";
	}*/
	file.close();
}



void bunin3(int grid, int gi)
{
	gama = gammaval[gi];
	double z1 = crit_z1();
	double zstart = 0.0;
	double zstop;
	
	ofstream fil; fil.open("mu3_" + to_string(gi) + ".txt");
	for (int i = 0; i <= grid; i++)
	{
		mu = (4.0*(double)i/(double)grid) - 3.0;
		if (i != 0) fil << ",";
		zstart = maxsig_z1(zstart);
		fil << mu << "," << sigma(z1) << "," << sigma(zstart); // critical line and maxsig line
		//cout << "mu = " << mu << ", zstart = " << zstart << ", sigma = " << sigma(zstart) << endl;
		if (sigma(zstart) > 0.0) zstop = zstart;
	}
	fil.close();
	
	ofstream file; file.open("bunin3_" + to_string(gi) + ".txt");
	vector<ArrayXd> muz;
	double critz1 = crit_z1();
	for (int i = 0; i <= grid; i++)
	{
		z1 = (50.0 + max(critz1, zstop))*(double)i/(double)grid - 50.0;
		mu = mud3(z1);
		cout << z1 << ", " << mu << "," << sigma(crit_z1()) << "," << sigma(z1) << endl;
		if (!(sigma(z1) >= 0.0)) break; // this one!
		if (i != 0) file << ",";
		file << mu << "," << sigma(z1);
		
	}
	file.close();
	
	ofstream fild; fild.open("div3_" + to_string(gi) + ".txt");
	for (int mi = 0; mi < 12; mi ++)
	{
		mu = muval[mi];
		double zlow = 1.0;
		double zhigh = 1.0;
		for (int i = 0; i <= grid; i++)
		{
			z1 = 100.0*(double)i/(double)grid - 50.0;
			if (sigma(z1) > 0 && sigma(z1) < 1.0/0.0)
			{
				if (zlow == 1.0 || zhigh == 1.0) {zlow = z1; zhigh = z1;}
				else zhigh = z1; //cout << "zhigh = " << zhigh << ", sigma = " << sigma(zhigh) << endl;}
				// the plot of sigma is dashy, so could put a break in here if sigma is not positive?
			}
		}
		//cout << "gamma = " << gama << ", mu = " << mu << ", zlow = " << zlow << ", mud3(zlow) = " << mud3(zlow) << ", zhigh = " << zhigh << ", mud3(zhigh) = " << mud3(zhigh) << endl;
		ofstream zz; zz.open("zz.txt");
		ofstream ab; ab.open("abs.txt");
	
		if (abs(mud3(zlow) - mu) < 0.001) {fild << sigma(zlow); cout << "sigmalow = " << sigma(zlow) << endl;}
		else fild << 1.0/0.0;
		fild << ",";
		if (abs(mud3(zhigh) - mu) < 0.001) {fild << sigma(zhigh); cout << "sigmahigh = " << sigma(zhigh) << endl;}
		else fild << 1.0/0.0;
		if (mi != 11) fild << ",";
		
		//string inn;
		//cin >> inn;
	}
	fild.close();
}
	

/*
// this one is for testing
bool compare(ArrayXd i, ArrayXd j) {return (i(1) < j(1));}
void fiveplot(int grid)
{
	string critstr = "Tcrit_" + to_string((int)p) + "_" + to_string((int)(10*mu)) + ".txt";
	ofstream critfile; critfile.open(critstr);
	// so i think the range of gamma is between -1/(p-1) and 1, but I need to check this!!
	
	double zcrit = crit_z1();
	cout << "crit " << zcrit << endl;
	
	//for (int gi = 0; gi <= 2; gi++)
	//{
		int gi = 1;
		
		string filename = "Tmeasfp_" + to_string((int)p) + "_" + to_string((int)(10*mu)) + "_" + to_string(gi) + ".txt";
		ofstream file; file.open(filename);
		gama = ((p - 2.0)*gi*gi/(2.0*(p - 1.0))) + ((4.0 - p)*gi/(2.0*(p - 1.0))) -1.0/(p - 1.0);
		cout << gama << endl;
		critfile << sigma(zcrit); // critical sigma
		vector<ArrayXd> sigmat;
		double start = -40.0;
		double end = 40.0;
		double range = end - start;
		for (int i = 0; i <= grid; i++)
		{
			double z1 = 20.0*(double)i/(double)grid - 10.0; //(k - mu)/(((double)i*range/(double)grid) + start); // where did i get this from?
			//if (M(k, z1) < 0.0) continue; //{cout << "bad solution 1 " << z1 << endl; continue;}
			//if (q(z1) < 0.0) continue; //{cout << "bad solution 2 " << z1 <<endl; continue;}
			//if (sigma(z1) < 0.0) continue; //{cout << "bad solution 3 " << z1 << endl; continue;}
			//if (!(abs(sigma(z1)) >= 0.0)) continue; //{cout << "bad solution 4 " << z1 << endl; continue;}
			//if (sigma(z1) > 10.0 || sigma(z1) < 0.1) continue;
			//if (sig(z1, z2) == 0.0) continue; // this is for histograms only
			ArrayXd entry = ArrayXd::Zero(6); // sigma, z1, phi, M, q, help
			entry(0) = sigma(z1);
			entry(1) = z1;
			entry(2) = phi(z1);
			entry(3) = M(k, z1);
			entry(4) = q(z1);
			entry(5) = help(z1);
			sigmat.push_back(entry);
			cout << "i " << i << ", z1 " << z1 << ", phi " << phi(z1) << ", M " << M(k, z1) << ", sig " << sigma(z1) << ", q " << q(z1) << endl;
			//cin >> carryon;
			//info = true;
		}
		sort(sigmat.begin(), sigmat.end(), compare);
		cout << gi << " " << sigmat.size() << " sorted!" << endl;
	
		for (int i = 0; i < sigmat.size(); i++)
		{
			file << sigmat[i].format(cc);
			//cout << sigmat[i].format(c) << endl;
			if (i != sigmat.size()-1) file << ",";
		}
		file.close();
		if (gi != 2) critfile << ",";
	//}
	critfile.close();
}
*/

void testing(int grids)
{
	ofstream file; file.open("testing.txt");
	cout << crit_z1() << endl;
	double prev = 1.0;
	for (int i = 0; i <= grids; i ++)
	{
		double z1 = (10.0*double(i)/(double)grids) - 9.0;
		file << z1 << "," << sigma(z1) << "," << pow(z1*help(z1)/first(z1), 2.0)/(4.0*k);
		//cout << z1 << ", " << -z1/first(z1) << endl;
		//if (z1/second(z1) > prev) cout << z1 << ", " << prev << endl << endl;
		//prev = z1/second(z1);
		if (i != grids) file << ",";
	}
	file.close();
}
		

int main()
{
	int grids = 400000; //400000; // for graphs with varying sigma
	p = 3.0;
	//gama = -0.0;
	mu = 0.0;
	k = 1.0;
	
	muval = vector<double>{-0.25, -0.05, 0.0, 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.4, 0.6, 1.0};
	gammaval = vector<double>{-0.5, -0.4, -0.2, 0.0, 0.4, 1.0};
	sigmaval = vector<double>(21);
	for (int i = 0; i <= 20; i++) sigmaval[i] = pow(10.0, (0.1*(double)i) - 1.5);
	
	//for (int mi = 0; mi < 12; mi++) fiveplot3(grids, mi);
	//for (int gi = 0; gi < 6; gi++) bunin3(1000000, gi);
	
	bunin3(500000, 1);
	
	//fiveplot(100000);

	
	//div3_z1(0.0);
	
	
	
	//phase(grids);
	
	//testing(grids);
	
	//info = true;
	
	//cout << crit_z1() << ", " << sigma(crit_z1()) << endl;
	
	//cout << div2_z1() << ", " << sigma(div2_z1()) << endl;
	
	//cout << div2_z1() << endl;
	
	
	//bunin3(1000000);
	
	/*for (int i = 0; i <= 1000000000; i++)
	{
		double z1 = 0.000006*(double)i/100000000.0 - 0.589555;
		//cout << z1 << ", " << mud3(z1) << endl;
		if (abs(mud3(z1) - 0.15) < 0.000000000001) cout << sigma(z1) << endl;
	}*/
	
	
	
	//double z1 = -100.0;
	
	//cout << help(z1) << endl;
	
	//cout << z1 << endl;
	
	//cout << help(z1) << endl;
	
	//cout << M(1.0, z1) << endl;
	
	//cout << (-z1*help(z1)/first(z1))/(2.0*mu) << endl;
	
	//cout << (sqrt(pow(z1*help(z1)/first(z1), 2.0) - (4.0*mu*k)))/(2.0*mu)
	
	//cout << (sqrt(pow(z1*help(z1)/first(z1), 2.0) - (4.0*mu*k)) - (z1*help(z1)/first(z1)))/(2.0*mu) << endl;
	
	//cout << sigma(z1) << endl;
	

	
	info = false;
	
	return 0;
}
/*
int main()
{
	info = true;
	p = 3.0;
	mu = 0.0;
	k = 1.0;
	gama = 0.0;
	double z1 = -0.83;
	cout << phi(z1) << endl;
	cout << second(z1) << endl;
	cout << X(z1) << endl;
	cout << help(z1) << endl;
	cout << first(z1) << endl;
	cout << M(k, z1) << endl;
	cout << q(z1) << endl;
	cout << sigma(z1) << endl;
	
	return 0;
}
*/
