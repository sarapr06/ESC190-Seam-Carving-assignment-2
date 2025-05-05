#include "seamcarving.h"
#include "c_img.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
float delta(int x, int y, struct rgb_img *im, int deltay) {
    int x_right = (x + 1) % im->width;
    int x_left = (x - 1 + im->width) % im->width;
    int y_down = (y + 1) % im->height;
    int y_up = (y - 1 + im->height) % im->height;

    int r1, g1, b1, r2, g2, b2;
    if (deltay == 0) {
        r1 = get_pixel(im, y, x_right, 0);
        g1 = get_pixel(im, y, x_right, 1);
        b1 = get_pixel(im, y, x_right, 2);

        r2 = get_pixel(im, y, x_left, 0);
        g2 = get_pixel(im, y, x_left, 1);
        b2 = get_pixel(im, y, x_left, 2);
        int rx = r2-r1;
        int gx = g2-g1;
        int bx = b2-b1;
        return rx * rx + gx * gx + bx * bx;
    } else {
        r1 = get_pixel(im, y_down, x, 0);
        g1 = get_pixel(im, y_down, x, 1);
        b1 = get_pixel(im, y_down, x, 2);
        r2 = get_pixel(im, y_up, x, 0);
        g2 = get_pixel(im, y_up, x, 1);
        b2 = get_pixel(im, y_up, x, 2);
        int ry = r2-r1;
        int gy = g2-g1;
        int by = b2-b1;
        return ry*ry+ by*by+ gy*gy;
    }
}

void calc_energy(struct rgb_img *im, struct rgb_img **grad){
    //compute the dual-gradient energy function, and place it in the struct rgb_img *grad.
    
    create_img(grad, im->height, im->width);
    float deltax;
    float deltay;
    float gradient;
    //energy per pixel (y,x) is Rx(y,x)^2 + Gx(y,x)^2Bx(y,x)^2
    for(int y = 0; y < im->height; y++){
        for(int x = 0; x < im->width; x++){
            deltax=delta(x, y, im, 0);
            deltay=delta(x, y, im, 1);
            gradient=sqrt(deltax+deltay);
            gradient = (uint8_t)(sqrt(deltax + deltay) / 10.0);
            set_pixel(*grad, y, x, gradient, gradient, gradient);
        }
    }

    //storing the dual-gradient energy in an image. 
    //^^divide the original energy by 10, and cast it to (uint8_t). 
    //For each pixel, set the r, g, and b channels to the same value (the energy divided by 10 and cast to uint8_t).
}

void dynamic_seam(struct rgb_img *grad, double **best_arr){
    *best_arr=(double*)malloc(sizeof(double)*(grad->width*grad->height));
    //first row initialized to gradient valuues:
    for (int j = 0; j < grad->width; j++) {
        (*best_arr)[j] = get_pixel(grad, 0, j, 0);
    }
    for (int i = 1; i < grad->height; i++) {
        for (int j = 0; j < grad->width; j++) {
            double min_energy = (*best_arr)[(i - 1) * grad->width + j];//
            if (j > 0) {
                double left_energy = (*best_arr)[(i - 1) * grad->width + (j - 1)];
                if (left_energy < min_energy) {
                    min_energy = left_energy;
                }
            }
            if (j < grad->width - 1) {
                double right_energy = (*best_arr)[(i - 1) * grad->width + (j + 1)];
                if (right_energy < min_energy) {
                    min_energy = right_energy;
                }
            }
            (*best_arr)[i * grad->width + j] = get_pixel(grad, i, j, 0) + min_energy;
        }
    }
    //which allocates and computes the dunamic array *best_arr
    //(*best_arr)[i*width+j] contains the minimum cost of a seam from the top of grad to the point (𝑖,𝑗)

}

