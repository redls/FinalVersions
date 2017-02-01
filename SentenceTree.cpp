#include "SentenceTree.h"
#include "MatrixOperations.h"
#include<string>
#include<cstdio>
#include<iostream>
#include<fstream>
using namespace std;

/**
 * Contains methods which interact with a given sentence's tree.
 */


 double distanceBetweenTwoVectors(vector<double> vec1, vector<double> vec2, vector<vector<double>> sentimentWeightMatrix, vector<double> product) {
    double score = 0;
   /* for(int i = 0; i < vec1.size(); i++) {
        score = score + (vec1[i] - vec2[i])*(vec1[i] - vec2[i]);
    } */
    vector<double> afterTanh = applyTanhElementWise(product);
    vector<double> softmaxResult = softmax(matrixMultplicationWithVector(sentimentWeightMatrix, afterTanh));
    if (softmaxResult[0] - softmaxResult[1] > 0) return softmaxResult[0] - softmaxResult[1];
    else softmaxResult[1] - softmaxResult[0];
    return score;
 }

// Given a sentence, construct its semantic tree by combining the words using the tanh rule.
Tree* constructTreeForASentence(string sentence, vector<vector<double>> weights, vector<vector<double>> sentimentWeightScore, Vocabulary *vocab) {
    vector<string> words = getWordsFromSentence(sentence);
    vector<Node> nodes = retrieveWordRepresentation(words, vocab);
    vector<pair<int,int>> pairedElem;
    vector<Tree*> trees;
    Tree *t;

    // Convert the nodes into trees with height 1.
    for(int i = 0; i < nodes.size();i++) {
        t = new Tree(nodes[i]);
        if (i < words.size()) {
            t->setWord(words[i]);
        }
        trees.push_back(t);
    }

    int numberOfWords = trees.size();

    // Merge all the small trees into only one tree.
    while (numberOfWords > 1) {
        double score = 0;
        double maxScore = 0;
        vector<double> concatenation = concatenateTwoVectors(trees[0]->getRootRepresentation(),
                    trees[1]->getRootRepresentation());
        vector<double> product = matrixMultplicationWithVector(weights, concatenation);
        vector<double> afterTanh = applyTanhElementWise(product);
        maxScore = distanceBetweenTwoVectors(trees[0]->getRootRepresentation(), trees[1]->getRootRepresentation(), sentimentWeightScore, product);
        int left = 0;
        int right = 1;
        for(int i = 1; i < trees.size(); i++) {
            concatenation = concatenateTwoVectors(trees[i-1]->getRootRepresentation(),
                    trees[i]->getRootRepresentation());
            product = matrixMultplicationWithVector(weights, concatenation);
            if (isnan(product[i])) {
                cout<<"The product has nan elements. This was found in constructTreeForASentence() method."<<endl;
                cout<<sentence<<endl;
                exit(0);
            }
            afterTanh = applyTanhElementWise(product);
           //. score = distanceBetweenTwoVectors(trees[i-1]->getRootRepresentation(),
                 //   trees[i]->getRootRepresentation());

            score = distanceBetweenTwoVectors(trees[i-1]->getRootRepresentation(),
                   trees[i]->getRootRepresentation(), sentimentWeightScore, product);
            // Check if the new node would hove a higer score. IF it has, change the variables
            // left and right to point to the positions of the two nodes.
            if (score > maxScore) {
                maxScore = score;
                left = i-1;
                right = i;
            }
        }
        // Recompute the value of the new node.
        concatenation = concatenateTwoVectors(trees[left]->getRootRepresentation(),
                    trees[right]->getRootRepresentation());
        product = matrixMultplicationWithVector(weights, concatenation);
        afterTanh = applyTanhElementWise(product);
        for(int i = 0;i < afterTanh.size(); i++) {
            if (isnan(afterTanh[i])) {
                cout<<"NAN is located in constructTreeForASentence() method, after applying tanh"<<endl;
                printElementsOfVector(product);
                //afterTanh[i] = 0;
                exit(0);
            }
        }
        // Create new node which has as children the 2 nodes, and as the root the computed
        // score.
        Tree *merged = new Tree(afterTanh);
        merged->setScore(softmax(matrixMultplicationWithVector(sentimentWeightScore, afterTanh)));

        merged->setLeftTree(trees[left]);
        merged->setRightTree(trees[right]);

        // Replace the 2 nodes by the new computed parent: add the root of the new node into the
        // vector trees (on the left position and shift the other nodes by one position to left).
        trees[left] = merged;
        for(int i = right + 1; i < trees.size();i++) {
           // delete()
            trees[i-1] = trees[i];
        }
        trees.pop_back();
        pairedElem.push_back(make_pair(left,right));
        numberOfWords--;
    }
    // Tells us what is the score of the root of the complete tree of  the sentence.
    vector<double> score = matrixMultplicationWithVector(sentimentWeightScore, trees[0]->getRootRepresentation());
    //for (int i = 0; i < 2; i++) {
        //cout<<"negative: "<<score[0]<<"\n positive"<<score[1]<< endl;
    //}
    return trees[0];
}



