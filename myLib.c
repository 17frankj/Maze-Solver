/*
 * myLib.c
 *
 * 
 *  Linear Algebra Library: Created on: September 15, 2025
 *      Author: Joshua Frank
 * 
 * 
 * 
 */
#define _USE_MATH_DEFINES
#include "myLib.h"
#include <stdio.h>
#include <math.h>

// Prints 4 x 1 Vector to console
void print_v4(vec4 v) 
{
    printf("[ %7.3f %7.3f %7.3f %7.3f ]\n", v.x, v.y, v.z, v.w);

}

// Prints 4 x 4 Matrix, COLUMN MAJOR ORDER
void print_mat4(mat4 m)
{
    // First row
    printf("[ %.3f ", m.x.x); // [0][0]
    printf(" %.3f ", m.y.x); // [0][1]
    printf(" %.3f ", m.z.x); // [0][2]
    printf(" %.3f ]\n", m.w.x); // [0][3]

    // Second Row
    printf("[ %.3f ", m.x.y); // [1][0]
    printf(" %.3f ", m.y.y); // [1][1]
    printf(" %.3f ", m.z.y); // [1][2]
    printf(" %.3f ]\n", m.w.y); // [1][3]

    // third row
    printf("[ %.3f ", m.x.z); // [2][0]
    printf(" %.3f ", m.y.z); // [2][1]
    printf(" %.3f ", m.z.z); // [2][2]
    printf(" %.3f ]\n", m.w.z); // [2][3]

    // Fourth Row
    printf("[ %.3f ", m.x.w); // [3][0]
    printf(" %.3f ", m.y.w); // [3][1]
    printf(" %.3f ", m.z.w); // [3][2]
    printf(" %.3f ]\n", m.w.w); // [3][3]
}


/* -------------------------------------------------------------------------------------------------------------------------- */
/*      Vector Math      */
/* -------------------------------------------------------------------------------------------------------------------------- */


// Scalar-Vector Multiplication
vec4 scalar_Vec_Multi(float scalar, vec4 vector)
{
    vector.x = scalar*vector.x;
    vector.y = scalar*vector.y;
    vector.z = scalar*vector.z;
    vector.w = scalar*vector.w;
    return vector;
}

// Vector-Vector Addition
vec4 vec_Addition(vec4 vec_1, vec4 vec_2)
{
    vec4 v_return = {0, 0, 0, 0};
    v_return.x = vec_1.x + vec_2.x;
    v_return.y = vec_1.y + vec_2.y;
    v_return.z = vec_1.z + vec_2.z;
    v_return.w = vec_1.w + vec_2.w;
    return v_return;
}

// Vector-Vector Subtraction
vec4 vec_Subtraction(vec4 vec_1, vec4 vec_2)
{
    vec4 v_return = {0, 0, 0, 0};
    v_return.x = vec_1.x - vec_2.x;
    v_return.y = vec_1.y - vec_2.y;
    v_return.z = vec_1.z - vec_2.z;
    v_return.w = vec_1.w - vec_2.w;
    return v_return;
}

// Magnitude of a Vector
float vec_Magnitude(vec4 vector)
{
    float v_return = 0;
    vector.x = vector.x * vector.x;
    vector.y = vector.y * vector.y;
    vector.z = vector.z * vector.z;
    vector.w = vector.w * vector.w;
    v_return = sqrtf(vector.x + vector.y + vector.z + vector.w);
    v_return = fabsf(v_return);
    return v_return;
}

// Normalize
vec4 vec_Normalized(vec4 vector)
{
    float mag_vec = vec_Magnitude(vector);
    vector.x = vector.x * (1/mag_vec);
    vector.y = vector.y * (1/mag_vec);
    vector.z = vector.z * (1/mag_vec);
    vector.w = vector.w * (1/mag_vec);
    return vector;
}

// Dot product
float vec_Dot_product(vec4 vec1, vec4 vec2)
{
    float v_return = 0;
    return v_return = ((vec1.x * vec2.x) + (vec1.y * vec2.y) + (vec1.z * vec2.z) + (vec1.w * vec2.w));
}

