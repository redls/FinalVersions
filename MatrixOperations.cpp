#include<cstdio>
#include<iostream>
#include<iomanip>
#include <math.h>
#include "MatrixOperations.h"

using namespace std;

#define EPSILON 0.000000000001
#define WORD_SIZE_VEC 30

// Find the maximum number in a vector.
double findMaxNumberInVector(vector<double> vec) {
    if (vec.size() == 0) {
        cout<<"The given vector has no elements which can be max."<<endl;
        return 0;
    }

    double m = vec[0];
    for(int i = 1; i < vec.size();i++) {
        if (EPSILON < vec[i] - m) m = vec[i];
    }
    return m;
}

// Find the minimum number in a vector.
double findMinNumberInVector(vector<double> vec) {
    if (vec.size() == 0) {
        cout<<"The given vector has no elements which can be min."<<endl;
        return 0;
    }

    double m = vec[0];
    for(int i = 1; i < vec.size();i++) {
        if (EPSILON > vec[i] - m) m = vec[i];
    }
    return m;
}


// Checks the dimensions of the matrix and of the vector to see if they can be multiplied.
bool checkMatrixMultiplicationWithVector(vector<vector<double>> matrix, vector<double> vec) {
    int n = matrix.size(); // number of lines
    if (n == 0) return false;
    int m = matrix[0].size(); // number of columns
    int d = vec.size(); // number of elements in the array

    if (m == d) return true;
    return false;
}

// Multiplies the given matrix with the vector and returns a vector.
vector<double> matrixMultplicationWithVector(vector<vector<double>> matrix, vector<double> vec) {
    vector<double> result;
    if (!checkMatrixMultiplicationWithVector(matrix, vec))  {
        cout<<"The matrix multiplication with the vector could not be computed. The 2 given inputs cannot be multiplied."<<endl;
        exit(0);
    }
    int n = matrix.size(); // number of lines
    int m = matrix[0].size(); // number of columns
    int d = vec.size(); // number of elements in the array

    double s = 0; // current sum
    for (int i = 0; i < n; i++) {
        s = 0;
        for (int j = 0;j < m; j++) {
            s = s + matrix[i][j] * vec[j];
        }
        result.push_back(s);
    }
    return result;
}

// Concatenate two vectors into a new vector. You first add vec1 and then add vec2.
vector<double> concatenateTwoVectors(vector<double> vec1, vector<double> vec2) {
    vector<double> result;
    for (int i = 0; i < vec1.size(); i++) {
        result.push_back(vec1[i]);
    }
    for (int i = 0; i < vec2.size(); i++) {
        result.push_back(vec2[i]);
    }
    return result;
}

// Apply the tanh function to every element in the given aray and return a vector with the results.
vector<double> applyTanhElementWise(vector<double> vec) {
    vector<double> result;
     for (int i = 0; i < vec.size(); i++) {
     if (isnan(tanh(vec[i]))) {
        printElementsOfVector(vec);
        cout<<" Found nan"<<endl;
        exit(0);
     }
     //cout<<tanh(vec[i])<<endl;
        result.push_back(tanh((double)vec[i]));
    }
    return result;
}

// Perform inner product of the 2 vectors and return the result back.
double vectorInnerProduct(vector<double> vec1, vector<double> vec2) {
    double result;
    if (vec1.size() != vec2.size()) {
        cout<<"The given vectors have different dimensions and could not perform inner product on them."<<endl;
        exit(0);
    }
    for (int i = 0; i < vec1.size(); i++) {
        result = result + vec1[i]*vec2[i];
    }
    return result;
}