// Create a container of size 2 (number of classes) containing in both positions a specific value, d.
vector<double> createTemporaryNodeRepresentation(int d) {
    vector<double> wordRep;
    for(int i = 0;i < 2; i++) {
        wordRep.push_back(d);
    }
    return wordRep;
}

// Find an element in an int array and return its index.
int findFirstOccurenceOfElement(int a[], int elem, int length) {
    for (int i =  0; i < length; i++) {
        if (a[i] == elem) return i;
    }
    return -1;
}

// Find the 2 positions, i and j, in the array for which a[i]=a[j]=elem
pair<int,int> findElementInArray(int a[], int length, int elem) {
    pair<int,int> result = make_pair(-1,-1);
    int c = 0;
    for( int i = 0;i < length; i++) {
        if (a[i] == elem && c == 0) {
            result.first = i;
            c++;
        } else if (a[i] == elem && c == 1) {
            result.second = i;
            c++;
            break;
        }
    }
    return result;
}


// Assign the labels of to the tree in a post-order way.
string assignRightLabels(Tree* t, vector<string> words, Dictionary* dictionary, SentimentLabels* sentimentLabels, int &numberOfLeaves) {
    if (t == nullptr) {
        cout<<"I am null"<<endl;
        return "";
    }

    // The current node is a leaf, so we compute its score by searching the word in the sentiment labels map.
    if (t->getLeftTree() == nullptr && t->getRightTree() == nullptr) {
        long long phraseIndex = dictionary->getPhraseIndex(words[numberOfLeaves]);
        double score = sentimentLabels->getSentimentScore(phraseIndex);
        if (phraseIndex == -1) cout<<words[numberOfLeaves]<<" not found in the dictionary."<<endl;
        if (score >= 0.5) {
            // case when the word is positive.
            vector<double> root = t->getRootRepresentation();
            root[0] = 0;
            root[1] = 1;
            t->setRoot(root);
            t->setWord(words[numberOfLeaves]);
        } else {
            // case when the word is negative.
            vector<double> root = t->getRootRepresentation();
            root[0] = 1;
            root[1] = 0;
            t->setRoot(root);
            t->setWord(words[numberOfLeaves]);
        }
        numberOfLeaves++;
        return words[numberOfLeaves - 1];
    }

    // The current node is an inner node, compute both left and right trees and then compute the value for the tree;
    string left = assignRightLabels(t->getLeftTree(), words, dictionary, sentimentLabels, numberOfLeaves);
    string right = assignRightLabels(t->getRightTree(), words, dictionary, sentimentLabels, numberOfLeaves);
    string partialPhrase = left + " " + right;
    long long phraseIndex = dictionary->getPhraseIndex(partialPhrase);
    if (phraseIndex == -1) cout<<partialPhrase<<" not found in the dictionary"<<endl;
    double score = sentimentLabels->getSentimentScore(phraseIndex);
    if (score >= 0.5) {
        // case when the word is positive.
        vector<double> root = t->getRootRepresentation();
        root[0] = 0;
        root[1] = 1;
        t->setRoot(root);
    } else {
        // case when the word is negative.
        vector<double> root = t->getRootRepresentation();
        root[0] = 1;
        root[1] = 0;
        t->setRoot(root);
    }
    return partialPhrase;
}


