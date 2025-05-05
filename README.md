# ESC190-Seam-Carving-assignment-2
Using dynamic programming to apply seam-carving to images without losing important data in the image. Credits go to ESC180, instructor Michael Guerzhoy, and functions not described here are pre-written by Michael Guerzhoy

seamcarving.c implements all the functions in seamcarving.h. c_img.h and c_img.c are written by Michael Guerzhoy and provided in the assignment.

# Assignment description
Seam-carving is a content-aware image resizing technique where the image is reduced in size by one pixel of width (or height) at a time. A vertical seam in an image is a path of pixels connected from the top to the bottom with one pixel in each row; a horizontal seam is a path of pixels connected from the left to the right with one pixel in each column. Below left is the original 505-by-287 pixel image; below right is the result after removing 150 vertical seams, resulting in a 30% narrower image. Unlike standard content-agnostic resizing techniques (such as cropping and scaling), seam carving preserves the most interest features (aspect ratio, set of objects present, etc.) of the image. Although the underlying algorithm is simple and elegant, it was not discovered until 2007. Now, it is now a core feature in Adobe Photoshop and other computer graphics applications.

In this assignment, you will create a data type that resizes a H-by-W image using the seam-carving technique.You will first compute the dual-gradient energy function, and then find vertical “seams” – paths from the top to the bottom of the image – such that the sum of the dual-gradient energy values in the pixels along the path is as small as possible

Notation. In image processing, pixel (y, x) refers to the pixel in column x and row y, with pixel (0,0) at the upper-left corner and pixel (H-1, W-1) at the lower-right corner.

The first step is to calculate the energy of a pixel, which is a measure of its perceptual importance – the higher the energy, the less likely that the pixel will be included as part of a seam (as you will see in the next step). In this project, you will use the dual-gradient energy function, which is described below. Here is the dual-gradient energy function of the surfing image above:

The energy is high (white) for pixels in the image where there is a rapid color gradient (such as the boundary between the sea and sky and the boundary between the surfing Josh Hug (the original author of this assignment) on the left and the ocean behind him). The seam carving technique avoids removing such high-energy pixels.

The next step is to find a vertical seam of minimum total energy. The seam is a path through pixels from top to bottom such that the sum of the energies of the pixels is minimal. You will identify the minimum-energy seam using dynamic programming.

Functions:
- void calc_energy(struct rgb_img *im, struct rgb_img **grad); : The function will compute the dual-gradient energy function, and place it in the struct rgb_img *grad. For pixels at the edge of the image, you should “wrap around” the image. For example, in the image below for the the pixel (0,1). You are storing the dual-gradient energy in an image. To do that, divide the original energy by 10, and cast it to (uint8_t). For each pixel, set the r, g, and b channels to the same value (the energy divided by 10 and cast to uint8_t).

- dynamic_seam(struct rgb_img *grad, double **best_arr) which allocates and computes the dunamic array *best_arr. (*best_arr)[i*width+j] contains the minimum cost of a seam from the top of grad to the point (𝑖,𝑗).

- void recover_path(double *best, int height, int width, int **path); : This function allocates a path through the minimum seam as defined by the array best.

- void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path); : The function creates the destination image, and writes to it the source image, with the seam removed.
