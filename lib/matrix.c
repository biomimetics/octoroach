/******************************************************************************
* Name: Matrix.c
* Desc: Matrix module
* Date: 2010-06-14
* Author: stanbaek
* Note:
*   This module does not support a matrix dimension greater than 3x3
*******************************************************************************/
#include "matrix.h"
#include <stddef.h>     // for NULL
#include <stdio.h>      // for sprintf
#include <stdlib.h>     // for malloc
#include <string.h>     // for strlen
#include "ports.h"      // for _LATB
#include "lcd.h"
#include "utils.h"

#define MAX_ROW_SIZE    3
#define MAX_COL_SIZE    3

static unsigned char lcdstr_[80];

// only floating point type is supported at this moment.
Matrix matCreate(unsigned char row_size, unsigned char col_size, MatrixType type) {
    
    float *data;
    unsigned char i;
    Matrix mat;
    
    if (row_size > MAX_ROW_SIZE || col_size > MAX_COL_SIZE) return NULL;

    mat = (Matrix)malloc(sizeof(MatrixStruct));

    mat->type = type;
    mat->row_size = row_size;
    mat->col_size = col_size;

    if (row_size == 0 || col_size == 0) {
        mat->data = NULL;
    } else {
        data = (float*)malloc(sizeof(float) * row_size * col_size);
        mat->data = (float**)malloc(sizeof(float*) * row_size); 

        // assign the address of data to mat->data so that we can have mat->data
        // pointing a N*M matrix.
        for (i = 0; i < row_size; i++) {
            //mat->data[i]= (float*)((int)data + i * col_size * sizeof(float));
            mat->data[i]= (float*)((int)data + i * col_size * sizeof(float));
        }

    }

    //sprintf(lcdstr_, "addr of data: 0x%x, and address of mat data is 0x%x.", data, mat->data[0]);
    //print(lcdstr_);
    //delay_ms(1000);



    return mat;
}

void matDelete(Matrix mat) {
    if (mat->data[0] != NULL) free(mat->data[0]); 
    if (mat->data != NULL) free(mat->data);
    free(mat);
}

void matAssignValues(Matrix mat, float *values) {
    unsigned char row, col;
    for (row = 0; row < mat->row_size; ++row) {
        for (col = 0; col < mat->col_size; ++col) {
            mat->data[row][col] = values[row * mat->row_size + col];
        }
    }
}


void matAdd(Matrix dst, Matrix src1, Matrix src2) {
    unsigned char row, col;
    for(row = 0; row < src1->row_size; ++row) {
        for(col=0; col < src1->col_size; ++col) {
            dst->data[row][col] = src1->data[row][col] + src2->data[row][col];
        }
    }
}

void matSub(Matrix dst, Matrix src1, Matrix src2) {
    unsigned char row, col;
    for(row = 0; row < src1->row_size; ++row) {
        for(col=0; col < src1->col_size; ++col) {
            dst->data[row][col] = src1->data[row][col] - src2->data[row][col];
        }
    }
}

void matScale(Matrix dst, Matrix src, float scale) {
    unsigned char row, col;
    for(row = 0; row < src->row_size; ++row) {
        for(col=0; col < src->col_size; ++col) {
            dst->data[row][col] = src->data[row][col] * scale;
        }
    }
}

void matDotProduct(Matrix dst, Matrix src1, Matrix src2) {
    unsigned char row, col, k;
    float **a = src1->data;
    float **b = src2->data;
    Matrix temp = matCreate(src1->row_size, src2->col_size, src1->type);

    for(row = 0; row < temp->row_size; ++row) {
        for(col = 0; col < temp->col_size; ++col) {
            temp->data[row][col] = 0;
            for(k = 0; k < src1->col_size; ++k) {
                temp->data[row][col] += a[row][k] * b[k][col];
            }
        }
    }
    matCopy(dst, temp);
    free(temp);
}

