
#include "ThreadPool.hh"
#include "Timer.hh"

#include <cstdlib>
#include <iostream>


using namespace std;


void scale(int i, double* a, double* b) {
        a[i]=4*b[i];
}

int main (int argc, char** argv) {

        ThreadPool pool(8);

        int N = 1e8;
        if(argc > 1) {
          N = std::atof(argv[1]);
        }

        auto a = (double*)calloc(N,sizeof(double));
        auto b = (double*)calloc(N,sizeof(double));

        if(a==0 || b==0) {
          std::cerr << "Could not allocate "
                    << 2*N*sizeof(double)/(1024*1024) << " MB.\n"
                    << "Buy more RAM or decrease N" << std::endl;
          return 1;
        }

        for (int i=0; i<N; i++) { b[i] = i; }


        // cold start for timing purposes
        pool.ParallelFor(0,N,scale,a,b);

        int ntrials = 10;
        double tperformance = 0.0;
        cout << "Callable: c-function pointer\n";
        for (int i=0; i<ntrials; i++)
        {
                Timer timer([&](int elapsed){
                                cout << "Trial " << i << ": "<< elapsed*1e-6 << " ms\n";
                                tperformance+=elapsed;
                        });
                pool.ParallelFor(0,N,scale,a,b);
        }
        cout << "Average: " << tperformance*1e-6 / ntrials << " ms\n\n";

        tperformance = 0.0;
        cout << "Callable: lambda function (without capture) \n";
        for (int i=0; i<ntrials; i++)
        {
                Timer timer([&](int elapsed){
                                cout << "Trial " << i << ": "<< elapsed*1e-6 << " ms\n";
                                tperformance+=elapsed;
                        });
                pool.ParallelFor(0,N,[](int k, double* a, double* b) {return a[k] = 4*b[k];},a,b);
        }
        cout << "Average: " << tperformance*1e-6 / ntrials << " ms\n\n";

        return 0;
}