// Update the given tree by merging the given branch to the tree.
void updateTree(Tree* t, vector<int> branch) {
    Tree* temp = t;
    int s = branch.size() - 2;
    while (s >= 0) {
        int value = branch[s];
        if (temp->getLeftTree() == nullptr) {
            Node* node = new Node(createTemporaryNodeRepresentation(value));
            Tree* newTree= new Tree(*node);
            temp->setLeftTree(newTree);
            temp = temp->getLeftTree();
        } else {
            Tree * leftChild = temp->getLeftTree();
            vector<double> leftChildRepresentation = leftChild->getRootRepresentation();
            if (leftChildRepresentation[0] == value) {
                temp = temp->getLeftTree();
            } else {
                Tree * rightChild = temp->getRightTree();
                if (rightChild == nullptr) {
                    Node* node = new Node(createTemporaryNodeRepresentation(value));
                    Tree* newTree= new Tree(*node);
                    temp->setRightTree(newTree);
                    temp = temp->getRightTree();
                } else {
                    vector<double> rightChildRepresentation = rightChild->getRootRepresentation();
                    if (rightChildRepresentation[0] == value) {
                        temp = temp->getRightTree();
                    } else cout<<"Error in the construction of the parse tree."<<endl;
                }
            }
        }
        s--;
    }
}

// Create tree such that the leaves have indices in ascending order
Tree* constructTree(int a[], int length, int numberOfLeaves) {
    int indexRoot = findFirstOccurenceOfElement(a, 0, length);
    indexRoot++;
    Node* temp = new Node(createTemporaryNodeRepresentation(indexRoot));
    Tree* parent = new Tree(*temp);
    delete(temp);
    vector<vector<int>> listOfBranches;
    for (int i = 0; i < numberOfLeaves; i++) {
        int j = i;
        vector<int> tempList;
        while (j <= length) {
            tempList.push_back(j + 1);
            if (a[j] == 0) break;
            j = a[j] - 1;
        }
        updateTree(parent, tempList);
        //parent->inOrderTraversal();
    }
    return parent;
}


// Given a string of digits and | symbols, return the target tree represented by that string.
Tree* constructTargetTree(string treeText, string sentence, Dictionary* dictionary, SentimentLabels* sentimentLabels) {
    vector<string> words = getWordsFromSentence(sentence);

    // Find the number of nodes in the tree and also retrieve the positions from treeText.
    int numberOfNodes = 0;
    for (char & c: treeText) {
        if (c == '|') numberOfNodes++;
    }
    numberOfNodes++;
    int positions[numberOfNodes] ;
    int number = 0;
    int k = 0;
    for (char & c: treeText) {
        if (c != '|') number = number * 10 + c - '0';
        else {
            positions[k] = number;
            k++;
            number = 0;
        }
    }

    positions[k] = number; // add the last int in the array.
    // Create tree by using temporary values for the inner nodes. Start by finding the root (0) in the array.
    int indexRoot = findFirstOccurenceOfElement(positions, 0, numberOfNodes);
    // Increase the index of the root by one since the array should be indexed from 1.
    indexRoot++;

    Tree* root = constructTree(positions, numberOfNodes, words.size());
    //cout<<"Index of the root: "<<indexRoot<<endl;
    int nr = 0;
    string x = assignRightLabels(root, words, dictionary, sentimentLabels, nr);
    if (root->getLeftTree() == nullptr) cout<<"XOXO"<<endl;
    return root;
}