int find_min_ind(double *best, int row,int length){
    double min = INFINITY;
    int min_ind = 0;
    for(int i=0;i<length; i++){
        if(best[row+i]<min){
            min=best[row+i];
            min_ind=i;
        }
    }
    return min_ind;
}
void recover_path(double *best, int height, int width, int **path){
    // allocates a path through the minimum seam as defined by the array best.
    *path=(int*)malloc(sizeof(int)*height);
    int min_start = find_min_ind(best, height*(width-1)-1, width);//last row, look for min
    (*path)[height-1]=min_start;
    //so while your index is not zero
    int cur_col=min_start;
    for(int i=height-2;i>=0;i--){
        int best_col = cur_col; //for now
        double best_val = best[i*width+cur_col]; // set as top as default
        //check left:
        if(cur_col>0 && best[i*width+(cur_col-1)]<best_val){//if prev_col not zero (can check left) and less than our default
            best_val=best[i*width+(cur_col-1)];
            best_col=cur_col-1;
        }
        //check right
        if(cur_col<width-1 &&best[i*width+(cur_col+1)]<best_val){
            best_val=best[i*width+(cur_col+1)];
            best_col=cur_col+1;
        }
        //iterate through and check to see if the top, top right or top left of the row above has a min
        (*path)[i]=best_col;
        cur_col=best_col;
    }
    //check through the lowest sum in the LAST row and select it
    //then backtrack until row 1 by checking the columns above, to the left, and right

}


void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path){ // debug
    create_img(dest, src->height, src->width-1); // space for no seam
    /*printf("Seam path: ");
    for (int i = 0; i < src->height; i++) {
        printf("%d ", path[i]);
    }
    printf("\n");
    */
    for (int y=0;y<src->height;y++){
        int k=0; //column index in dest
        for (int x=0;x<src->width;x++){
            if (x!=path[y]){
                uint8_t r = get_pixel(src, y, x, 0);
                uint8_t g = get_pixel(src, y, x, 1);
                uint8_t b = get_pixel(src, y, x, 2);

                set_pixel(*dest, y, k, r, g, b);
                k++;
            }
        }
    }
    //creates the destination image, and writes to it the source image, with the seam removed.
}


void run_functions(){//test for repeatedly removing seams 
    struct rgb_img *im;
    struct rgb_img *cur_im;
    struct rgb_img *grad;
    double *best;
    int *path;

    read_in_img(&im, "HJoceanSmall.bin");
    
    for(int i = 0; i < 5; i++){
        printf("i = %d\n", i);
        calc_energy(im,  &grad);
        dynamic_seam(grad, &best);
        recover_path(best, grad->height, grad->width, &path);
        remove_seam(im, &cur_im, path);

        char filename[200];
        sprintf(filename, "img%d.bin", i);
        write_img(cur_im, filename);


        destroy_image(im);
        destroy_image(grad);
        free(best);
        free(path);
        im = cur_im;
    }
    destroy_image(im);
}

void print_d(double *best_arr, struct rgb_img *grad){
    for(int i =0;i<grad->height;i++){
        for(int j=0;j<grad->width;j++){
            printf("%f ", best_arr[i*grad->width +j]);
        }
        printf("\n");
    }
}
void print_i(int *best_arr, struct rgb_img *grad){
    for(int i =0;i<grad->height;i++){
        printf("%d ", best_arr[i]);
    }
    printf("\n");
}

int main(){
    //to print: 
    struct rgb_img *grad;
    struct rgb_img *im;
    read_in_img(&im, "puppy1.bin");
    calc_energy(im,  &grad);
    print_grad(grad);
    double *best_arr;
    dynamic_seam(grad, &best_arr);
    printf("\n");
    print_d(best_arr, grad);
    int *path;
    recover_path(best_arr, im->height, im->width, &path);
    print_i(path, grad);

    //testing the remove seam:
    struct rgb_img *dest;
    remove_seam(im, &dest, path);
    write_img(dest, "puppy1_after.bin");
    destroy_image(dest);
}