// Cross Product
vec4 vec_Cross_Product(vec4 vec1, vec4 vec2)
{
    vec4 v_return = {0, 0, 0, 0};
    v_return.x = ((vec1.y * vec2.z) - (vec1.z * vec2.y));
    v_return.y = ((vec1.z * vec2.x) - (vec1.x * vec2.z));
    v_return.z = ((vec1.x * vec2.y) - (vec1.y * vec2.x));
    return v_return;
}


/* -------------------------------------------------------------------------------------------------------------------------- */
/*       Matrix Math      */
/* -------------------------------------------------------------------------------------------------------------------------- */

mat4 identity()
{
    return (mat4){{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
}

mat4 matrix_translation(float glFloat1, float glFloat2, float glFloat3)
{
    mat4 transform = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
    transform.w.x = glFloat1;
    transform.w.y = glFloat2;
    transform.w.z = glFloat3;
    return transform;
}

mat4 matrix_scaling(float glFloat1, float glFloat2, float glFloat3)
{
    mat4 transform = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
    transform.x.x = glFloat1;
    transform.y.y = glFloat2;
    transform.z.z = glFloat3;
    return transform;
}

mat4 rotate_x(float theta)
{
    mat4 identity = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
    float rad = theta / 180.0 * M_PI;
    identity.y.y = cos(rad);
    identity.y.z = sin(rad);
    identity.z.y = -sin(rad);
    identity.z.z = cos(rad);
    return identity;
}

mat4 rotate_y(float theta)
{
    mat4 identity = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
    float rad = theta / 180.0 * M_PI;
    identity.x.x = cos(rad);
    identity.x.z = -sin(rad);
    identity.z.x = sin(rad);
    identity.z.z = cos(rad);
    return identity;
}

mat4 rotate_z(float theta)
{
    mat4 identity = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
    float rad = theta / 180.0 * M_PI;
    identity.x.x = cos(rad);
    identity.x.y = sin(rad);
    identity.y.x = -sin(rad);
    identity.y.y = cos(rad);
    return identity;
}

// Scalar Multi on Matrix
mat4 matrix_scalar_Multi(float scaler, mat4 matrix)
{
    matrix.x.x = matrix.x.x * scaler;
    matrix.x.y = matrix.x.y * scaler;
    matrix.x.z = matrix.x.z * scaler;
    matrix.x.w = matrix.x.w * scaler;

    matrix.y.x = matrix.y.x * scaler;
    matrix.y.y = matrix.y.y * scaler;
    matrix.y.z = matrix.y.z * scaler;
    matrix.y.w = matrix.y.w * scaler;

    matrix.z.x = matrix.z.x * scaler;
    matrix.z.y = matrix.z.y * scaler;
    matrix.z.z = matrix.z.z * scaler;
    matrix.z.w = matrix.z.w * scaler;

    matrix.w.x = matrix.w.x * scaler;
    matrix.w.y = matrix.w.y * scaler;
    matrix.w.z = matrix.w.z * scaler;
    matrix.w.w = matrix.w.w * scaler;

    return matrix;
}

// Matrix addition
mat4 matrix_addition(mat4 matrix1, mat4 matrix2)
{
    mat4 return_matrix = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};

    // first column
    return_matrix.x.x = matrix1.x.x + matrix2.x.x;
    return_matrix.x.y = matrix1.x.y + matrix2.x.y;
    return_matrix.x.z = matrix1.x.z + matrix2.x.z;
    return_matrix.x.w = matrix1.x.w + matrix2.x.w;

    // second column
    return_matrix.y.x = matrix1.y.x + matrix2.y.x;
    return_matrix.y.y = matrix1.y.y + matrix2.y.y;
    return_matrix.y.z = matrix1.y.z + matrix2.y.z;
    return_matrix.y.w = matrix1.y.w + matrix2.y.w;

    // Third column
    return_matrix.z.x = matrix1.z.x + matrix2.z.x;
    return_matrix.z.y = matrix1.z.y + matrix2.z.y;
    return_matrix.z.z = matrix1.z.z + matrix2.z.z;
    return_matrix.z.w = matrix1.z.w + matrix2.z.w;

    // Fourth column
    return_matrix.w.x = matrix1.w.x + matrix2.w.x;
    return_matrix.w.y = matrix1.w.y + matrix2.w.y;
    return_matrix.w.z = matrix1.w.z + matrix2.w.z;
    return_matrix.w.w = matrix1.w.w + matrix2.w.w;

    return return_matrix;
}

// Matrix Subtraction
mat4 matrix_subtraction(mat4 matrix1, mat4 matrix2)
{
    mat4 return_matrix = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};

    // first column
    return_matrix.x.x = matrix1.x.x - matrix2.x.x;
    return_matrix.x.y = matrix1.x.y - matrix2.x.y;
    return_matrix.x.z = matrix1.x.z - matrix2.x.z;
    return_matrix.x.w = matrix1.x.w - matrix2.x.w;

    // second column
    return_matrix.y.x = matrix1.y.x - matrix2.y.x;
    return_matrix.y.y = matrix1.y.y - matrix2.y.y;
    return_matrix.y.z = matrix1.y.z - matrix2.y.z;
    return_matrix.y.w = matrix1.y.w - matrix2.y.w;

    // Third column
    return_matrix.z.x = matrix1.z.x - matrix2.z.x;
    return_matrix.z.y = matrix1.z.y - matrix2.z.y;
    return_matrix.z.z = matrix1.z.z - matrix2.z.z;
    return_matrix.z.w = matrix1.z.w - matrix2.z.w;

    // Fourth column
    return_matrix.w.x = matrix1.w.x - matrix2.w.x;
    return_matrix.w.y = matrix1.w.y - matrix2.w.y;
    return_matrix.w.z = matrix1.w.z - matrix2.w.z;
    return_matrix.w.w = matrix1.w.w - matrix2.w.w;

    return return_matrix;
}

