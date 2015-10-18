#include <omp.h>
#include <stdio.h>

int main(){
    #pragma omp parallel num_threads(10)
    {
      int ID =  omp_get_thread_num();
      printf("hello %d\n",ID);
    }

    return 0;
}
