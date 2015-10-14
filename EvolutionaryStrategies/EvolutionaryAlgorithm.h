#include <random>
#include <functional>

class FitnessFunction{
  public:
    //Vector to evaluate
    virtual double evaluate(double*)=0;

    //Vector to evaluate and its dimension
    virtual bool satisfy(double*,unsigned)=0;

    virtual ~FitnessFunction(){}
};

class EvolutionaryAlgorithm{

  private:
    std::mt19937 rng;
    bool randInit;
    void (*selectionOperator)(FitnessFunction*,vector<double*>&);
    void (*crossoverOperator)(FitnessFunction*,vector<double*>&,unsigned);
    void (*mutationOperator)(FitnessFunction*,vector<double*>&,unsigned);

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

  public:

      EvolutionaryAlgorithm(
            void (*selection)(FitnessFunction*,vector<double*>&),
            void (*crossover)(FitnessFunction*,vector<double*>&,unsigned),
            void (*mutation)(FitnessFunction*,vector<double*>&,unsigned)
          ){
        randInit = false;
        selectionOperator = selection;
        crossoverOperator = crossover;
        mutationOperator = mutation;
      }


      inline double best(FitnessFunction* function, vector<double*>& population){

          
          double minm = function->evaluate( population[0] );

          for(unsigned i=1; i<population.size(); i++){
            double aux = function->evaluate( population[i] );
            if( minm < aux )
              aux = minm;
          }
          return minm;
      }

      inline double optimize( FitnessFunction* function, unsigned iterations, unsigned dimension ){

        vector<double*> population;

        for(unsigned it = 0; it < iterations; it++){
          printf("%d\n", it);
          selectionOperator(function,population);
          crossoverOperator(function,population,dimension);
          mutationOperator(function,population,dimension);

        }

        double ans = best(function,population);

        return ans;
      }
};

class CECAdapter : public FitnessFunction{


  public:
    CECAdapter(Benchmarks* f){
        function = f;
    }

    double evaluate(double* x){
      return function->compute(x);
    }

    bool satisfy(double* x, unsigned dimension){

      for(unsigned i=0; i<dimension; i++){
        if( !(x[i] >= function->getMinX() && x[i] <= function->getMaxX()) )
          return false;
      }
      return true;
    }

    ~CECAdapter(){
      delete function;
    }

  private:
    Benchmarks* function;
};