// Returns a new matrix which is the input matrix transposed.
vector<vector<double>> getTransposeMatrix(vector<vector<double>> matrix) {
    vector<vector<double>> result;
    int numberOfLines = matrix.size();
    int numberOfColumns = 0;
    if (numberOfLines != 0) numberOfColumns = matrix[0].size();
    // Transpose the first line, in order to obtain the lines of the transpose matrix in order
    // to create the entry vectors for each line.
    for(int i = 0; i < numberOfColumns; i++) {
        vector<double> numberToInsert;
        numberToInsert.push_back(matrix[0][i]);
        result.push_back(numberToInsert);
    }

    // Transpose the rest of the matrix.
    for(int i = 1; i < numberOfLines; i++) {
        for(int j = 0; j < numberOfColumns; j++) {
            result[j].push_back(matrix[i][j]);
        }
    }
    return result;
}

// Checks if the given matrices have the same number of lines and columns. This is for the Hadamard Product.
bool checkIfTwoMatricesHaveTheSameDimesions(vector<vector<double>> matrix1, vector<vector<double>> matrix2) {
    int l1 = matrix1.size();
    int l2 = matrix2.size();
    if (l1 != l2) return false;
    if (l1 > 0) {
        for(int i = 0; i < l1; i++) {
            int c1 = matrix1[i].size();
            int c2 = matrix2[i].size();
            if (c1 != c2) return false;
        }
    }
    return true;
}

 // Returns a new matrix which is the Hadamard product of the 2 given matrices.
vector<vector<double>> getMatrixHadamardProduct(vector<vector<double>> matrix1, vector<vector<double>> matrix2) {
    vector<vector<double>> result;
    if (!checkIfTwoMatricesHaveTheSameDimesions(matrix1, matrix2)) {
        cout<<"The 2 matrices don't have the same dimesions. Cannot obtain their Hadamard product."<<endl;
        exit(0);
    }
    int numberOfColumns = matrix1[0].size();
    int numberOfLines = matrix1.size();

    // Multiply the first line, in order to obtain the lines of the transpose matrix in order
    // to create the entry vectors for each line.
    for(int i = 0; i < numberOfLines; i++) {
        vector<double> numberToInsert;
        for(int j = 0; j < numberOfColumns; j++) {
            numberToInsert.push_back(matrix1[i][j] * matrix2[i][j]);
        }
        result.push_back(numberToInsert);
    }
    return result;
}


// Returns a new matrix which is the Hadamard product of the 2 given vectors.
vector<double> getVectorHadamardProduct(vector<double> vect1, vector<double> vect2) {
    vector<double> result;
    double numberToInsert;
    for(int i = 0; i < vect1.size(); i++) {
        numberToInsert = vect1[i] *vect2[i];
        result.push_back(numberToInsert);
    }
    return result;
}


long long findPower(double x) {
    long long prod = 1;
    long long counter = 0;
    if (x < 0) x = x *(-1);
    while(x < 1) {
        x *= 10;
        prod *= 10;
        counter++;
    }
    return counter;
}

// Compute the softmax of a vector container.
vector<double> softmax(vector<double> vec) {
    double sum = 0;
    double temp;
    double maximum = findMaxNumberInVector(vec);

    long long prod = 1;
    long long aux = 0;
//cout<<vec[0]<<" "<<vec[1]<<endl;
    for (int i = 0; i < vec.size(); i++) {
        vec[i] = vec[i] - maximum;
        sum = sum + exp(vec[i]);
    }

    if (sum - EPSILON == 0) {
        cout<<"The sum computed for softmax is zero."<<endl;
        exit(0);
    }

    vector<double> result;
    for (int i = 0; i < vec.size(); i++) {
        temp = exp(vec[i]) / sum;
        //cout<<temp<<endl;
        result.push_back(temp);
    }
    return result;
}


/**
 * Substract, elemnt-wise 2 vector and return vector containing the result of this operation. This assumes
 * that the 2 vectors have the same dimensions.
 */
