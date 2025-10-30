/*
 * myLib.h
 *
 * 
 *  Linear Algebra Library: Created on: September 15, 2025
 *      Author: Joshua Frank
 * 
 * 
 * 
 */
#ifndef _MYLIB_H_

#define _MYLIB_H_

typedef struct {
    float x;
    float y;
    float z;
    float w;
} vec4;

typedef struct {
    vec4 x;
    vec4 y;
    vec4 z;
    vec4 w;
} mat4;

// Insert function signatures after this line

void print_v4(vec4);
void print_mat4(mat4);

// Vectors
vec4 scalar_Vec_Multi(float scalar, vec4 vector);
vec4 vec_Addition(vec4 vec_1, vec4 vec_2);
vec4 vec_Subtraction(vec4 vec_1, vec4 vec_2);
float vec_Magnitude(vec4 vector);
vec4 vec_Normalized(vec4 vector);
float vec_Dot_product(vec4 vec1, vec4 vec2);
vec4 vec_Cross_Product(vec4 vec1, vec4 vec2);

// Matrixs
mat4 matrix_scalar_Multi(float scaler, mat4 matrix);
mat4 matrix_addition(mat4 matrix1, mat4 matrix2);
mat4 matrix_subtraction(mat4 matrix1, mat4 matrix2);
mat4 matrix_multi(mat4 matrix1, mat4 matrix2);
mat4 matrix_inverse(mat4 matrix);
mat4 matrix_minor(mat4 matrix);
mat4 matrix_cofactor(mat4 matrix);
mat4 matrix_transpose(mat4 matrix);
float matrix_determinant(mat4 normal, mat4 minor);
mat4 sm4_multi(float fraction, mat4 transpose);
vec4 matrix_vector_multi(mat4 matrix, vec4 vector);
mat4 matrix_translation(float glFloat1, float glFloat2, float glFloat3);
mat4 matrix_scaling(float glFloat1, float glFloat2, float glFloat3);
mat4 rotate_x(float theta);
mat4 rotate_y(float theta);
mat4 rotate_z(float theta);
mat4 identity();

// Do not put anything after this line

#endif
