
#include <iostream>
#include <sstream>
#include <string>
#include <random>
#include <fstream>
#include <omp.h>
#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>
#include <iomanip>
using namespace Eigen;
using namespace std;
random_device generator;
mt19937 twist(generator());
//IOFormat p(20, DontAlignCols, ",", ",", "", "", "", "");
IOFormat c(6, DontAlignCols, ",", ",", "", "", "", "");
normal_distribution<double> distributionn(0.0, 1.0);
uniform_real_distribution<double> distributionr(0.0, 1.0);


int N;
double Nd; // the double version of N
int T;
double dt;
// these have index starting at 2
ArrayXd mu(4);
ArrayXd gama(4); // should this maybe be a Vector or a Tensor?
ArrayXd sigma(4); // its ok for now but may need to be different in numerical solution
int p;

template<typename T>
using  ArrayType = Eigen::Array<T, Eigen::Dynamic, 1>;

template<typename Scalar, typename sizeType>
auto Tarr(const Eigen::Tensor<Scalar, 1> &tensor,const sizeType rows) // this function maps tensor to array
{
    return Eigen::Map<const ArrayType<Scalar>> (tensor.data(), rows);
}

typedef Eigen::IndexPair<int> IP;
Eigen::array<IP, 1> d10 = { IP(1, 0) }; // second index of T


// this is some things i picked up from the internet
//void randomise(Tensor
// Create a 4 x 3 tensor of floats.
// TensorFixedSize<float, Sizes<4, 3>> t_4x3;
/*a.setZero();
a.setConstant(1000);
a.setValues({{10, 20, 30}});
// can take reduction over various number of dimensions

  typedef Eigen::IndexPair<int> IP;
  Eigen::array<IP, 1> d10 = { IP(1, 0) }; // second index of T
  boost::timer timer;
  Eigen::Tensor<double, 1>  Tyz = T.contract(y, d10).contract(z, d10);
  //cout << Tyz << endl;
use ato as the function type?
always use tensors for multiplication
*/


MatrixXd cholesky(int p)
{
	MatrixXd low = MatrixXd::Zero(p, p);

	for (int i = 0; i < p; i++)
	{
		for (int j = 0; j <= i; j++)
		{
			double sum = 0.0;
			if (j == i)
			{
				for (int k = 0; k < j; k++) sum += pow(low(j, k), 2.0);
				low(j, j) = sqrt(1 - sum);
			}
			else
			{
				for (int k = 0; k < j; k++) sum += (low(i, k)*low(j,k));
				low(i, j) = (gama(p-2) - sum)/low(j, j);
				 
			}
		}
	}

	return low;
}


Tensor<double, 2> order2()
{
	Tensor<double, 2> A(N, N);
	A.setZero();
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < i; j++)
		{
			MatrixXd cho = cholesky(2);
			VectorXd z(2);
			for (int zi = 0; zi < 2; zi++) z(zi) = distributionn(twist);
			MatrixXd list = cho*z;
			A(i,j) = (mu(0)/Nd) + ((sigma(0)*list(0))/sqrt(Nd));
			A(j,i) = (mu(0)/Nd) + ((sigma(0)*list(1))/sqrt(Nd));
		}
	}
	return A;
}