// Matrix multiplication
mat4 matrix_multi(mat4 matrix1, mat4 matrix2)
{
    mat4 return_matrix = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};

    // first column
    return_matrix.x.x = ((matrix1.x.x * matrix2.x.x) + (matrix1.y.x * matrix2.x.y) + (matrix1.z.x * matrix2.x.z) + (matrix1.w.x * matrix2.x.w));
    return_matrix.x.y = ((matrix1.x.y * matrix2.x.x) + (matrix1.y.y * matrix2.x.y) + (matrix1.z.y * matrix2.x.z) + (matrix1.w.y * matrix2.x.w));
    return_matrix.x.z = ((matrix1.x.z * matrix2.x.x) + (matrix1.y.z * matrix2.x.y) + (matrix1.z.z * matrix2.x.z) + (matrix1.w.z * matrix2.x.w));
    return_matrix.x.w = ((matrix1.x.w * matrix2.x.x) + (matrix1.y.w * matrix2.x.y) + (matrix1.z.w * matrix2.x.z) + (matrix1.w.w * matrix2.x.w));

    // second column
    return_matrix.y.x = ((matrix1.x.x * matrix2.y.x) + (matrix1.y.x * matrix2.y.y) + (matrix1.z.x * matrix2.y.z) + (matrix1.w.x * matrix2.y.w));
    return_matrix.y.y = ((matrix1.x.y * matrix2.y.x) + (matrix1.y.y * matrix2.y.y) + (matrix1.z.y * matrix2.y.z) + (matrix1.w.y * matrix2.y.w));
    return_matrix.y.z = ((matrix1.x.z * matrix2.y.x) + (matrix1.y.z * matrix2.y.y) + (matrix1.z.z * matrix2.y.z) + (matrix1.w.z * matrix2.y.w));
    return_matrix.y.w = ((matrix1.x.w * matrix2.y.x) + (matrix1.y.w * matrix2.y.y) + (matrix1.z.w * matrix2.y.z) + (matrix1.w.w * matrix2.y.w));

    // third Column
    return_matrix.z.x = ((matrix1.x.x * matrix2.z.x) + (matrix1.y.x * matrix2.z.y) + (matrix1.z.x * matrix2.z.z) + (matrix1.w.x * matrix2.z.w));
    return_matrix.z.y = ((matrix1.x.y * matrix2.z.x) + (matrix1.y.y * matrix2.z.y) + (matrix1.z.y * matrix2.z.z) + (matrix1.w.y * matrix2.z.w));
    return_matrix.z.z = ((matrix1.x.z * matrix2.z.x) + (matrix1.y.z * matrix2.z.y) + (matrix1.z.z * matrix2.z.z) + (matrix1.w.z * matrix2.z.w));
    return_matrix.z.w = ((matrix1.x.w * matrix2.z.x) + (matrix1.y.w * matrix2.z.y) + (matrix1.z.w * matrix2.z.z) + (matrix1.w.w * matrix2.z.w));

    // fourth column
    return_matrix.w.x = ((matrix1.x.x * matrix2.w.x) + (matrix1.y.x * matrix2.w.y) + (matrix1.z.x * matrix2.w.z) + (matrix1.w.x * matrix2.w.w));
    return_matrix.w.y = ((matrix1.x.y * matrix2.w.x) + (matrix1.y.y * matrix2.w.y) + (matrix1.z.y * matrix2.w.z) + (matrix1.w.y * matrix2.w.w));
    return_matrix.w.z = ((matrix1.x.z * matrix2.w.x) + (matrix1.y.z * matrix2.w.y) + (matrix1.z.z * matrix2.w.z) + (matrix1.w.z * matrix2.w.w));
    return_matrix.w.w = ((matrix1.x.w * matrix2.w.x) + (matrix1.y.w * matrix2.w.y) + (matrix1.z.w * matrix2.w.z) + (matrix1.w.w * matrix2.w.w));

    return return_matrix;
}

