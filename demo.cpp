#include "Header.h"
#include <sys/time.h>
#include <cstdio>
#include <unistd.h>
#include <algorithm>
#include "EvolutionaryStrategies/EvolutionaryAlgorithm.h"

std::mt19937 rng;
bool randInit = false;

void initRandomIfNeeded(){
  if( !randInit ){
    rng.seed(std::random_device()());
    randInit = true;
  }
}

double gaussian(double mean, double stddeviation){
  initRandomIfNeeded();
  std::normal_distribution<double> normal_dist(mean, stddeviation);
  return normal_dist(rng);
}

double gaussian(){
  return gaussian(0.0,1.0);
}

double uniform(double lowerbound, double upperbound){
  initRandomIfNeeded();
  std::uniform_real_distribution<double> dis(lowerbound,upperbound);
  return dis(rng);
}

double uniform(){
  return uniform(0.0,1.0);
}

int uniformInt(int lowerbound, int upperbound){
  initRandomIfNeeded();
  std::uniform_int_distribution<int> dis(lowerbound,upperbound);
  return dis(rng);
}

int uniformInt(){
  return uniformInt(0,1);
}

bool nextBoolean(){
  return uniform() <= 0.5;
}

void selection(FitnessFunction* problem, vector<double*>& population, unsigned dimension){
    //printf("Selection\n");
    double* a = new double[population.size()];
    unsigned n = population.size();
    for(unsigned i=0; i<n; i++)
        a[i] = -1*problem->evaluate( population[i] );

    double extra = abs(*min_element(a,a+n));
    for(unsigned i=0; i<n; i++)
        a[i] += extra;

    for(unsigned i=1; i<n; i++)
        a[i] = a[i-1] + a[i];

    double maxm = a[n-1];
    for(unsigned i=0; i<n; i++) a[i] /= maxm;
    vector<unsigned> buffer;
    for(unsigned k=0; k<n; k++){
        unsigned low = 0, high = population.size()-1;
        unsigned mid;

             double p = gaussian();
             while( low < high && high-low > 1 ){
                 mid = (low+high) / 2;
                 if( p <= a[mid] )
                     high = mid;
                 else
                     low = mid;
             }

             if( a[low] > p )
                 buffer.push_back( low );
             else
                 buffer.push_back( high );
    }

    for(unsigned i=0; i<buffer.size(); i++){
      for(unsigned j=0; j<dimension; j++){
        population[i][j] = population[ buffer[i] ][j];
      }
    }

    buffer.clear();
    delete[] a;
}

void crossover(FitnessFunction* problem, vector<double*>& population, unsigned dimension){
  //printf("Crossover\n");

    for(unsigned k=0; k<population.size(); k+=2){
        unsigned piv = uniformInt( 0, dimension );

        double* o1 = new double[dimension];
        double* o2 = new double[dimension];

        for(unsigned i=0; i<piv; i++){
            o1[i] = population[k][i];
            o2[i] = population[k+1][i];
        }

        for(unsigned i=piv; i<dimension; i++){
            o1[i] = population[k+1][i];
            o2[i] = population[k][i];
        }


        if( problem->satisfy(o1,dimension) )
          for(unsigned i=0; i<dimension; i++)
            population[k][i] = o1[i];

        if( problem->satisfy(o2,dimension) )
          for(unsigned i=0; i<dimension; i++)
            population[k+1][i] = o2[i];

        delete []o1;
        delete []o2;
    }

    //population.clear();
    //population.insert(population.end(),buffer.begin(),buffer.end());


}

void mutation(FitnessFunction* problem, vector<double*>& population, unsigned dimension){
  //printf("Mutation\n");
  for(unsigned i=0; i<population.size(); i++){
        if( nextBoolean() ){
            vector<double> randMut(dimension);
            for(unsigned k=0; k<dimension; k++){
                randMut[k] = 0.0;
                if( nextBoolean() ){
                    randMut[k] = gaussian(0.0, 10.1);
                    population[i][k] += randMut[k];
                  }
            }

            if( !problem->satisfy(population[i],dimension) ){
              for(unsigned k=0; k<dimension; k++)
                population[i][k] -= randMut[k];
            }

            randMut.clear();
        }
    }

}

int main(){
   randInit = false;
  /*  Test the basic benchmark function */
  double* X;
  Benchmarks* fp=NULL;
  unsigned dim = 1000;
  unsigned funToRun[] = {13};
  // unsigned funToRun[] = {1};
  // unsigned funToRun[] = {15};
  unsigned funNum = 1;
  unsigned iterations = 1000;

  vector<double> runTimeVec;
  struct timeval start, end;
  long seconds, useconds;
  double mtime;

  X = new double[dim];
  for (unsigned i=0; i<dim; i++){
    X[i]=0;
  }

  unsigned POP_SIZE = 50;
  EvolutionaryAlgorithm* algorithm = new EvolutionaryAlgorithm(selection,crossover,mutation,POP_SIZE);


  for (unsigned i=0; i<funNum; i++){
    fp = generateFuncObj(funToRun[i]);
    fp->compute(X);
    CECAdapter* adapter = new CECAdapter(fp);

    gettimeofday(&start, NULL);
    printf("F %d value = %1.20E\n", fp->getID(), algorithm->optimize( adapter, iterations, dim ) );
    gettimeofday(&end, NULL);

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = (((seconds) * 1000 + useconds/1000.0) + 0.5)/1000;

    runTimeVec.push_back(mtime);
    printf ( "F %d, Running Time = %f s\n\n", fp->getID(), mtime);

    delete adapter;
  }

  delete []X;
  delete algorithm;

  // for (unsigned i=0; i<runTimeVec.size(); i++){
  // 	printf ( "%f\n", runTimeVec[i] );
  // }

  return 0;
}

// create new object of class with default setting
Benchmarks* generateFuncObj(int funcID){
  Benchmarks *fp;
  // run each of specified function in "configure.ini"
  if (funcID==1){
    fp = new F1();
  }else if (funcID==2){
    fp = new F2();
  }else if (funcID==3){
    fp = new F3();
  }else if (funcID==4){
    fp = new F4();
  }else if (funcID==5){
    fp = new F5();
  }else if (funcID==6){
    fp = new F6();
  }else if (funcID==7){
    fp = new F7();
  }else if (funcID==8){
    fp = new F8();
  }else if (funcID==9){
    fp = new F9();
  }else if (funcID==10){
    fp = new F10();
  }else if (funcID==11){
    fp = new F11();
  }else if (funcID==12){
    fp = new F12();
  }else if (funcID==13){
    fp = new F13();
  }else if (funcID==14){
    fp = new F14();
  }else if (funcID==15){
    fp = new F15();
  }else{
    cerr<<"Fail to locate Specified Function Index"<<endl;
    exit(-1);
  }
  return fp;
}