/*
Tensor<double, 3> order3()
{
	Tensor<double, 3> A(N, N, N);
	A.setZero();
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < i; j++)
		{
			for (int k = 0; k < j; k++)
			{
				MatrixXd cho = cholesky(3);
				VectorXd z(3);
				for (int zi = 0; zi < 3; zi++) z(zi) = distributionn(twist);
				MatrixXd list = cho*z;
				A(i,j,k) = (2.0*mu(1)/pow(Nd, 2.0)) + (sqrt(3.0)*sigma(1)*list(0)/Nd);
				A(j,k,i) = (2.0*mu(1)/pow(Nd, 2.0)) + (sqrt(3.0)*sigma(1)*list(1)/Nd);
				A(k,i,j) = (2.0*mu(1)/pow(Nd, 2.0)) + (sqrt(3.0)*sigma(1)*list(2)/Nd);
			}
		}
	}
	return A;
}

Tensor<double, 4> order4()
{
	cout << "aac" << endl;
	Tensor<double, 4> A(N, N, N, N);
	cout << "aa" << endl;
	A.setZero();
	cout << "ab" << endl;
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < i; j++)
		{
			for (int k = 0; k < j; k++)
			{
				for (int l = 0; l < k; l++)
				{
					MatrixXd cho = cholesky(4);
					VectorXd z(4);
					for (int zi = 0; zi < 4; zi++) z(zi) = distributionn(twist);
					MatrixXd list = cho*z;
					A(i,j,k,l) = (6.0*mu(2)/pow(Nd, 3.0)) + (sqrt(12.0/pow(Nd, 3.0))*sigma(2)*list(0));
					A(j,k,l,i) = (6.0*mu(2)/pow(Nd, 3.0)) + (sqrt(12.0/pow(Nd, 3.0))*sigma(2)*list(1));
					A(k,l,i,j) = (6.0*mu(2)/pow(Nd, 3.0)) + (sqrt(12.0/pow(Nd, 3.0))*sigma(2)*list(2));
					A(l,i,j,k) = (6.0*mu(2)/pow(Nd, 3.0)) + (sqrt(12.0/pow(Nd, 3.0))*sigma(2)*list(3));
				}
			}
		}
	}
	return A;
}
Tensor<double, 5> order5()
{
	Tensor<double, 5> A(N, N, N, N, N);
	A.setZero();
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < i; j++)
		{
			for (int k = 0; k < j; k++)
			{
				for (int l = 0; l < k; l++)
				{
					for (int m = 0; m < l; m++)
					{
						MatrixXd cho = cholesky(5);
						VectorXd z(5);
						for (int zi = 0; zi < 5; zi++) z(zi) = distributionn(twist);
						MatrixXd list = cho*z;
						A(i,j,k,l,m) = (24.0*mu(3)/pow(Nd, 4.0)) + (sqrt(60.0)*sigma(3)*list(0)/pow(Nd, 2.0));
						A(j,k,l,m,i) = (24.0*mu(3)/pow(Nd, 4.0)) + (sqrt(60.0)*sigma(3)*list(1)/pow(Nd, 2.0));
						A(k,l,m,i,j) = (24.0*mu(3)/pow(Nd, 4.0)) + (sqrt(60.0)*sigma(3)*list(2)/pow(Nd, 2.0));
						A(l,m,i,j,k) = (24.0*mu(3)/pow(Nd, 4.0)) + (sqrt(60.0)*sigma(3)*list(3)/pow(Nd, 2.0));
						A(m,i,j,k,l) = (24.0*mu(3)/pow(Nd, 4.0)) + (sqrt(60.0)*sigma(3)*list(4)/pow(Nd, 2.0));
					}
				}
			}
		}
	}
	return A;
}
		
*/