// Inverse of a Matrix
mat4 matrix_inverse(mat4 matrix)
{
    mat4 original = matrix;

    // calc matrix of minor A
    mat4 minor = matrix_minor(matrix);

    // Make minor A into the matrix of cofactor
    mat4 cofactor = matrix_cofactor(minor);

    // Transpose result
    mat4 transpose = matrix_transpose(cofactor);
    
    // Multiply result by 1 / (determinant of A)
    float determinent = matrix_determinant(original, minor);

    // multiply 1/ determinent by transpose
    return sm4_multi((1/determinent), transpose);

}

vec4 matrix_vector_multi(mat4 matrix, vec4 vector)
{
    vec4 return_vector = {0, 0, 0, 0};

    return_vector.x = ((matrix.x.x * vector.x) + (matrix.y.x * vector.y) + (matrix.z.x * vector.z) + (matrix.w.x * vector.w));
    return_vector.y = ((matrix.x.y * vector.x) + (matrix.y.y * vector.y) + (matrix.z.y * vector.z) + (matrix.w.y * vector.w));
    return_vector.z = ((matrix.x.z * vector.x) + (matrix.y.z * vector.y) + (matrix.z.z * vector.z) + (matrix.w.z * vector.w));
    return_vector.w = ((matrix.x.w * vector.x) + (matrix.y.w * vector.y) + (matrix.z.w * vector.z) + (matrix.w.w * vector.w));

    return return_vector;
}

mat4 sm4_multi(float fraction, mat4 transpose)
{
    mat4 return_matrix = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};

    // first column
    return_matrix.x.x = (fraction * transpose.x.x);
    return_matrix.x.y = (fraction * transpose.x.y);
    return_matrix.x.z = (fraction * transpose.x.z);
    return_matrix.x.w = (fraction * transpose.x.w);

    // second column
    return_matrix.y.x = (fraction * transpose.y.x);
    return_matrix.y.y = (fraction * transpose.y.y);
    return_matrix.y.z = (fraction * transpose.y.z);
    return_matrix.y.w = (fraction * transpose.y.w);

    // third Column
    return_matrix.z.x = (fraction * transpose.z.x);
    return_matrix.z.y = (fraction * transpose.z.y);
    return_matrix.z.z = (fraction * transpose.z.z);
    return_matrix.z.w = (fraction * transpose.z.w);

    // fourth column
    return_matrix.w.x = (fraction * transpose.w.x);
    return_matrix.w.y = (fraction * transpose.w.y);
    return_matrix.w.z = (fraction * transpose.w.z);
    return_matrix.w.w = (fraction * transpose.w.w);

    return return_matrix;

}

float matrix_determinant(mat4 original, mat4 minor)
{
    float result = ((original.x.x*minor.x.x) - (original.x.y*minor.x.y) + (original.x.z*minor.x.z) - (original.x.w*minor.x.w));
    return result;
}