RNNParam* backprop(Tree * targetTree, Tree * computedTree, vector<vector<double>> weightScoresMatrix, vector<vector<double>> weightsMatrix, vector<double> parentError, Vocabulary* vocab) {
    // Find the transpose matrixes.
    vector<vector<double>> weightScoresMatrixTranspose = getTransposeMatrix(weightScoresMatrix);
    vector<vector<double>> weightsMatrixTranspose = getTransposeMatrix(weightsMatrix);

    Tree* copyOfTargetTree = targetTree;
    RNNParam* rnnParam = new RNNParam();
    vector<double> targetRootRepresentation;
     if (targetTree == nullptr) {
        targetRootRepresentation = getZeros(2);
        copyOfTargetTree = new Tree(targetRootRepresentation);
        delete(copyOfTargetTree);
        return rnnParam;
    } else {
        targetRootRepresentation = targetTree->getRootRepresentation();
        }

    vector<vector<double>> resultSentimentMatrix;
    resultSentimentMatrix.push_back(getZeros(25));
    resultSentimentMatrix.push_back(getZeros(25));

   // rnnParam->setSentimentWeightsMatrix(resultSentimentMatrix);
   // rnnParam->setWeightsMatrix(getZerosWeightMatrix(25));

    // Retrieve the current node vector.
    vector<double> currentNodeVectorRepresentation = computedTree->getRootRepresentation();

    // Compute predictions of the current node.
    vector<double> softmaxResult = softmax(matrixMultplicationWithVector(weightScoresMatrix, currentNodeVectorRepresentation));

    // Subtract the actual predictions from the softmax.
    vector<double> difference = softmaxResult;

    difference[0] = difference[0] - targetRootRepresentation[0];
    difference[1] = difference[1] - targetRootRepresentation[1];

    // Compute the error for the Sentiment Weight Matrix at this node.
    for(int i = 0; i < difference.size(); i++) {
        for(int j = 0; j < currentNodeVectorRepresentation.size(); j++) {
            resultSentimentMatrix[i][j] = (difference[i])*currentNodeVectorRepresentation[j];
        }
    }
    //vector<double> difference = substractTwoVectors(softmaxResult, targetRootRepresentation);

    vector<double> deltaScore = matrixMultplicationWithVector(weightScoresMatrixTranspose, difference);

    vector<double> afterTanhDeriv = getTanhDerivativeFunction(currentNodeVectorRepresentation);

    deltaScore = getVectorHadamardProduct(deltaScore, afterTanhDeriv);

    vector<double> deltaTotal = addTwoVectors(deltaScore, parentError);


    vector<double> newParentError = matrixMultplicationWithVector(weightsMatrixTranspose, deltaTotal);

    // Compute error for children.
    vector<double> leftChild = getZeros(25);
    vector<double> rightChild = getZeros(25);

    // This means the current node is a leaf, return the error only for this node.
    if (computedTree->getLeftTree() == nullptr) {
        rnnParam->setSentimentWeightsMatrix(resultSentimentMatrix);
        rnnParam->updateVocabError(computedTree->getWord(), deltaTotal);
        return rnnParam;
    }
    // This means the node is not a leaf. Retrieve the vector representations of the two children.
    leftChild =  computedTree->getLeftTree()->getRootRepresentation();
    rightChild =  computedTree->getRightTree()->getRootRepresentation();

    // Concatenate the two children.
    vector<double> mergedChildren = concatenateTwoVectors(leftChild, rightChild);

    // Transpose the total error already obtained at this node.
    vector<vector<double>> deltaTransposed = transposeRowVector(deltaTotal);

    // Compute the total error at this node and obtain the error matrices.
    vector<vector<double>> deltaMatrix = multiplyMatrices(deltaTransposed, mergedChildren);


    // Apply derivate of tanh to the children.
    mergedChildren = getTanhDerivativeFunction(mergedChildren);

    newParentError = getVectorHadamardProduct(newParentError, mergedChildren);

    vector<double> leftChildError;
    vector<double> rightChildError;
    for (int i = 0; i < newParentError.size(); i++) {
        if (i < newParentError.size()/2) leftChildError.push_back(newParentError[i]);
        else rightChildError.push_back(newParentError[i]);
    }

    RNNParam* leftRNNParam = backprop(copyOfTargetTree->getLeftTree(), computedTree->getLeftTree(), weightScoresMatrix, weightsMatrix, leftChildError, vocab);
    RNNParam* rightRNNParam = backprop(copyOfTargetTree->getRightTree(), computedTree->getRightTree(), weightScoresMatrix, weightsMatrix, rightChildError, vocab);

    rnnParam->updateVocabError(leftRNNParam->getVocabError(), rightRNNParam->getVocabError());

    rnnParam->setWeightsMatrix(deltaMatrix);
    rnnParam->updateWeightsMatrix(leftRNNParam->getWeightsMatrix());
    rnnParam->updateWeightsMatrix(rightRNNParam->getWeightsMatrix());

    rnnParam->setSentimentWeightsMatrix(resultSentimentMatrix);
    rnnParam->updateSentimentWeightsMatrix(leftRNNParam->getSentimentWeightsMatrix());
    rnnParam->updateSentimentWeightsMatrix(rightRNNParam->getSentimentWeightsMatrix());

    delete(leftRNNParam);
    delete(rightRNNParam);

    return rnnParam;
}



