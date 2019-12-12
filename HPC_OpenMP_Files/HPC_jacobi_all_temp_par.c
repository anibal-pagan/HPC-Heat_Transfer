#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>

#define TUP 25
#define TDOWN 250
#define TLEFT 25
#define TRIGHT 25

#define ITERATIONS 1000
#define ERROR 0.000001

int length = 2;
int height = 4;
float spacing = 0.1;
int size_x = 1;
int size_y = 1;

// TIMER %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double get_walltime() {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return (double) (tp.tv_sec + tp.tv_usec*1e-6);
}

void initTemps(float **T) {
    int i,j;
    for(i=0; i<size_x; i++) {
        T[i][0] = TDOWN;
        T[i][size_y-1] = TUP;
    }
    for(j=0; j<size_y; j++) {
        T[0][j] = TLEFT;
        T[size_x-1][j] = TRIGHT;
    }
}

int main() {
    int i, j, k;
    printf("Enter width of the plate: ");
    scanf("%d", &length);
    printf("Enter the height of the plate: ");
    scanf("%d", &height);
    printf("Enter spacing between nodes: ");
    scanf("%f", &spacing);
    size_x = (length/spacing) + 1;
    size_y = (height/spacing);
    float **T=(float**) malloc(size_x *sizeof(float*));
    float **TNew=(float**) malloc(size_x *sizeof(float*));
    for(i=0; i<size_x *sizeof(float*); i++){
        T[i]=(float*) malloc(size_y *sizeof(float));
        TNew[i]=(float*) malloc(size_y *sizeof(float));
    }
    for(i=0; i<size_x; i++) {
        for(j=0; j<size_y; j++) {
            T[i][j] = 0;
            TNew[i][j] = 0;
        }
    }
    initTemps(T);
    initTemps(TNew);
    
    float omega = 1.5;
    
    double time1 = get_walltime(), time2;
    
    for(k=0; k<ITERATIONS; k++) {
        #pragma omp parallel for shared(size_x, size_y, T, TNew, error) private(i, j)
        for(i=1; i<size_x-1; i++) {
            for(j=1; j<size_y-1; j++) {
                //center
                TNew[i][j] = 0.25 * (T[i+1][j] + T[i-1][j] + T[i][j+1] + T[i][j-1]);
                
  //              error = fmax(error, fabs(T[i][j] - TNew[i][j]));
            }
        }
        //       printf("error: %f\n", error);
        #pragma omp parallel for shared(size_x, size_y, T, TNew, error) private(i, j)
        for(i=0; i<size_x; i++) {
            for(j=1; j<size_y-1; j++) {
                T[i][j] = TNew[i][j];
            }
        }
    }
    time2 = get_walltime();
    printf("\nTime: %fs\n", time2-time1);
    
    FILE *fp1 = fopen("resultCoordinatesAllTemp.txt", "w");
    FILE *fp2 = fopen("resultMatrixAllTemp.txt", "w");
    
    //    printf("\nTEMPERATURE COORDINATES:\n");
    float xcoord = 0.0;
    float ycoord = 0.0;
    for(j=0; j<size_y; j++, ycoord += spacing){
        for(i=0; i<size_x; i++, xcoord += spacing) {
            fprintf(fp1, "%f\t%f\t%f\n", xcoord, ycoord, T[i][j]);
        }
        xcoord = 0.0;
    }
    //    printf("\nTEMPERATURES:\n");
    for(j=size_y-1; j>=0; j--) {
        for(i=0; i<size_x; i++) {
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

