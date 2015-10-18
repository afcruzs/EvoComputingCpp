#include <random>
#include <functional>

class FitnessFunction{
  public:
    //Vector to evaluate
    virtual double evaluate(double*)=0;

    //Vector to evaluate and its dimension
    virtual bool satisfy(double*,unsigned)=0;

    //Creates a randomPopulation with given size and dimension for each
    //individual.
    virtual void randomPopulation(vector<double*>&,unsigned)=0;

    virtual ~FitnessFunction(){}
};

class EvolutionaryAlgorithm{

  private:
    std::mt19937 rng;
    bool randInit, initPop;
    void (*selectionOperator)(FitnessFunction*,vector<double*>&,unsigned);
    void (*crossoverOperator)(FitnessFunction*,vector<double*>&,unsigned);
    void (*mutationOperator)(FitnessFunction*,vector<double*>&,unsigned);
    vector<double*> population;

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
            void (*selection)(FitnessFunction*,vector<double*>&,unsigned),
            void (*crossover)(FitnessFunction*,vector<double*>&,unsigned),
            void (*mutation)(FitnessFunction*,vector<double*>&,unsigned),
            unsigned popSize
          ){

        randInit = false;
        initPop = false;
        selectionOperator = selection;
        crossoverOperator = crossover;
        mutationOperator = mutation;
        population = vector<double*>(popSize);
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

      void initPopIfNeeded(unsigned dimension){
        if( !initPop ){
          for(unsigned k=0; k<population.size(); k++){
            double* ind = new double[dimension];
            for(unsigned i=0; i<dimension; i++)
              ind[i] = 0.0;
            population[k] = ind;
          }
          initPop = true;
        }
      }

      inline double optimize( FitnessFunction* function, unsigned iterations, unsigned dimension ){
        initPopIfNeeded( dimension );
         function->randomPopulation(population,dimension);

        for(unsigned it = 0; it < iterations; it++){
          //printf("%d\n", it);
          selectionOperator(function,population,dimension);
          crossoverOperator(function,population,dimension);
          mutationOperator(function,population,dimension);

        }

        double ans = best(function,population);


        return ans;
      }

      ~EvolutionaryAlgorithm(){

            double* aux;
            for(unsigned i=0; i<population.size(); i++){
                aux = population[i];
                delete [] aux;
            }

            population.clear();
      }
};

class CECAdapter : public FitnessFunction{
  private:
    std::mt19937 rng;
    bool randInit;

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


    void randomPopulation(vector<double*>& population, unsigned dimension){
      for(unsigned k=0; k<population.size(); k++){
        for(unsigned i=0; i<dimension; i++)
          population[k][i] = uniform(function->getMinX(),function->getMaxX());
      }
    }

    ~CECAdapter(){
      delete function;
    }

  private:
    Benchmarks* function;
};