vector<double> substractTwoVectors(vector<double> firstVector, vector<double> secondVector) {
    vector<double> result;
    if (firstVector.size() != secondVector.size()) {
        cout<<"Couldn't subtract the 2 given vectors. They have different dimensions"<<endl;
        exit(0);
    }
    double difference;
    for (int i = 0; i < firstVector.size(); i++) {
        difference = firstVector[i] - secondVector[i];
        result.push_back(difference);
    }
    return result;
}

/**
 * Add, elemnt-wise 2 vector and return vector containing the result of this operation. This assumes
 * that the 2 vectors have the same dimensions.
 */
vector<double> addTwoVectors(vector<double> firstVector, vector<double> secondVector) {
    vector<double> result;
    if (firstVector.size() != secondVector.size()) {
        cout<<"Couldn't add the 2 given vectors. They have different dimensions"<<endl;
        exit(0);
    }
    double addition;
    for (int i = 0; i < firstVector.size(); i++) {
        addition = firstVector[i] + secondVector[i];
        result.push_back(addition);
    }
    return result;
}


// Obtain the derivative of the tanh function using (tanh x)' = 1 - ((tanh x)^2) (entry-wise).
vector<double> getTanhDerivativeFunction(vector<double> vec) {
    vector<double> result;
    double aux;
    double currentElement;
    for (int i = 0; i < vec.size(); i++) {
        currentElement = vec[i];
        aux = 1 - (currentElement * currentElement);
        result.push_back(aux);
    }
    return result;
}


// Create a vector containing d elements, all equal to 0.
vector<double> getZeros(int d) {
    vector<double> result;
    for (int i = 0; i < d; i++) {
        result.push_back(0);
    }
    return result;

}


// Print the elements in a vector container.
void printElementsOfVector(vector<double> vec) {
    for(int i = 0; i < vec.size(); i++) {
    cout<<vec[i]<<" ; ";
    }
    cout<<endl;
}

// Print the elements in a vector container.
void printElementsOfVector(vector<double> vec, ofstream f) {
    for(int i = 0; i < vec.size(); i++) {
    f<<vec[i]<<" ; ";
    }
    f<<endl;
}


// Print the elements of a given matrix.
void printElementsOfMatrix(vector<vector<double>> matrix) {
    for(int i = 0; i < matrix.size(); i++) {
        for(int j = 0; j < matrix[0].size(); j++) {
            cout<<" "<<matrix[i][j];
        }
        cout<<endl;
    }
}

// Print the elements of a given matrix.
void printElementsOfMatrix(vector<vector<double>> matrix, ofstream f) {
    for(int i = 0; i < matrix.size(); i++) {
        for(int j = 0; j < matrix[0].size(); j++) {
            f<<" "<<matrix[i][j];
        }
        f<<endl;
    }
}


// Looks at the dimensions of the two matrices and checks if the dimensions are equal.
bool checkMatricesHaveSameDimenesions(vector<vector<double>> firstMatrice, vector<vector<double>> secondMatrice) {
    int firstMatriceLines = firstMatrice.size();
    int firstMatriceColumns = firstMatrice[0].size();

    int secondMatriceLines = secondMatrice.size();
    int secondMatriceColumns = secondMatrice[0].size();

    if ((firstMatriceLines == secondMatriceLines) && (firstMatriceColumns == secondMatriceColumns)) return true;
    return false;
}

/**
 * Add, elemnt-wise 2 matrices and return a matrice containing the result of this operation. This assumes
 * that the 2 matrices have the same dimensions.
 */
vector<vector<double>> addTwoMatrices(vector<vector<double>> firstMatrice, vector<vector<double>> secondMatrice) {
    vector<vector<double>> result;
    if (firstMatrice.size() != secondMatrice.size() || firstMatrice[0].size() != secondMatrice[0].size()) {
        cout<< "The 2 given matrices cannot be added together because they don't have the same dimensions."<<endl;
        exit(0);
    }
    for(int i = 0; i < firstMatrice.size(); i++) {
        vector<double> temp;
        for(int j = 0; j < firstMatrice[0].size(); j++) {
            temp.push_back(firstMatrice[i][j] + secondMatrice[i][j]);
        }
        result.push_back(temp);
    }
    return result;
}