mat4 matrix_transpose(mat4 matrix)
{
    mat4 return_matrix = matrix;

    // xx stays

    // xy <=> yx
    return_matrix.x.y = matrix.y.x;
    return_matrix.y.x = matrix.x.y;

    // xz <=> zx
    return_matrix.x.z = matrix.z.x;
    return_matrix.z.x = matrix.x.z;

    // xw <=> wx
    return_matrix.x.w = matrix.w.x;
    return_matrix.w.x = matrix.x.w;

    // yz <=> zy
    return_matrix.y.z = matrix.z.y;
    return_matrix.z.y = matrix.y.z;

    // yw <=> wy
    return_matrix.y.w = matrix.w.y;
    return_matrix.w.y = matrix.y.w;

    // zw <=> wz
    return_matrix.z.w = matrix.w.z;
    return_matrix.w.z = matrix.z.w;


    return return_matrix;
}

mat4 matrix_cofactor(mat4 matrix)
{
    // flip all signs to opposite

    //matrix.x.x = -matrix.x.x;
    matrix.x.y = -matrix.x.y;
    //matrix.x.z = -matrix.x.z;
    matrix.x.w = -matrix.x.w;

    matrix.y.x = -matrix.y.x;
    //matrix.y.y = -matrix.y.y;
    matrix.y.z = -matrix.y.z;
    //matrix.y.w = -matrix.y.w;

    //matrix.z.x = -matrix.z.x;
    matrix.z.y = -matrix.z.y;
    //matrix.z.z = -matrix.z.z;
    matrix.z.w = -matrix.z.w;

    matrix.w.x = -matrix.w.x;
    //matrix.w.y = -matrix.w.y;
    matrix.w.z = -matrix.w.z;
    //matrix.w.w = -matrix.w.w;

    return matrix;

}

