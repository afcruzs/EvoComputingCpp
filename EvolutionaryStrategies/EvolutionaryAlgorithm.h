#include <random>
#include <functional>

class FitnessFunction{
  public:
    double evaluate(double* x);
};

class EvolutionaryAlgorithm{

  private:
    std::mt19937 rng;
    bool randInit;
    void (*selectionOperator)(FitnessFunction*,vector<double*>);
    void (*crossoverOperator)(FitnessFunction*,vector<double*>);
    void (*mutationOperator)(FitnessFunction*,vector<double*>);

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
            void (*selection)(FitnessFunction*,vector<double*>),
            void (*crossover)(FitnessFunction*,vector<double*>),
            void (*mutation)(FitnessFunction*,vector<double*>)
          ){
        randInit = false;
        selectionOperator = selection;
        crossoverOperator = crossover;
        mutationOperator = mutation;
      }

      inline double optimize(
                      FitnessFunction* function,
                      unsigned iterations
                      ){

        selectionOperator(function,vector<double*>());
        crossoverOperator(function,vector<double*>());
        mutationOperator(function,vector<double*>());
        
        return gaussian(0,10);
      }
};

class CECAdapter:public FitnessFunction{
  private:
    Benchmarks* function;

  public:
    CECAdapter(Benchmarks* f){
        function = f;
    }

    double evaluate(double* x){
      return function->compute(x);
    }

    ~CECAdapter(){
      delete function;
    }
};