void matToString(Matrix mat, unsigned char* str) {
    unsigned char row, col;
    str[0] = 0;  // make the length of str zero not to screw up. 

    for(row = 0; row < mat->row_size; ++row) {
        for(col = 0; col < mat->col_size; ++col) {
            sprintf(str + strlen(str), "%f,", mat->data[row][col]);
        }
    }
    str[strlen(str)-1] = 0;   // replace "," with "\0" at the end of str.

}

/******************************************************************************
* For floating point operation, a 3*3 matrix inversion takes about 180 - 200us. 
*|a11 a12 a13|-1         |   a33a22-a32a23  -(a33a12-a32a13)   a23a12-a22a13  |
*|a21 a22 a23| = 1/DET * | -(a33a21-a31a23)   a33a11-a31a13  -(a23a11-a21a13) |
*|a31 a32 a33|           |   a32a21-a31a22  -(a32a11-a31a12)   a22a11-a21a12  |
******************************************************************************/
float matInverse(Matrix dst, Matrix src) {
    float det = matDet(src);

    if (det == 0) return det;

    float det_inv = 1/det;
    float **a = src->data;
    Matrix temp = matCreate(src->row_size, src->col_size, src->type);

    if (src->row_size == 2 && src->col_size == 2) {
        temp->data[0][0] = a[1][1]*det_inv;
        temp->data[0][1] = a[1][0]*det_inv;
        temp->data[1][0] = a[0][1]*det_inv;
        temp->data[1][1] = a[0][0]*det_inv;
    } else if (src->row_size == 3 && src->col_size == 3) {
        temp->data[0][0] = (a[2][2] * a[1][1] - a[2][1] * a[1][2])*det_inv;
        temp->data[0][1] = (a[2][1] * a[0][2] - a[2][2] * a[0][1])*det_inv;
        temp->data[0][2] = (a[1][2] * a[0][1] - a[1][1] * a[0][2])*det_inv;
        temp->data[1][0] = (a[2][0] * a[1][2] - a[2][2] * a[1][0])*det_inv;
        temp->data[1][1] = (a[2][2] * a[0][0] - a[2][0] * a[0][2])*det_inv;
        temp->data[1][2] = (a[1][0] * a[0][2] - a[1][2] * a[0][0])*det_inv;
        temp->data[2][0] = (a[2][1] * a[1][0] - a[2][0] * a[1][1])*det_inv;
        temp->data[2][1] = (a[2][0] * a[0][1] - a[2][1] * a[0][0])*det_inv;
        temp->data[2][2] = (a[1][1] * a[0][0] - a[1][0] * a[0][1])*det_inv;
    } else {
        return 0;
    }
    matCopy(dst, temp);
    free(temp);
    return det;
}

// DET  =  a11(a33a22-a32a23)-a21(a33a12-a32a13)+a31(a23a12-a22a13)
float matDet(Matrix mat) {
    float **a = mat->data;

    if (mat->row_size == 2 && mat->col_size == 2) {
        return a[0][0] * a[1][1] - a[0][1] * a[1][0];
    } else if (mat->row_size == 3 && mat->col_size == 3) {
        return a[0][0] * (a[1][1] * a[2][2] - a[1][2] * a[2][1]) +
               a[1][0] * (a[0][2] * a[2][1] - a[0][1] * a[2][2]) +
               a[2][0] * (a[0][1] * a[1][2] - a[0][2] * a[1][1]);
    } else {
        return 0;
    }
}        

void matCopy(Matrix dst, Matrix src) {
    unsigned char row, col;
    for(row = 0; row < src->row_size; ++row) {
        for(col = 0; col < src->col_size; ++col) {
            dst->data[row][col] = src->data[row][col];
        }
    }
}

void matTranspose(Matrix dst, Matrix src) {
    Matrix temp = matCreate(src->row_size, src->col_size, src->type);
    unsigned char row, col;
    for(row = 0; row < src->row_size; ++row) {
        for(col = 0; col < src->col_size; ++col) {
            temp->data[col][row] = src->data[row][col];
        }
    }
    matCopy(dst, temp);
    free(temp);
}

float matTrace(Matrix mat) {
    float sum = 0;
    unsigned char i;

    for(i = 0; i < mat->row_size; ++i) {    
        sum += mat->data[i][i];
    }

    return sum;
}



