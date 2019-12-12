#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#define TUP 25
#define TDOWN 250
#define TLEFT 25
#define TRIGHT 25

#define ITERATIONS 250


int length = 2;
int height = 4;
float spacing = 0.1;
int size_x = (length/spacing) + 1;
int size_y = (height/spacing);

// TIMER %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double get_walltime() {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return (double) (tp.tv_sec + tp.tv_usec*1e-6);
}

void initTemps(float **T) {
    for(int i=0; i<size_x; i++) {
        T[i][0] = TDOWN;
        T[i][size_y-1] = TUP;
    }
    for(int j=0; j<size_y; j++) {
        T[0][j] = TLEFT;
        T[size_x-1][j] = TRIGHT;
    }
}

__global__ void temp(float *T,float *TNew)
{
        int j = threadIdx.x;
        int i = blockIdx.x;
        TNew[i][j] = 0.25 * (T[i+1][j] + T[i-1][j] + T[i][j+1] + T[i][j-1]);
}


__global__ void temp1(float *T, float *TNew)
{
        int j = threadIdx.x;
        int i = blockIdx.x;
        T[i][j] = TNew[i][j];
}


int main() {
    printf("Enter width of the plate: ");
    scanf("%d", &length);
    printf("Enter the height of the plate: ");
    scanf("%d", &height);
    printf("Enter spacing between nodes: ");
    scanf("%f", &spacing);
    size_x = (length/spacing) + 1;
    size_y = (height/spacing);

    float **T;
    cudaMalloc((void**)&T, size_x*size_y*sizeof(float));

    float **TNew;
    cudaMalloc((void**)&TNew, size_x*size_y*sizeof(float));


    for(int i=0; i<size_x*size_y; i++) {
            T[i][j] = 0;
            TNew[i][j] = 0;
    }
    initTemps(T);
    initTemps(TNew);

    float omega = 1.5, error = 1.0;

    double time1 = get_walltime(), time2;

    for(int k=0; k<ITERATIONS; k++) {

        temp<<size_x,size_y>>(T, TNew);

        temp1<<size_x,size_y>>(T, TNew);
  }
    time2 = get_walltime();
    printf("\nTime: %fs\n", time2-time1);


    //    printf("\nResults:\n");
    //    for(int j=size_y-1; j>=0; j--) {
    //        for(int i=0; i<size_x; i++) {
    //            printf("%f\t",T[i][j]);
    //        }
    //        printf("\n");
    //    }

    FILE *fp1 = fopen("resultCoordinatesAllTemp.txt", "w");
    FILE *fp2 = fopen("resultMatrixAllTemp.txt", "w");

    //    printf("\nTEMPERATURE COORDINATES:\n");
    float xcoord = 0.0;
    float ycoord = 0.0;
    for(int j=0; j<size_y; j++, ycoord += spacing){
        for(int i=0; i<size_x; i++, xcoord += spacing) {
            fprintf(fp1, "%f\t%f\t%f\n", xcoord, ycoord, T[i][j]);
        }
        xcoord = 0.0;
    }
    //    printf("\nTEMPERATURES:\n");
    for(int j=size_y-1; j>=0; j--) {
        for(int i=0; i<size_x; i++) {
            fprintf(fp2, "%f\t", T[i][j]);
        }
        fprintf(fp2, "\n");
    }

//    for(int k=0; k<size_x; k++) {
//        free(T[k]);
//        free(TNew[k]);
//    }
//    free(T);
//    free(TNew);
    fclose(fp1);
    fclose(fp2);
    return 0;
}
                                                                
                                                                                                                                                                                          86,1-8        47%
