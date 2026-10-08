#include <stdio.h>
#include <stdlib.h>

/**
 * Return an array of arrays of size *returnSize.
 * The sizes of the arrays are returned as *returnColumnSizes array.
 * Note: Both returned array and *columnSizes array must be malloced, assume caller calls free().
 */
int** generateMatrix(int n, int* returnSize, int** returnColumnSizes) {
    *returnSize = n;
    
    // Allocate memory for returnColumnSizes
    *returnColumnSizes = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        *(*returnColumnSizes + i) = n;
    }
    
    // Allocate memory for the 2D matrix
    int** matrix = (int**)malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
        *(matrix + i) = (int*)malloc(n * sizeof(int));
    }
    
    int top = 0, bottom = n - 1;
    int left = 0, right = n - 1;
    int num = 1;
    
    while (top <= bottom && left <= right) {
        // Traverse Left to Right
        for (int j = left; j <= right; j++) {
            *(*(matrix + top) + j) = num++;
        }
        top++;
        
        // Traverse Top to Bottom
        for (int i = top; i <= bottom; i++) {
            *(*(matrix + i) + right) = num++;
        }
        right--;
        
        // Traverse Right to Left
        if (top <= bottom) {
            for (int j = right; j >= left; j--) {
                *(*(matrix + bottom) + j) = num++;
            }
            bottom--;
        }
        
        // Traverse Bottom to Top
        if (left <= right) {
            for (int i = bottom; i >= top; i--) {
                *(*(matrix + i) + left) = num++;
            }
            left++;
        }
    }
    
    return matrix;
}

int main() {
    int n = 3;
    int returnSize;
    int* returnColumnSizes;
    
    int** result = generateMatrix(n, &returnSize, &returnColumnSizes);
    
    // Print matrix using pointer arithmetic
    printf("Matrix (%dx%d) using pointer arithmetic:\n", n, n);
    for (int i = 0; i < returnSize; i++) {
        for (int j = 0; j < *(*(&returnColumnSizes) + i); j++) {
            printf("%d\t", *(*(result + i) + j));
        }
        printf("\n");
    }
    
    // Free allocated memory
    for (int i = 0; i < n; i++) {
        free(*(result + i));
    }
    free(result);
    free(returnColumnSizes);
    
    return 0;
}