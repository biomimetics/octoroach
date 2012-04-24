/*********************************************
* Name: kf.c
* Desc: Kalman Filters (EKF)
* Date: 2010-06-30
* Author: stanbaek
* 
*   
*********************************************/

#include "kf.h"
#include "matrix.h"
#include "trig.h"

#define TIME_STEP     0.01        // 10ms

// 2x2 matrix
static Matrix P;
// 2x2 matrix
static Matrix Q;    
// 3x3 matrix
static Matrix R;    
// 3x3 matrix
static Matrix A;    
// 3x2 matrix
static Matrix C;    
// 3x1 matrix
static Matrix H;   
// 2x3 matrix
static Matrix L;    
// 2x2 identity matrix
static Matrix I;    
// 2x3 matrix
static Matrix dP;    

static Matrix temp2x2, temp3x2, temp2x3, temp3x3;

static float states[3] = {0,0,0};

/*****************************************************************************
* Function Name : kfSetup
* Description   : 
* Parameters    : None
* Return Value  : None
*****************************************************************************/
void kfSetup(void) {
    P = matCreate(2, 2, MAT_TYPE_FLOAT);
    Q = matCreate(2, 2, MAT_TYPE_FLOAT);
    R = matCreate(3, 3, MAT_TYPE_FLOAT);
    A = matCreate(2, 2, MAT_TYPE_FLOAT);
    C = matCreate(3, 2, MAT_TYPE_FLOAT);
    H = matCreate(3, 1, MAT_TYPE_FLOAT);
    L = matCreate(2, 3, MAT_TYPE_FLOAT);
    I = matCreate(2, 2, MAT_TYPE_FLOAT);    
    dP = matCreate(2, 2, MAT_TYPE_FLOAT);    
    temp2x2 = matCreate(2, 2, MAT_TYPE_FLOAT);
    temp3x2 = matCreate(3, 2, MAT_TYPE_FLOAT);
    temp2x3 = matCreate(2, 3, MAT_TYPE_FLOAT);
    temp3x3 = matCreate(3, 3, MAT_TYPE_FLOAT);


    float p_values[] = {0.01, 0, 0, 0.01};  // 2x2 identity matrix
    float q_values[] = {4.9e-5, 0, 0, 2.1e-5};  // 2x2 matrix
    float r_values[] = {8.1e-4, 0, 0,   0, 7.9e-4, 0,   0, 0, 1.6e-3};
    float i_values[] = {1, 0, 0, 1};

    matAssignValues(P, p_values);
    matAssignValues(Q, q_values);
    matAssignValues(R, r_values);
    matAssignValues(I, i_values);

}

/*****************************************************************************
* Function Name : kfUpdate
* Description   : 
* Parameters    : xl is an array of size 3 holding acceleration data.
*                 omega is an array of size 3 holding angular speed data.
* Return Value  : None
*****************************************************************************/
#define ax      (xl[0])
#define ay      (xl[1])
#define az      (xl[2])
#define wx      (omega[0])
#define wy      (omega[1])
#define wz      (omega[2])

void kfUpdate(float *xl, float *omega) {
    static float phi = 0;
    static float theta = 0;
    static float psi = 0;
    
    float dphi, dtheta, dP;

    float sin_theta;
    float cos_phi = cos(phi);
    float sin_phi = sin(phi);
    float cos_theta = cos(theta);
    float tan_theta = tan(theta);

    float a_values[] = { -wy*cos_phi*tan_theta + wz*sin_phi*tan_theta, 
                         (-wy*sin_phi + wz*cos_phi) / (cos_theta*cos_theta),
                         wy*sin_phi - wz*cos_phi, 
                         0 };

    float c_values[6];
    float h_values[3];

    matAssignValues(A, a_values);

    dphi = wx - wy*sin_phi*tan_theta - wz*cos_phi*tan_theta;
    dtheta = -wy*cos_phi - wz*sin_phi;
    phi = phi + dphi * TIME_STEP;
    theta = theta + dtheta * TIME_STEP;

    // computing dP = APA' + Q
    matTranspose(temp2x2, A);               // A'
    matDotProduct(temp2x2, P, temp2x2);     // PA'
    matDotProduct(temp2x2, A, temp2x2);     // APA'
    matAdd(dP, temp2x2, Q);                 // APA' + Q

    // computing P = P + dt*dP   
    matScale(temp2x2, dP, TIME_STEP);      // dt*dP
    matAdd(P, P, temp2x2);                 // P + dt*dP

    cos_phi = cos(phi);
    sin_phi = sin(phi);
    cos_theta = cos(theta);
    sin_theta = sin(theta);


    c_values = {0,                  cos_theta,
                -cos_theta*cos_phi, sin_theta*sin_phi,
                cos_theta*sin_phi,  sin_theta*cos_phi }

    matAssignValues(C, c_values);


    // L = PC'(R + CPC')^-1
    matTranspose(temp2x3, C);               // C'
    matDotProduct(temp2x3, P, temp2x3);     // PC'
    matDotProduct(temp3x3, C, temp2x3);     // CPC'
    matAdd(temp3x3, R, temp3x3);            // R + CPC'
    matInverse(temp3x3, temp3x3);           // (R + CPC')^-1
    matDotProduct(L, temp2x3, temp3x3);     // PC'(R + CPC')^-1


    // P = (I - LC)P
    matDotProduct(temp2x2, L, C);       // LC
    matSub(temp2x2, I, temp2x2);        // I - LC
    matDotProduct(P, temp2x2, P);       // (I - LC)P


    h_values = {sin_theta, -cos_theta*sin_phi, -cos_theta*cos_phi};
    matAssignValues(H, h_values);

    /*

    ph = ph + dot(L[0], self.ab - h)
    th = th + dot(L[1], self.ab - h) 

    */

    // change the values so that they stay between -PI and +PI
    phi = ((phi + PI) % (2*PI)) - PI;
    theta = ((theta + PI) % (2*PI)) - PI;

    psidot = wy * sin(ph) / cos(th) +  * cos(ph) / cos(th);
    psi += psidot * TIME_STEP;



}
#undef ax      xl[0]
#undef ay      xl[1]
#undef az      xl[2]
#undef wx      omega[0]
#undef wy      omega[1]
#undef wz      omega[2] 


