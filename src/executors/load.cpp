#include "global.h"
/**
 * @brief 
 * SYNTAX: LOAD relation_name
 */
bool syntacticParseLOAD()
{
    logger.log("syntacticParseLOAD");
    if (tokenizedQuery.size() > 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    else if(tokenizedQuery.size() == 3){
        if(tokenizedQuery[1] != "MATRIX"){
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
        else{
            parsedQuery.queryType = LOADMATRIX;
            parsedQuery.loadRelationName = tokenizedQuery[2];
            return true;
        }
    }
    parsedQuery.queryType = LOAD;
    parsedQuery.loadRelationName = tokenizedQuery[1];
    return true;
}

bool semanticParseLOAD()
{
    logger.log("semanticParseLOAD");
    if (tableCatalogue.isTable(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Relation already exists" << endl;
        return false;
    }

    if (!isFileExists(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
        return false;
    }
    return true;
}

bool semanticParseLOADMATRIX()
{
    logger.log("semanticParseLOADMATRIX");
    if(matrixCatalogue.isMatrix(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Matrix already exists" << endl;
        return false;
    }

    if (!isFileExists(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeLOAD()
{
    logger.log("executeLOAD");

    Table *table = new Table(parsedQuery.loadRelationName);
    if (table->load())
    {
        tableCatalogue.insertTable(table);
        cout << "Loaded Table. Column Count: " << table->columnCount << " Row Count: " << table->rowCount << endl;
    }
    return;
}

void executeLOADMATRIX()
{
    logger.log("executeLOADMATRIX");

    Matrix *matrix = new Matrix(parsedQuery.loadRelationName);
    if (matrix->load())
    {
        matrixCatalogue.insertMatrix(matrix);
        cout << "Loaded Matrix. Column Count: " << matrix->n << " Row Count: " << matrix->n << endl;
    }
    return;
}