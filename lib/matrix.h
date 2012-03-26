/************************************************************
* Name: Matrix.h
* Desc: Module for matrices with maximum dimension of 3 by 3
* Date: 2010-06-14
* Author: stanbaekk
**************************************************************/
/*
 * Copyright (c) 2008-2010, Regents of the University of California
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of the University of California, Berkeley nor the names
 *   of its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * <DESCRIPTION>
 *
 * by Stanley S. Baek
 *
 * v.0.1 alpha
 *
 * Revisions:
 *  Stanley S. Baek     2010-7-20   Initial release
 *
 * Notes:
 *  This module is based on the Matrix2D module developed by P. Lacroix 
 *  (http://dev.tplanet.net/Matrix2D). It has been modified for matrices with
 *  demensions of 3*3 or less to compute matrix operations more efficiently. 
 *
 * Usage:
 *  <SAMPLE USAGE>
 */

#ifndef __MATRIX_H
#define __MATRIX_H

typedef enum {MAT_TYPE_FIXED, MAT_TYPE_FLOAT} MatrixType;

typedef struct {
    MatrixType type;
    unsigned char row_size;
    unsigned char col_size;
    // char isSub;
    // void **data;
    float **data;  
} MatrixStruct;

typedef MatrixStruct* Matrix;

Matrix matCreate(unsigned char row_size, unsigned char col_size, MatrixType type);
void matDelete(Matrix mat);
void matAssignValues(Matrix mat, float *values);
void matAdd(Matrix dst, Matrix src1, Matrix src2);
void matSub(Matrix dst, Matrix src1, Matrix src2);
void matScale(Matrix dst, Matrix src, float scale); 
void matDotProduct(Matrix dst, Matrix src1, Matrix src2);
void matToString(Matrix mat, unsigned char* str);
float matInverse(Matrix src, Matrix dst);
float matDet(Matrix mat);
void matClone(Matrix dst, Matrix src);
void matTranspose(Matrix dst, Matrix src);
float matTrace(Matrix mat); 

#endif //__MATRIX_H