// Assign the labels of to the tree in a post-order way.
void assignParsingTreeLabels(Tree* t, vector<string> words, Dictionary* dictionary, Vocabulary* vocab, int &numberOfLeaves,
            vector<vector<double>> weightScoresMatrix, vector<vector<double>> weightsMatrix) {
    if (t == nullptr) {
        cout<<"I am null"<<endl;
        return;
    }
    // The current node is a leaf, so we compute its score by searching the word in the sentiment labels map.
    if (t->getLeftTree() == nullptr && t->getRightTree() == nullptr) {
        vector<double> leafRepresentation = vocab->getWordRepresentation(words[numberOfLeaves]);
        if (leafRepresentation.empty()) {
            cout<<words[numberOfLeaves]<<" not found in the dictionary."<<endl;
            vocab->addNewWord(words[numberOfLeaves]);
            leafRepresentation = vocab->getWordRepresentation(words[numberOfLeaves]);
            //cout<<"klkdlskdkslkd"<<endl;
        }
        for(int i = 0;i < leafRepresentation.size(); i++) {
            if (isnan(leafRepresentation[i])) {
                cout<<"NAN is located in assignParsingTreeLabels() method, in assigning the leaf a value."<<endl;
                printElementsOfVector(leafRepresentation);
                exit(0);
            }
        }
        vector<double> root = leafRepresentation;
        t->setRoot(root);
        t->setWord(words[numberOfLeaves]);
        numberOfLeaves++;
        return;
    }

    if ((t->getLeftTree() != nullptr && t->getRightTree() == nullptr) ||
            (t->getLeftTree() == nullptr && t->getRightTree() != nullptr)) {
        cout<<"The tree is not binary."<<endl;
        exit(0);
    }

    // The current node is an inner node, compute both left and right trees and then compute the value for the tree;
    if (t->getLeftTree() != nullptr && t->getRightTree() != nullptr) {

    assignParsingTreeLabels(t->getLeftTree(), words, dictionary, vocab, numberOfLeaves, weightScoresMatrix, weightsMatrix);
    assignParsingTreeLabels(t->getRightTree(), words, dictionary, vocab, numberOfLeaves, weightScoresMatrix, weightsMatrix);

    vector<double> concatenation = concatenateTwoVectors(t->getLeftTree()->getRootRepresentation(),
                    t->getRightTree()->getRootRepresentation());
    if (concatenation.size() !=50) {
        cout<<"The concatenation of the children does not have the right dimension."<<endl;
        exit(0);
    }
    vector<double> product = matrixMultplicationWithVector(weightsMatrix, concatenation);

    for(int i = 0; i < product.size(); i++) {
        if (isnan(product[i])) {
                    cout<<"The product has nan elements. This was found in assignParsingTreeLabels() method."<<endl;
                    printElementsOfVector(product);
                    cout<<endl;
                    printElementsOfVector(concatenation);
                    cout<<endl;
                    printElementsOfMatrix(weightsMatrix);
                    cout<<endl;
                    printElementsOfMatrix(weightScoresMatrix);
                    cout<<words[0] + words[1] + words[2]<<endl;
                    exit(0);
                }
    }
    vector<double> afterTanh = applyTanhElementWise(product);

   for(int i = 0;i < afterTanh.size(); i++) {
            if (isnan(afterTanh[i])) {
                cout<<"NAN is located in assignParsingTreeLabels() method, after applying tanh"<<endl;
                printElementsOfVector(product);
                exit(0);
            }
        }
    // Set score of the current node.
    t->setScore(softmax(matrixMultplicationWithVector(weightScoresMatrix, afterTanh)));
    // Set the new node value.
    t->setRoot(afterTanh);
    }
}

