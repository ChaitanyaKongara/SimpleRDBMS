#include "global.h"
/**
 * @brief 
 * SYNTAX: PRINT relation_name
 */
bool syntacticParsePRINT()
{
    logger.log("syntacticParsePRINT");
    if (tokenizedQuery.size() >  3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    else if(tokenizedQuery.size() <= 2){
        parsedQuery.queryType = PRINT;
        parsedQuery.printRelationName = tokenizedQuery[1];
        return true;
    }
    else{
        parsedQuery.queryType = PRINTMATRIX;
        parsedQuery.printRelationName = tokenizedQuery[2];
        return true;
    }


}

bool semanticParsePRINT()
{
    logger.log("semanticParsePRINT");
    if (!tableCatalogue.isTable(parsedQuery.printRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    return true;
}
bool semanticParsePRINTMATRIX()
{
    logger.log("semanticParsePRINT");
    if (!matrixCatalogue.isMatrix(parsedQuery.printRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    return true;
}


void executePRINT()
{
    logger.log("executePRINT");
    Table* table = tableCatalogue.getTable(parsedQuery.printRelationName);
    table->print();
    return;
}


void executePRINTMATRIX()
{
    logger.log("executePRINTMATRIX");
    Matrix* matrix= matrixCatalogue.getMatrix(parsedQuery.printRelationName);
    matrix->print();
    return;
}