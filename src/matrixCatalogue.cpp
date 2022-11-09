#include "global.h"

void MatrixCatalogue::insertMatrix(Matrix* matrix)
{
    logger.log("MatrixCatalogue::~insertMatrix"); 
    this->matrixs[matrix->matrixName] = matrix;
}
void MatrixCatalogue::deleteMatrix(string matrixName)
{
    logger.log("MatrixCatalogue::deleteMatrix"); 
    this->matrixs[matrixName]->unload();
    delete this->matrixs[matrixName];
    this->matrixs.erase(matrixName);
}
Matrix* MatrixCatalogue::getMatrix(string matrixName)
{
    logger.log("MatrixCatalogue::getMatrix"); 
    Matrix *matrix = this->matrixs[matrixName];
    return matrix;
}
bool MatrixCatalogue::isMatrix(string matrixName)
{
    logger.log("MatrixCatalogue::isMatrix"); 
    if (this->matrixs.count(matrixName))
        return true;
    return false;
}

void MatrixCatalogue::print()
{
    logger.log("MatrixCatalogue::print"); 
    cout << "\nRELATIONS" << endl;

    int rowCount = 0;
    for (auto rel : this->matrixs)
    {
        cout << rel.first << endl;
        rowCount++;
    }
    printRowCount(rowCount);
}

MatrixCatalogue::~MatrixCatalogue(){
    logger.log("MatrixCatalogue::~MatrixCatalogue"); 
    for(auto matrix: this->matrixs){
        matrix.second->unload();
        delete matrix.second;
    }
}
