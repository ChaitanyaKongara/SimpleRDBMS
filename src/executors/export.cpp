#include "global.h"

/**
 * @brief 
 * SYNTAX: EXPORT <relation_name> 
 */

bool syntacticParseEXPORT()
{
    logger.log("syntacticParseEXPORT");
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
            parsedQuery.queryType = EXPORTMATRIX;
            parsedQuery.exportRelationName = tokenizedQuery[2];
            return true;
        }
    }
    parsedQuery.queryType = EXPORT;
    parsedQuery.exportRelationName = tokenizedQuery[1];
    return true;
}

bool semanticParseEXPORT()
{
    logger.log("semanticParseEXPORT");
    //Table should exist
    if (tableCatalogue.isTable(parsedQuery.exportRelationName))
        return true;
    cout << "SEMANTIC ERROR: No such relation exists" << endl;
    return false;
}
bool semanticParseEXPORTMATRIX()
{
    logger.log("semanticParseEXPORT");
    //Table should exist
    if (matrixCatalogue.isMatrix(parsedQuery.exportRelationName)){
     
        return true;
    }
    cout << "SEMANTIC ERROR: No such relation exists" << endl;
    return false;
}

void executeEXPORT()
{
    logger.log("executeEXPORT");
    Table* table = tableCatalogue.getTable(parsedQuery.exportRelationName);
    table->makePermanent();
    return;
}

void executeEXPORTMATRIX()
{
    logger.log("executeEXPORTMATRIX");
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.exportRelationName);
    matrix->makePermanent();
    return;
}