class simulation
{
	public:
	Tensor<double, 2> A;
	//Tensor<double, 3> B; //Tensor<double, 4> C; Tensor<double, 5> D;
	Tensor<double, 1> x; Tensor<double, 1> y; Tensor<double, 1> k;
	bool fixed; bool unique; bool diverge;
	vector<ArrayXd> trajx; vector<ArrayXd> trajy; //
	simulation()
	{
		diverge = false;
		x = Tensor<double, 1>(N); y = Tensor<double, 1>(N); k = Tensor<double, 1>(N);
		for (int i = 0; i < N; i++) {x(i) = distributionr(twist); y(i) = distributionr(twist); k(i) = 1.0;}
		//trajx.push_back(Tarr(x, N)); trajy.push_back(Tarr(y, N)); //
		A = order2();
		//B = order3(); //C = order4(); D = order5();
		//B.setZero(); C.setZero(); D.setZero();
	}
	~simulation() {}
	void check() // this is for traj only last 1% and this does each species induvidually
	{
		fixed = true;
		for (int t = 2; t < (0.01*T)+2; t++)
		{
			if (((abs(trajx[t] - trajx[t-1]) < 0.0001) || (abs((trajx[t] - trajx[t-1])/(trajx[t-1] - trajx[t-2])) < 1.0)).minCoeff() == 0) {fixed = false; break;}
			if (((abs(trajy[t] - trajy[t-1]) < 0.0001) || (abs((trajy[t] - trajy[t-1])/(trajy[t-1] - trajy[t-2])) < 1.0)).minCoeff() == 0) {fixed = false; break;}
		}
		unique = true;
		for (int t = 2; t < (0.01*T)+2; t++)
		{
			if (((abs(trajx[t] - trajy[t]) < 0.0001) || (abs((trajx[t] - trajy[t])/(trajx[t-1] - trajy[t-1])) < 1.0)).minCoeff() == 0) {unique = false; break;}
		}
	}
	void run()
	{
		for (int t = 0; t < T; t++)
		{	
			//cout << t << endl;

			// should comment out a lot of this stuff when isolating orders
			
			if (diverge == false)
			{

				// for x ////////

				Tensor<double, 1> k1 = dt*x*(k - x
				+ A.contract(x, d10) 
				//+ B.contract(x, d10).contract(x, d10)
				//+ C.contract(x, d10).contract(x, d10).contract(x, d10)
				//+ D.contract(x, d10).contract(x, d10).contract(x, d10).contract(x, d10)
				);
	
				Tensor<double, 1> k2 = dt*(x + (k1/2.0))*(k - (x + (k1/2.0))
				+ A.contract(x + (k1/2.0), d10)
				//+ B.contract(x + (k1/2.0), d10).contract(x + (k1/2.0), d10)
				//+ C.contract(x + (k1/2.0), d10).contract(x + (k1/2.0), d10).contract(x + (k1/2.0), d10)
				//+ D.contract(x + (k1/2.0), d10).contract(x + (k1/2.0), d10).contract(x + (k1/2.0), d10).contract(x + (k1/2.0), d10)
				);
	
				Tensor<double, 1> k3 = dt*(x + (k2/2.0))*(k - (x + (k2/2.0))
				+ A.contract(x + (k2/2.0), d10)
				//+ B.contract(x + (k2/2.0), d10).contract(x + (k2/2.0), d10)
				//+ C.contract(x + (k2/2.0), d10).contract(x + (k2/2.0), d10).contract(x + (k2/2.0), d10)
				//+ D.contract(x + (k2/2.0), d10).contract(x + (k2/2.0), d10).contract(x + (k2/2.0), d10).contract(x + (k2/2.0), d10)
				);
	
				Tensor<double, 1> k4 = dt*(x + k3)*(k - (x + k3)
				+ A.contract(x + k3, d10)
				//+ B.contract(x + k3, d10).contract(x + k3, d10)
				//+ C.contract(x + k3, d10).contract(x + k3, d10).contract(x + k3, d10)
				// + D.contract(x + k3, d10).contract(x + k3, d10).contract(x + k3, d10).contract(x + k3, d10)
				);
	
				Tensor<double, 1> xtemp = x + (k1 + (2.0*k2) + (2.0*k3) + k4)/6.0;
	
				
				// for y /////////////
	
				k1 = dt*y*(k - y
				+ A.contract(y, d10) 
				//+ B.contract(y, d10).contract(y, d10)
				//+ C.contract(y, d10).contract(y, d10).contract(y, d10)
				//+ D.contract(y, d10).contract(y, d10).contract(y, d10).contract(y, d10)
				);
	
				k2 = dt*(y + (k1/2.0))*(k - (y + (k1/2.0))
				+ A.contract(y + (k1/2.0), d10)
				//+ B.contract(y + (k1/2.0), d10).contract(y + (k1/2.0), d10)
				//+ C.contract(y + (k1/2.0), d10).contract(y + (k1/2.0), d10).contract(y + (k1/2.0), d10)
				//+ D.contract(y + (k1/2.0), d10).contract(y + (k1/2.0), d10).contract(y + (k1/2.0), d10).contract(y + (k1/2.0), d10)
				);
	
				k3 = dt*(y + (k2/2.0))*(k - (y + (k2/2.0))
				+ A.contract(y + (k2/2.0), d10)
				//+ B.contract(y + (k2/2.0), d10).contract(y + (k2/2.0), d10)
				//+ C.contract(y + (k2/2.0), d10).contract(y + (k2/2.0), d10).contract(y + (k2/2.0), d10)
				//+ D.contract(y + (k2/2.0), d10).contract(y + (k2/2.0), d10).contract(y + (k2/2.0), d10).contract(y + (k2/2.0), d10)
				);
	
				k4 = dt*(y + k3)*(k - (y + k3)
				+ A.contract(y + k3, d10)
				//+ B.contract(y + k3, d10).contract(y + k3, d10)
				//+ C.contract(y + k3, d10).contract(y + k3, d10).contract(y + k3, d10)
				//+ D.contract(y + k3, d10).contract(y + k3, d10).contract(y + k3, d10).contract(y + k3, d10)
				);

				Tensor<double, 1> ytemp = y + (k1 + (2.0*k2) + (2.0*k3) + k4)/6.0;

				//////////////////////////////////////////////
				
				
				
				if (Tarr(xtemp, N).maxCoeff() > pow(10.0, 10.0) || Tarr(ytemp, N).maxCoeff() > pow(10.0, 20.0) || Tarr(xtemp, N).minCoeff() < 0.0 || Tarr(ytemp, N).minCoeff() < 0.0) diverge = true;
				else {x = xtemp; y = ytemp;}
				
			}
	
			// store data //////////////////

			//trajx.push_back(Tarr(x, N)); trajy.push_back(Tarr(y, N)); //
			
			//cout << t << ", " << Tarr(x, N).maxCoeff() << ", " << Tarr(x, N).minCoeff() << endl;

			if (t >= (0.99*T)-2) {trajx.push_back(Tarr(x, N)); trajy.push_back(Tarr(y, N));} //

		}
		//if (diverge == false) check();
	}
	ArrayXd measures()
	{
		ArrayXd m = ArrayXd::Zero(6); // phi, M, q, diversity, dsq, h, the last 3 are relative things
		
		ArrayXd sum1 = ArrayXd::Zero(N);
		ArrayXd sum2 = ArrayXd::Zero(N);
		double Msq = 0.0;

		for (int t = 0; t < 0.01*T; t++)
		{
			sum1 += trajx[t];
			sum2 += trajx[t]*trajx[t];

			double M = trajx[t].mean();
			//if (t == 0) cout << trajx[t] << endl;
			double q = (trajx[t]*trajx[t]).mean();
			for (int i = 0; i < N; i++) if (trajx[t](i) > 0.0001) m(0) ++;

			m(1) += M;
			//cout << "m1 " << m(1) << endl;
			m(2) += q;
			m(3) += M*M/q;
			m(4) += (((trajx[t] - trajy[t])*(trajx[t] - trajy[t])).mean());
			Msq += M*M;
		}
		

		sum1 /= (0.01*(double)T);
		sum2 /= (0.01*(double)T);

		m(0) /= 0.01*(double)N*(double)T;
		m(1) /= 0.01*(double)T;
		m(2) /= 0.01*(double)T;
		m(3) /= 0.01*(double)T;
		m(4) /= Msq;

		sum2 -= (sum1*sum1);
		m(5) = sum2.mean()/(sum1*sum1).mean();

		return m;
	}

};