/**
 * Subtract, elemnt-wise 2 matrices and return a matrice containing the result of this operation. This assumes
 * that the 2 matrices have the same dimensions.
 */
vector<vector<double>> subtractTwoMatrices(vector<vector<double>> firstMatrice, vector<vector<double>> secondMatrice) {
    vector<vector<double>> result;
    if (firstMatrice.size() != secondMatrice.size() || firstMatrice[0].size() != secondMatrice[0].size()) {
        cout<< "The 2 given matrices cannot be added together because they don't have the same dimensions."<<endl;
        exit(0);
    }

    for(int i = 0; i < firstMatrice.size(); i++) {
        vector<double> temp;
        for(int j = 0; j < firstMatrice[0].size(); j++) {
            temp.push_back(firstMatrice[i][j] - secondMatrice[i][j]);
        }
        result.push_back(temp);
    }
    return result;
}

// Multiply matrix by scalar.
vector<vector<double>> multiplyMatrixByScalar(vector<vector<double>> matrix, double scalar) {
    vector<vector<double>> result;
    for(int i = 0; i < matrix.size(); i++) {
        vector<double> temp;
        for(int j = 0; j < matrix[0].size(); j++) {
            temp.push_back(matrix[i][j] * scalar);
        }
        result.push_back(temp);
    }
    return result;
}

// Transpose a row vector. Return a matrix representing the column vector.
vector<vector<double>> transposeRowVector(vector<double> vec) {
    vector<vector<double>> result;
    for(int i = 0; i < vec.size(); i++) {
        vector<double> temp;
        temp.push_back(vec[i]);
        result.push_back(temp);
    }
    return result;
}


// Create a weight matrix containing only 0s of dimensions d x 2d.
vector<vector<double>> getZerosWeightMatrix(int d) {
    vector<vector<double>> result;
    for(int i = 0; i < d; i++) {
        vector<double> temp;
        for(int j = 0; j < 2*d; j++) {
            temp.push_back(0);
        }
        result.push_back(temp);
    }
    return result;
}

// Matrix multiplication
vector<vector<double>> multiplyMatrices(vector<vector<double>> matrix, vector<double> vec) {
    vector<vector<double>> result;
    double s = 0; // current sum
    int n = matrix.size();
    for (int i = 0; i < n; i++) {
        vector<double> temp;
        for (int j = 0;j < vec.size(); j++) {
            temp.push_back(matrix[i][0] * vec[j]);
        }
        result.push_back(temp);
    }
    return result;
}

// Craete an empty Sentiment Weight matrix of dimension 2 x 25.S
vector<vector<double>> getEmptySentimentWeightMatrix() {
    vector<vector<double>> result;
    vector<double> temp;
    for(int i = 0; i < 25; i++) {
        temp.push_back(0);
    }
    result.push_back(temp);
    result.push_back(temp);
    return result;
}

// Matrix multiplication.
double getNormOfMatrix(vector<vector<double>> m1) {
    double result;
    double s = 0;
    for (int i = 0; i < m1.size(); i++)
        for (int j = 0; j < m1[0].size(); j++) {
            s = s + m1[i][j] * m1[i][j];
        }
    result = sqrt(s);
    return result;
}

// Multiply vector by scalar.
vector<double> multiplyVectorByScalar(vector<double> vec, double scalar) {
    vector<double> result;
    for(int i = 0; i < vec.size(); i++) {
        result.push_back(vec[i] * scalar);
        }
    return result;
}


double getNormOfVector(vector<double> vec) {
    double result;
    double s = 0;
    for (int i = 0; i < vec.size(); i++) {
            s = s + vec[i] * vec[i];
        }
    result = sqrt(s);
    return result;
}
