using namespace std;

class FitnessFunction{
  public:
    double evaluate(double* x);
};

class EvolutionaryAlgorithm{
  public:
      inline double optimize( FitnessFunction* function, unsigned iterations ){
        return 44444.0;
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