void fiveplot(int grid, int runs) // for a single p
{
	string filename = "homeas_" + to_string(p) + "_" + to_string((int)(10*mu(p-2))) + ".txt"; //
	ofstream file; file.open(filename);
	for (int g = 0; g <= 2; g++)
	{
		gama(p-2) = ((p - 2.0)*g*g/(2.0*(p - 1.0))) + ((4.0 - p)*g/(2.0*(p - 1.0))) -1.0/(p - 1.0);
		for (int s = 0; s <= grid; s++)
		{
			//int s = 7;
		
			double po = (2.0*s/(double)grid) - 1.0;
			sigma(p-2) = pow(10.0, po);
			ArrayXd meas = ArrayXd::Zero(6);
		
			for (int r = 0; r < runs; r++)
			{
				cout << "g " << g << ", s " << s << ", r " << r << endl;
				simulation sim;
				sim.run();
				meas += sim.measures();
			}
			meas /= (double)runs;
			file << meas.format(c);
			if (s != grid) file << ",";
		}
		if (g != 2) file << ",";
	}
}

int main(int argc, char** argv) // for condor
{
	gama = ArrayXd::Zero(4);
	mu = ArrayXd::Zero(4);
	sigma = ArrayXd::Zero(4);
	p = 2;
	N = 200; //200;
	Nd = (double)N;
	T = 200000;
	dt = 0.001;
	//int grid = 20; // for fiveplots
	int runs = 20;
	mu(p-2) = -2.0;
	
	
	int v1 = atoi(argv[1]);
	int v2 = atoi(argv[2]);


	string filename = "five2m0_" + to_string(v1) + "_" + to_string(v2) + ".txt"; // letter represents order
	ofstream file; file.open(filename);

	//file << v1 << ", " << v2 << endl;

	gama(p-2) = ((p - 2.0)*(double)v1*(double)v1/(2.0*(p - 1.0))) + ((4.0 - p)*(double)v1/(2.0*(p - 1.0))) -1.0/(p - 1.0);
	double po = ((double)v2/10.0) - 1.0;
	sigma(p-2) = pow(10.0, po);
	
	ArrayXd meas = ArrayXd::Zero(6);
		
	for (int r = 0; r < runs; r++)
	{
		//cout << "g " << g << ", s " << s << ", r " << r << endl;
		simulation sim;
		sim.run();
		meas += sim.measures();
	}
	meas /= (double)runs;
	file << meas.format(c);

	file.close();	
	
	return 0;
}
				


