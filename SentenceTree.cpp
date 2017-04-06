#include "SentenceTree.h"
#include "MatrixOperations.h"
#include<string>
#include<cstdio>
#include<iostream>
#include<fstream>
using namespace std;
#define WORD_SIZE_VEC 30

/**
 * Contains methods which interact with a given sentence's tree.
 */


 double distanceBetweenTwoVectors(vector<double> vec1, vector<double> vec2, vector<vector<double>> sentimentWeightMatrix, vector<double> product, vector<double> biasSentimentMatrix) {
    double score = 0;
   /* for(int i = 0; i < vec1.size(); i++) {
        score = score + (vec1[i] - vec2[i])*(vec1[i] - vec2[i]);
    } */
    vector<double> afterTanh = applyTanhElementWise(product);
    vector<double> softmaxResult = softmax(addTwoVectors(biasSentimentMatrix, matrixMultplicationWithVector(sentimentWeightMatrix, afterTanh)));
    if (softmaxResult[0] - softmaxResult[1] > 0) return softmaxResult[0] - softmaxResult[1];
    else softmaxResult[1] - softmaxResult[0];
    return score;
 }

// Given a sentence, construct its semantic tree by combining the words using the tanh rule.
Tree* constructTreeForASentence(string sentence, vector<vector<double>> weights, vector<vector<double>> sentimentWeightScore,
        Vocabulary *vocab, vector<double> biasSentimentMatrix, vector<double> biasWeights) {
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
        product = addTwoVectors(product, biasWeights);
        vector<double> afterTanh = applyTanhElementWise(product);
        maxScore = distanceBetweenTwoVectors(trees[0]->getRootRepresentation(), trees[1]->getRootRepresentation(), sentimentWeightScore, product, biasSentimentMatrix);
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
            product = addTwoVectors(product, biasWeights);
            afterTanh = applyTanhElementWise(product);
           //. score = distanceBetweenTwoVectors(trees[i-1]->getRootRepresentation(),
                 //   trees[i]->getRootRepresentation());

            score = distanceBetweenTwoVectors(trees[i-1]->getRootRepresentation(),
                   trees[i]->getRootRepresentation(), sentimentWeightScore, product, biasSentimentMatrix);
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
        product = addTwoVectors(product, biasWeights);
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
        merged->setScore(addTwoVectors(softmax(matrixMultplicationWithVector(sentimentWeightScore, afterTanh)), biasSentimentMatrix));

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

//    delete(t);
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
        if (phraseIndex == -1) cout<<words[numberOfLeaves]<<" not found in the dictionary while constructing the target tree***"<<endl;
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
	    delete(node);
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
		    delete(node);
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



RNNParam* backprop(Tree * targetTree, Tree * computedTree, vector<vector<double>> weightScoresMatrix, vector<vector<double>> weightsMatrix, vector<double> parentError,
        vector<double> biasSentiment) {
    // Find the transpose matrixes.
    vector<vector<double>> weightScoresMatrixTranspose = getTransposeMatrix(weightScoresMatrix);
    vector<vector<double>> weightsMatrixTranspose = getTransposeMatrix(weightsMatrix);

    RNNParam* rnnParam = new RNNParam();
    vector<double> targetRootRepresentation;
     if ((targetTree == nullptr && computedTree != nullptr) || (targetTree != nullptr && computedTree == nullptr)) {
        cout<<"copacei difereti"<<endl;
        return rnnParam;
    } else {
        targetRootRepresentation = targetTree->getRootRepresentation();
        }

    vector<double> zeros25 = getZeros(WORD_SIZE_VEC);
    vector<vector<double>> resultSentimentMatrix;
    resultSentimentMatrix.push_back(zeros25);
    resultSentimentMatrix.push_back(zeros25);


    if ((targetRootRepresentation[0] == 1 && targetRootRepresentation[1] == 1) ||
        (targetRootRepresentation[0] == 0 && targetRootRepresentation[1] == 0)) {
            cout<<"Tree is constructed badly i backprop"<<endl;
            exit(0);
        }


    // Retrieve the current node vector.
    vector<double> currentNodeVectorRepresentation = computedTree->getRootRepresentation();

    // Compute predictions of the current node.
//    vector<double> softmaxResult = softmax(addTwoVectors(biasSentiment, matrixMultplicationWithVector(weightScoresMatrix, currentNodeVectorRepresentation)));
    vector<double> softmaxResult = computedTree->getScore();

    // Subtract the actual predictions from the softmax.
    vector<double> difference = softmaxResult;

    double totalError = 0;

    if (targetRootRepresentation[0] == 1) {
        totalError += log((double)difference[0]);
        difference[0] = difference[0] - 1;
    }
    if (targetRootRepresentation[1] == 1) {
        totalError += log((double)difference[1]);
        difference[1] = difference[1] - 1;
    }


    vector<double> biasSentimentMatrix = difference;
    rnnParam->updateBiasSentimentMatrix(biasSentimentMatrix);

    vector<double> biasWeightsMatrix;

    // Compute the error for the Sentiment Weight Matrix at this node.
    for(int i = 0; i < difference.size(); i++) {
        for(int j = 0; j < currentNodeVectorRepresentation.size(); j++) {
            resultSentimentMatrix[i][j] = (difference[i])*currentNodeVectorRepresentation[j];
        }
    }
    rnnParam->updateSentimentWeightsMatrix(resultSentimentMatrix);

    vector<double> deltaScore = matrixMultplicationWithVector(weightScoresMatrixTranspose, difference);

    for(int i = 0;i < deltaScore.size(); i++) {
            if (isnan((double)deltaScore[i])) {
                 cout<<"NAN is located when computing first delta score"<<endl;
                printElementsOfVector(deltaScore);
                cout<<endl<<endl;
                printElementsOfVector(difference);
                cout<<endl<<endl;
                printElementsOfVector(targetRootRepresentation);
                cout<<endl<<endl;
                 printElementsOfMatrix(weightScoresMatrix);
                cout<<endl<<endl;
                exit(0);
            }
        }
    // Add parent error.
    deltaScore = addTwoVectors(deltaScore, parentError);

    // Compute f' (which in this case is tanh').
    vector<double> afterTanhDeriv = getTanhDerivativeFunction(currentNodeVectorRepresentation);

    for(int i = 0;i < afterTanhDeriv.size(); i++) {
            if (isnan((double)afterTanhDeriv[i])) {
                 cout<<"NAN is located when computing tanh deruvatuve"<<endl;
                printElementsOfVector(afterTanhDeriv);
                cout<<endl<<endl;
                printElementsOfVector(currentNodeVectorRepresentation);
                cout<<endl<<endl;
                exit(0);
            }
        }


     // deltaScore = getVectorHadamardProduct(deltaScore, afterTanhDeriv);
      for(int i = 0;i < deltaScore.size(); i++) {
         if (currentNodeVectorRepresentation[i] <= 0) deltaScore[i] = 0;
      }


     for(int i = 0;i < deltaScore.size(); i++) {
            if (isnan((double)deltaScore[i])) {
                 cout<<"NAN is located when computing delta score"<<endl;
                printElementsOfVector(deltaScore);
                cout<<endl<<endl;
                printElementsOfVector(afterTanhDeriv);
                cout<<endl<<endl;
                exit(0);
            }
        }

    vector<double> deltaTotal = deltaScore;

    // This means the current node is a leaf, return the error only for this node.
    if (computedTree->getLeftTree() == nullptr) {
        rnnParam->updateVocabError(computedTree->getWord(), deltaScore);
        return rnnParam;
    }


    biasWeightsMatrix = deltaScore;
    rnnParam->updateBiasWeightMatrix(biasWeightsMatrix);

    vector<double> newParentError = matrixMultplicationWithVector(weightsMatrixTranspose, deltaScore);

    // Compute error for children. This means the node is not a leaf.
    // Retrieve the vector representations of the two children.
    vector<double> leftChild =  computedTree->getLeftTree()->getRootRepresentation();
    vector<double> rightChild =  computedTree->getRightTree()->getRootRepresentation();

    // Concatenate the two children.
    vector<double> mergedChildren = concatenateTwoVectors(leftChild, rightChild);

    // Transpose the total error already obtained at this node.
    vector<vector<double>> deltaTransposed = transposeRowVector(deltaScore);

    // Compute the total error at this node and obtain the error matrices.
    vector<vector<double>> deltaMatrix = multiplyMatrices(deltaTransposed, mergedChildren);

    rnnParam->updateWeightsMatrix(deltaMatrix);

    for(int i = 0;i < newParentError.size(); i++) {
         if (mergedChildren[i] <= 0) newParentError[i] = 0;
      }


    vector<double> leftChildError;
    vector<double> rightChildError;
    for (int i = 0; i < newParentError.size(); i++) {
        if (i < newParentError.size()/2) leftChildError.push_back(newParentError[i]);
        else rightChildError.push_back(newParentError[i]);
    }

    RNNParam* leftRNNParam = backprop(targetTree->getLeftTree(), computedTree->getLeftTree(), weightScoresMatrix, weightsMatrix, leftChildError, biasSentiment);
    RNNParam* rightRNNParam = backprop(targetTree->getRightTree(), computedTree->getRightTree(), weightScoresMatrix, weightsMatrix, rightChildError, biasSentiment);

    rnnParam->updateVocabError(leftRNNParam->getVocabError(), rightRNNParam->getVocabError());

    rnnParam->updateBiasSentimentMatrix(leftRNNParam->getBiasSentimentMatrix());
    rnnParam->updateBiasSentimentMatrix(rightRNNParam->getBiasSentimentMatrix());

    rnnParam->updateBiasWeightMatrix(leftRNNParam->getBiasWeightMatrix());
    rnnParam->updateBiasWeightMatrix(rightRNNParam->getBiasWeightMatrix());

    rnnParam->updateWeightsMatrix(leftRNNParam->getWeightsMatrix());
    rnnParam->updateWeightsMatrix(rightRNNParam->getWeightsMatrix());

    rnnParam->updateSentimentWeightsMatrix(leftRNNParam->getSentimentWeightsMatrix());
    rnnParam->updateSentimentWeightsMatrix(rightRNNParam->getSentimentWeightsMatrix());

    rnnParam->updateTotalError(totalError);
    rnnParam->updateTotalError(leftRNNParam->getTotalError());
    rnnParam->updateTotalError(rightRNNParam->getTotalError());

    delete(leftRNNParam);
    delete(rightRNNParam);

    return rnnParam;
}



// Assign the labels of to the tree in a post-order way.
void assignParsingTreeLabels(Tree* t, vector<string> words, Vocabulary* vocab, int &numberOfLeaves,
            vector<vector<double>> weightScoresMatrix, vector<vector<double>> weightsMatrix, vector<double> biasSentimentMatrix, vector<double> biasWeights) {
    if (t == nullptr) {
        cout<<"I am null"<<endl;
        return;
    }
    // The current node is a leaf, so we compute its score by searching the word in the sentiment labels map.
    if (t->getLeftTree() == nullptr && t->getRightTree() == nullptr) {
        vector<double> leafRepresentation = vocab->getWordRepresentation(words[numberOfLeaves]);
        if (leafRepresentation.empty()) {
            cout<<words[numberOfLeaves]<<" not found in the dictionary."<<endl;
          //  vocab->addNewWord(words[numberOfLeaves]);
            leafRepresentation = vocab->getWordRepresentation("UNKNOWN");
//            cout<<"HUUUGE PROBLEM IN VOCABULARY"<<endl;
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
	t->setScore(softmax(addTwoVectors(matrixMultplicationWithVector(weightScoresMatrix, root), biasSentimentMatrix)));
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

    assignParsingTreeLabels(t->getLeftTree(), words, vocab, numberOfLeaves, weightScoresMatrix, weightsMatrix, biasSentimentMatrix, biasWeights);
    assignParsingTreeLabels(t->getRightTree(), words, vocab, numberOfLeaves, weightScoresMatrix, weightsMatrix, biasSentimentMatrix, biasWeights);

    vector<double> concatenation = concatenateTwoVectors(t->getLeftTree()->getRootRepresentation(),
                    t->getRightTree()->getRootRepresentation());

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
    product = addTwoVectors(product, biasWeights);
    for(int i = 0; i < product.size(); i++) {
        if (product[i] <= 0) product[i] = 0;
    }
   // vector<double> afterTanh = applyTanhElementWise(product);
    vector<double> afterTanh = product;

   for(int i = 0;i < afterTanh.size(); i++) {
            if (isnan(afterTanh[i])) {
                cout<<"NAN is located in assignParsingTreeLabels() method, after applying tanh"<<endl;
                printElementsOfVector(product);
                exit(0);
            }
        }
    // Set score of the current node.
    t->setScore(softmax(addTwoVectors(matrixMultplicationWithVector(weightScoresMatrix, afterTanh), biasSentimentMatrix)));

    // Set the new node value.
    t->setRoot(afterTanh);
    }
}

// Read from the PreprocessedDatasetSentences.txt. Map each tree representation with its line position in the file.
unordered_map<long long, string> readParsedTrees() {
   // ifstream input("PreprocessedDatasetSentences.txt");
    ifstream input("stanfordSentimentTreebank/STree.txt");
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
    if (t->getRootRepresentation().size() != WORD_SIZE_VEC) cout<<"!!!!!!!! "<<t->getRootRepresentation().size();
    checkVectorRepresentationHaveSize25(t->getLeftTree());
    checkVectorRepresentationHaveSize25(t->getRightTree());
}

// Use the parsing tring in the forward propogation.
Tree* useParserForCreatingTheTree(string treeText, string sentence, Vocabulary* vocab,
    vector<vector<double>> weightScoresMatrix, vector<vector<double>> weightsMatrix, vector<double> biasSentimentMatrix, vector<double> biasWeights) {
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
    assignParsingTreeLabels(root, words, vocab, nr, weightScoresMatrix, weightsMatrix, biasSentimentMatrix, biasWeights);
    if (root->getLeftTree() == nullptr) {
        cout<<"The root of the parse tree has left child null => not binary tree."<<endl;
        exit(0);
        }
    //checkVectorRepresentationHaveSize25(root);
    return root;
    } else return constructTreeForASentence(sentence, weightsMatrix, weightScoresMatrix, vocab, biasSentimentMatrix, biasWeights);
}


// Get error for validation.
double computeErrorForTree(Tree* computedTree, Tree* targetTree, vector<vector<double>> weightScoresMatrix, vector<double> biasWeights) {
    if (computedTree == nullptr || targetTree == nullptr) return 0;
    double totalError = 0;
    double left = computeErrorForTree(computedTree->getLeftTree(), targetTree->getLeftTree(), weightScoresMatrix, biasWeights);
    double right = computeErrorForTree(computedTree->getRightTree(), targetTree->getRightTree(), weightScoresMatrix, biasWeights);

    // Compute predictions of the current node.
    vector<double> softmaxResult = softmax(addTwoVectors(biasWeights, matrixMultplicationWithVector(weightScoresMatrix, computedTree->getRootRepresentation())));


    if (targetTree->getRootRepresentation()[0] == 1) totalError += log(softmaxResult[0]);
    if (targetTree->getRootRepresentation()[1] == 1) totalError += log(softmaxResult[1]);
    totalError = totalError + left + right;

    return totalError;
}


long long getTotalNumberOfInnerNodesCorrectlyPredictted(Tree* targetTree, Tree* computedTree) {
    if (targetTree == nullptr || computedTree == nullptr) return 0;

    long long leftSide = getTotalNumberOfInnerNodesCorrectlyPredictted(targetTree->getLeftTree(), computedTree->getLeftTree());
    long long rightSide = getTotalNumberOfInnerNodesCorrectlyPredictted(targetTree->getRightTree(), computedTree->getRightTree());

    if (targetTree->getRootRepresentation()[0] == 1 && computedTree->getScore()[0] > computedTree->getScore()[1]) return 1 + leftSide + rightSide;
    if (targetTree->getRootRepresentation()[1] == 1 && computedTree->getScore()[0] < computedTree->getScore()[1]) return 1 + leftSide + rightSide;
    return 0 + leftSide + rightSide;
}
