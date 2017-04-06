#include<cstdio>
#include<iostream>
#include <fstream>
#include<vector>
#include<math.h>

using namespace std;

// Multiplies the given matrix with the vector and returns a vector.
vector<double> matrixMultplicationWithVector(vector<vector<double>> matrix, vector<double> vec);

// Checks the dimensions of the matrix and of the vector to see if they can be multiplied.
bool checkMatrixMultiplicationWithVector(vector<vector<double>> matrix, vector<double> vec);

// Concatenate two vectors into a new vector. You first add vec1 and then add vec2.
vector<double> concatenateTwoVectors(vector<double> vec1, vector<double> vec2);

// Apply the tanh function to every element in the given aray and return a vector with the results.
vector<double> applyTanhElementWise(vector<double> vec);

// Perform inner product of the 2 vectors and return the result back.
double vectorInnerProduct(vector<double> vec1, vector<double> vec2);

// Returns a new matrix which is the input matrix transposed.
vector<vector<double>> getTransposeMatrix(vector<vector<double>> matrix);

// Returns a new matrix which is the Hadamard product of the 2 given matrices.
vector<vector<double>> getMatrixHadamardProduct(vector<vector<double>> matrix1, vector<vector<double>> matrix2);

// Returns a new matrix which is the Hadamard product of the 2 given vectors.
vector<double> getVectorHadamardProduct(vector<double> vect1, vector<double> vect2);

// Checks if the given matrices have the same number of lines and columns. This is for the Hadamard Product.
bool checkIfTwoMatricesHaveTheSameDimesions(vector<vector<double>> matrix1, vector<vector<double>> matrix2);

// Compute the softmax of a vector container.
vector<double> softmax(vector<double> vec);

/**
 * Substract, elemnt-wise 2 vector and return vector containing the result of this operation. This assumes
 * that the 2 vectors have the same dimensions.
 */
vector<double> substractTwoVectors(vector<double> firstVector, vector<double> secondVector);

/**
 * Add, elemnt-wise 2 vector and return vector containing the result of this operation. This assumes
 * that the 2 vectors have the same dimensions.
 */
vector<double> addTwoVectors(vector<double> firstVector, vector<double> secondVector);

// Looks at the dimensions of the two matrices and checks if the dimensions are equal.
bool checkMatricesHaveSameDimenesions(vector<vector<double>> firstMatrice, vector<vector<double>> secondMatrice);

/**
 * Add, elemnt-wise 2 matrices and return a matrice containing the result of this operation. This assumes
 * that the 2 matrices have the same dimensions.
 */
vector<vector<double>> addTwoMatrices(vector<vector<double>> firstMatrice, vector<vector<double>> secondMatrice);

/**
 * Subtract, elemnt-wise 2 matrices and return a matrice containing the result of this operation. This assumes
 * that the 2 matrices have the same dimensions.
 */
vector<vector<double>> subtractTwoMatrices(vector<vector<double>> firstMatrice, vector<vector<double>> secondMatrice);

// Obtain the derivative of the tanh function using (tanh x)' = 1 - ((tanh x)^2) (entry-wise).
vector<double> getTanhDerivativeFunction(vector<double> vec);

// Create a vector containing d elements, all equal to 0.
vector<double> getZeros(int d);

// Print the elements in a vector container.
void printElementsOfVector(vector<double> vec);

// Print the elements in a vector container.
void printElementsOfVector(vector<double> vec, ofstream f);

// Print the elements of a given matrix.
void printElementsOfMatrix(vector<vector<double>> matrix);

// Print the elements of a given matrix.
void printElementsOfMatrix(vector<vector<double>> matrix, ofstream f);

// Transpose a row vector. Return a matrix representing the column vector.
vector<vector<double>> transposeRowVector(vector<double> vec);

// Create a weight matrix containing only 0s of dimensions d x 2d.
vector<vector<double>> getZerosWeightMatrix(int d);

// Matrix multiplication
vector<vector<double>> multiplyMatrices(vector<vector<double>> matrix, vector<double> vec);

// Multiply matrix by scalar.
vector<vector<double>> multiplyMatrixByScalar(vector<vector<double>> matrix, double scalar);

// Create an empty Sentiment Weight matrix of dimension 2 x 25.S
vector<vector<double>> getEmptySentimentWeightMatrix();

// Find the maximum number in a vector.
double findMaxNumberInVector(vector<double> vec);

// Find the minimum number in a vector.
double findMinNumberInVector(vector<double> vec);

// Return norm of a matrix.
double getNormOfMatrix(vector<vector<double>> matrix);

double getNormOfVector(vector<double> vec);

// Matrix multiplication.
double getNormOfMatrix(vector<vector<double>> m1);

// Multiply vector by scalar.
vector<double> multiplyVectorByScalar(vector<double> vec, double scalar);