/*
void histogram(int runs)
{
	string filename = "hohist_" + to_string((int)(10*mu)) + ".txt"; //
	ofstream file; file.open(filename);
	for (int gi = 0; gi <= 2; gi++)
	{
		double gamma = (double)gi - 1.0;
		for (int si = 0; si <= 2; si++)
		{
			double sigma = pow(10.0, ((double)si*0.5) - 1.0);
		
			for (int r = 0; r < runs; r++)
			{
				cout << gi << ", " << si << ", " << r << endl;
				simulation sim(sigma, gamma);
				sim.run();
				file << sim.x.format(c);
				if (r != runs-1) file << ",";
			}
			if (si != 2) file << ",";
		}
		if (gi != 2) file << ",";
	}
	file.close();
}


void plot(double sigma, double gamma, int plots)
{

	ofstream datax;
	datax.open ("trajectoriesx.txt");
	ofstream datay;
	datay.open ("trajectoriesy.txt");
	
	simulation sim(sigma, gamma);
	sim.run();
	cout << sim.fixed << endl;
	cout << sim.unique << endl;
		
	for (int i = 0; i < plots; i++)
	{
		for (int t = 0; t < T; t++)
		{
			datax << sim.trajx[t](i);
			datay << sim.trajy[t](i);
			if (i != plots-1 || t != T-1) {datax << ","; datay << ",";}
		}
	}
}

*/

			


/*
int main()	
{
	gama = ArrayXd::Zero(4);
	mu = ArrayXd::Zero(4);
	sigma = ArrayXd::Zero(4);
	p = 2;
	N = 200; //200;
	Nd = (double)N;
	T = 200000;
	dt = 0.001;
	int grid = 10; // for fiveplots
	int runs = 5;
	int plots = 20;
	mu(p-2) = -2.0;
	
	// everything for p != 2 commented out
	

	// traj matrix 1%


	
	//histogram(runs);

	//fiveplot(grid, runs);

	//plot(pow(10.0, 1.0), 0.0, 20);
	
	
	
	int s = 7;
		
	double po = (2.0*s/(double)grid) - 1.0;
	sigma(p-2) = pow(10.0, po);
	ArrayXd meas = ArrayXd::Zero(6);
	
	//for (int r = 0; r < runs; r++)
	//{
		//cout << "r " << r << endl;
		simulation sim;
		sim.run();
		//cout << sim.measures() << endl;
		if (sim.diverge == false) {cout << "fix " << sim.fixed << endl; cout << "un " << sim.unique << endl;}
		//meas += sim.measures();
	//}
	//meas /= (double)runs;
	//cout << meas.format(c) << endl;



	return 0;
}
*/