// minor A
mat4 matrix_minor(mat4 matrix)
{
    mat4 return_matrix = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};

    // [00][yx][zx][wx]
    // [xy][yy][zy][wy]
    // [xz][yz][zz][wz]
    // [xw][yw][zw][ww]

    // [yy][zy][wy]
    // [yz][zz][wz]
    // [yw][zw][ww]
    return_matrix.x.x = ((matrix.y.y * matrix.z.z * matrix.w.w) + (matrix.z.y * matrix.w.z * matrix.y.w) + (matrix.w.y * matrix.y.z * matrix.z.w) - (matrix.y.w * matrix.z.z * matrix.w.y) - (matrix.z.w * matrix.w.z * matrix.y.y) - (matrix.w.w * matrix.y.z * matrix.z.y));

    // [xx][00][zx][wx]
    // [xy][yy][zy][wy]
    // [xz][yz][zz][wz]
    // [xw][yw][zw][ww]    

    // [xy][zy][wy]
    // [xz][zz][wz]
    // [xw][zw][ww]  
    return_matrix.y.x = ((matrix.x.y * matrix.z.z * matrix.w.w) + (matrix.z.y * matrix.w.z * matrix.x.w) + (matrix.w.y * matrix.x.z * matrix.z.w) - (matrix.x.w * matrix.z.z * matrix.w.y) - (matrix.z.w * matrix.w.z * matrix.x.y) - (matrix.w.w * matrix.x.z * matrix.z.y));

    // [xx][yx][00][wx]
    // [xy][yy][zy][wy]
    // [xz][yz][zz][wz]
    // [xw][yw][zw][ww] 

    // [xy][yy][wy]
    // [xz][yz][wz]
    // [xw][yw][ww]
    return_matrix.z.x = ((matrix.x.y * matrix.y.z * matrix.w.w) + (matrix.y.y * matrix.w.z * matrix.x.w) + (matrix.w.y * matrix.x.z * matrix.y.w) - (matrix.x.w * matrix.y.z * matrix.w.y) - (matrix.y.w * matrix.w.z * matrix.x.y) - (matrix.w.w * matrix.x.z * matrix.y.y));

    // [xx][yx][zx][00]
    // [xy][yy][zy][wy]
    // [xz][yz][zz][wz]
    // [xw][yw][zw][ww] 

    // [xy][yy][zy]
    // [xz][yz][zz]
    // [xw][yw][zw]
    return_matrix.w.x = ((matrix.x.y * matrix.y.z * matrix.z.w) + (matrix.y.y * matrix.z.z * matrix.x.w) + (matrix.z.y * matrix.x.z * matrix.y.w) - (matrix.x.w * matrix.y.z * matrix.z.y) - (matrix.y.w * matrix.z.z * matrix.x.y) - (matrix.z.w * matrix.y.y * matrix.x.z));

    // [xx][yx][zx][wx]
    // [00][yy][zy][wy]
    // [xz][yz][zz][wz]    
    // [xw][yw][zw][ww] 

    // [yx][zx][wx]
    // [yz][zz][wz]
    // [yw][zw][ww]
    return_matrix.x.y = ((matrix.y.x * matrix.z.z * matrix.w.w) + (matrix.z.x * matrix.w.z * matrix.y.w) + (matrix.w.x * matrix.y.z * matrix.z.w) - (matrix.y.w * matrix.z.z * matrix.w.x) - (matrix.z.w * matrix.w.z * matrix.y.x) - (matrix.w.w * matrix.y.z * matrix.z.x));

    // [xx][yx][zx][wx]
    // [xy][00][zy][wy]
    // [xz][yz][zz][wz]
    // [xw][yw][zw][ww]

    // [xx][zx][wx]
    // [xz][zz][wz]
    // [xw][zw][ww]
    return_matrix.y.y = ((matrix.x.x * matrix.z.z * matrix.w.w) + (matrix.z.x * matrix.w.z * matrix.x.w) + (matrix.w.x * matrix.x.z * matrix.z.w) - (matrix.x.w * matrix.z.z * matrix.w.x) - (matrix.z.w * matrix.w.z * matrix.x.x) - (matrix.w.w * matrix.x.z * matrix.z.x));
    
    // [xx][yx][zx][wx]
    // [xy][yy][00][wy]
    // [xz][yz][zz][wz]
    // [xw][yw][zw][ww]

    // [xx][yx][wx]
    // [xz][yz][wz]
    // [xw][yw][ww]
    return_matrix.z.y = ((matrix.x.x * matrix.y.z * matrix.w.w) + (matrix.y.x * matrix.w.z * matrix.x.w) + (matrix.w.x * matrix.x.z * matrix.y.w) - (matrix.x.w * matrix.y.z * matrix.w.x) - (matrix.y.w * matrix.w.z * matrix.x.x) - (matrix.w.w * matrix.x.z * matrix.y.x));

    // [xx][yx][zx][wx]
    // [xy][yy][zy][00]
    // [xz][yz][zz][wz]
    // [xw][yw][zw][ww]
                                     
    // [xx][yx][zx]
    // [xz][yz][zz]                     
    // [xw][yw][zw]
    return_matrix.w.y = ((matrix.x.x * matrix.y.z * matrix.z.w) + (matrix.y.x * matrix.z.z * matrix.x.w) + (matrix.z.x * matrix.x.z * matrix.y.w) - (matrix.x.w * matrix.y.z * matrix.z.x) - (matrix.y.w * matrix.z.z * matrix.x.x) - (matrix.z.w * matrix.x.z * matrix.y.x));

    // [xx][yx][zx][wx]
    // [xy][yy][zy][wy]
    // [00][yz][zz][wz]
    // [xw][yw][zw][ww]

    // [yx][zx][wx]
    // [yy][zy][wy]
    // [yw][zw][ww]
    return_matrix.x.z = ((matrix.y.x * matrix.z.y * matrix.w.w) + (matrix.z.x * matrix.w.y * matrix.y.w) + (matrix.w.x * matrix.y.y * matrix.z.w) - (matrix.y.w * matrix.z.y * matrix.w.x) - (matrix.z.w * matrix.w.y * matrix.y.x) - (matrix.w.w * matrix.y.y * matrix.z.x));

    // [xx][yx][zx][wx]
    // [xy][yy][zy][wy]
    // [xz][00][zz][wz]
    // [xw][yw][zw][ww]

    // [xx][zx][wx]
    // [xy][zy][wy]
    // [xw][zw][ww]
    return_matrix.y.z = ((matrix.x.x * matrix.z.y * matrix.w.w) + (matrix.z.x * matrix.w.y * matrix.x.w) + (matrix.w.x * matrix.x.y * matrix.z.w) - (matrix.x.w * matrix.z.y * matrix.w.x) - (matrix.z.w * matrix.w.y * matrix.x.x) - (matrix.w.w * matrix.x.y * matrix.z.x));

    // [xx][yx][zx][wx]
    // [xy][yy][zy][wy]
    // [xz][yz][00][wz]
    // [xw][yw][zw][ww]

    // [xx][yx][wx]
    // [xy][yy][wy]
    // [xw][yw][ww]
    return_matrix.z.z = ((matrix.x.x * matrix.y.y * matrix.w.w) + (matrix.y.x * matrix.w.y * matrix.x.w) + (matrix.w.x * matrix.x.y * matrix.y.w) - (matrix.x.w * matrix.y.y * matrix.w.x) - (matrix.y.w * matrix.w.y * matrix.x.x) - (matrix.w.w * matrix.x.y * matrix.y.x));
    
    // [xx][yx][zx][wx]
    // [xy][yy][zy][wy]
    // [xz][yz][zz][00]
    // [xw][yw][zw][ww]

    // [xx][yx][zx]
    // [xy][yy][zy]
    // [xw][yw][zw]
    return_matrix.w.z = ((matrix.x.x * matrix.y.y * matrix.z.w) + (matrix.y.x * matrix.z.y * matrix.x.w) + (matrix.z.x * matrix.x.y * matrix.y.w) - (matrix.x.w * matrix.y.y * matrix.z.x) - (matrix.y.w * matrix.z.y * matrix.x.x) - (matrix.z.w * matrix.x.y * matrix.y.x));

    // [xx][yx][zx][wx]
    // [xy][yy][zy][wy]
    // [xz][yz][zz][wz]
    // [00][yw][zw][ww]

    // [yx][zx][wx]
    // [yy][zy][wy]
    // [yz][zz][wz]
    return_matrix.x.w = ((matrix.y.x * matrix.z.y * matrix.w.z) + (matrix.z.x * matrix.w.y * matrix.y.z) + (matrix.w.x * matrix.y.y * matrix.z.z) - (matrix.y.z * matrix.z.y * matrix.w.x) - (matrix.z.z * matrix.w.y * matrix.y.x) - (matrix.w.z * matrix.y.y * matrix.z.x));

    // [xx][yx][zx][wx]
    // [xy][yy][zy][wy]
    // [xz][yz][zz][wz]
    // [xw][00][zw][ww]

    // [xx][zx][wx]
    // [xy][zy][wy]
    // [xz][zz][wz]
    return_matrix.y.w = ((matrix.x.x * matrix.z.y * matrix.w.z) + (matrix.z.x * matrix.w.y * matrix.x.z) + (matrix.w.x * matrix.x.y * matrix.z.z) - (matrix.x.z * matrix.z.y * matrix.w.x) - (matrix.z.z * matrix.w.y * matrix.x.x) - (matrix.w.z * matrix.x.y * matrix.z.x));

    // [xx][yx][zx][wx]
    // [xy][yy][zy][wy]
    // [xz][yz][zz][wz]
    // [xw][yw][00][ww]

    // [xx][yx][wx]
    // [xy][yy][wy]
    // [xz][yz][wz]
    return_matrix.z.w = ((matrix.x.x * matrix.y.y * matrix.w.z) + (matrix.y.x * matrix.w.y * matrix.x.z) + (matrix.w.x * matrix.x.y * matrix.y.z) - (matrix.x.z * matrix.y.y * matrix.w.x) - (matrix.y.z * matrix.w.y * matrix.x.x) - (matrix.w.z * matrix.x.y * matrix.y.x));

    // [xx][yx][zx][wx]
    // [xy][yy][zy][wy]
    // [xz][yz][zz][wz]
    // [xw][yw][zw][00]

    // [xx][yx][zx]
    // [xy][yy][zy]
    // [xz][yz][zz]
    return_matrix.w.w = ((matrix.x.x * matrix.y.y * matrix.z.z) + (matrix.y.x * matrix.z.y * matrix.x.z) + (matrix.z.x * matrix.x.y * matrix.y.z) - (matrix.x.z * matrix.y.y * matrix.z.x) - (matrix.y.z * matrix.z.y * matrix.x.x) - (matrix.z.z * matrix.x.y * matrix.y.x));

    return return_matrix;
}
    