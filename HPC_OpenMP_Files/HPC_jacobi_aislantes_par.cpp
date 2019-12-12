#include <string.h>
#include <stdlib.h>
#include <omp.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#define TUP 25
#define TDOWN 250
#define ITERATIONS 1000
#define ERROR 0.000001

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
    float **T=(float**) malloc(size_x *sizeof(float*));
    float **TNew=(float**) malloc(size_x *sizeof(float*));
    for(int i=0; i<size_x *sizeof(float*); i++){
        T[i]=(float*) malloc(size_y *sizeof(float));
        TNew[i]=(float*) malloc(size_y *sizeof(float));
    }
    for(int i=0; i<size_x; i++) {
        for(int j=0; j<size_y; j++) {
            T[i][j] = 0;
            TNew[i][j] = 0;
        }
    }
    initTemps(T);
    initTemps(TNew);
    
    float omega = 1.5;
    
    double time1 = get_walltime(), time2;
    
    for(int k=0; k<ITERATIONS; k++) {
        #pragma omp parallel for shared(size_x, size_y, T, TNew) private(i, j)
        for(int i=0; i<size_x; i++) {
            for(int j=1; j<size_y-1; j++) {
                //center
                if(i != 0 && i != size_x-1) {
                    TNew[i][j] = 0.25 * (T[i+1][j] + T[i-1][j] + T[i][j+1] + T[i][j-1]);
                }
                //left wall
                else if(i == 0) {
                    TNew[i][j] = 0.25 * (T[i][j+1] + T[i][j-1] + 2*T[i+1][j]);
                }
                //right wall
                else if(i == size_x-1) {
                    TNew[i][j] = 0.25 * (T[i][j+1] + T[i][j-1] + 2*T[i-1][j]);
                }
            }
        }
        //       printf("error: %f\n", error);
        #pragma omp parallel for shared(size_x, size_y, T, TNew) private(i, j)
        for(int i=0; i<size_x; i++) {
            for(int j=1; j<size_y-1; j++) {
                T[i][j] = TNew[i][j];
            }
        }
    }
    time2 = get_walltime();
    printf("\nTime: %fs\n\n", time2-time1);
    
    FILE *fp1 = fopen("resultCoordinates.txt", "w");
    FILE *fp2 = fopen("resultMatrix.txt", "w");
    
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