// Read from the PreprocessedDatasetSentences.txt. Map each tree representation with its line position in the file.
unordered_map<long long, string> readParsedTrees() {
    ifstream input("PreprocessedDatasetSentences.txt");
    //ifstream input("stanfordSentimentTreebank/STree.txt");
    string line;
    unordered_map<long long, string> result;
    long long counter = 0;
     while(getline(input, line)) {
         counter++;
         result.insert(make_pair(counter, line));
    }
    return result;
}

void checkVectorRepresentationHaveSize25(Tree* t) {
    if (t == nullptr) return;
    if (t->getRootRepresentation().size() != 25) cout<<"!!!!!!!! "<<t->getRootRepresentation().size();
    checkVectorRepresentationHaveSize25(t->getLeftTree());
    checkVectorRepresentationHaveSize25(t->getRightTree());
}

// Use the parsing tring in the forward propogation.
Tree* useParserForCreatingTheTree(string treeText, string sentence, Dictionary* dictionary, Vocabulary* vocab,
    vector<vector<double>> weightScoresMatrix, vector<vector<double>> weightsMatrix) {
    vector<string> words = getWordsFromSentence(sentence);
    // Find the number of nodes in the tree and also retrieve the positions from treeText.
    int numberOfNodes = 0;
    for (char & c: treeText) {
        if (c == '|') numberOfNodes++;
    }
    numberOfNodes++;
    int positions[numberOfNodes] ;
    int number = 0;
    int k = 0;
    for (char & c: treeText) {
        if (c != '|') number = number * 10 + c - '0';
        else {
            positions[k] = number;
            k++;
            number = 0;
        }
    }

    positions[k] = number; // add the last int in the array.

    // Create tree by using temporary values for the inner nodes. Start by finding the root (0) in the array.
    int indexRoot = findFirstOccurenceOfElement(positions, 0, numberOfNodes);

    // Increase the index of the root by one since the array should be indexed from 1.
    indexRoot++;
    if (2 * words.size() - 1 == indexRoot) {
    Tree* root = constructTree(positions, numberOfNodes, words.size());
    int nr = 0;

    // Compute the values of the inner nodes and leaves of the tree.
    assignParsingTreeLabels(root, words, dictionary, vocab, nr, weightScoresMatrix, weightsMatrix);
    if (root->getLeftTree() == nullptr) {
        cout<<"The root of the parse tree has left child null => not binary tree."<<endl;
        exit(0);
        }
    checkVectorRepresentationHaveSize25(root);
    return root;
    } else return constructTreeForASentence(sentence, weightsMatrix, weightScoresMatrix, vocab);
}
