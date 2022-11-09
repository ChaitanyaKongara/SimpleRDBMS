#include "global.h"
/**
 * @brief 
 * SYNTAX: R <- JOIN relation_name1, relation_name2 ON column_name1 bin_op column_name2 
 *                           ||
 *                            V
 *                       (normal join)
 * SYNTAX: R <- JOIN USING NESTED relation_name1, relation_name2 ON column1 bin_op column2 BUFFER buffersize
 *                           ||
 *                            V
 *                           (nested join)
 * SYNTAX: R <- JOIN USING PARTHASH relation_name1, relation_name2 ON column_name1 bin_op column_name2 BUFFER buffer_size
 *                           ||
 *                            V
 *                       (hash join)
 * 
 * 
 */

bool syntacticParseJOIN()
{
    logger.log("syntacticParseJOIN");
    if ((tokenizedQuery.size() != 9 && tokenizedQuery.size() != 13) || (tokenizedQuery[5] != "ON" && tokenizedQuery[7] != "ON"))
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    parsedQuery.queryType = JOIN;
    if (tokenizedQuery.size() == 9)
    {
        parsedQuery.joinResultRelationName = tokenizedQuery[0];
        parsedQuery.joinFirstRelationName = tokenizedQuery[3];
        parsedQuery.joinSecondRelationName = tokenizedQuery[4];
        parsedQuery.joinFirstColumnName = tokenizedQuery[6];
        parsedQuery.joinSecondColumnName = tokenizedQuery[8];

        string binaryOperator = tokenizedQuery[7];
        if (binaryOperator == "<"){
            parsedQuery.joinBinaryOperator = LESS_THAN;
            parsedQuery.joinOp = 0;
        }
        else if (binaryOperator == ">"){
            parsedQuery.joinBinaryOperator = GREATER_THAN;
            parsedQuery.joinOp = 1;

        }
        else if (binaryOperator == ">=" || binaryOperator == "=>"){
            parsedQuery.joinBinaryOperator = GEQ;
            parsedQuery.joinOp =  2;
        }
        else if (binaryOperator == "<=" || binaryOperator == "=<"){
            parsedQuery.joinBinaryOperator = LEQ;
            parsedQuery.joinOp = 3;
        }
        else if (binaryOperator == "=="){
            parsedQuery.joinBinaryOperator = EQUAL;
            parsedQuery.joinOp = 4;
        }
        else if (binaryOperator == "!="){
            parsedQuery.joinBinaryOperator = NOT_EQUAL;
            parsedQuery.joinOp = 5;
        }
        else
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
        return true;
    }
    else if (tokenizedQuery[4] == "NESTED")
    {
        parsedQuery.joinType = "NESTED";
        //parsedQuery.buffAvailable = stoi(tokenizedQuery[12]);
    }
    else if (tokenizedQuery[4] == "PARTHASH")
    {
        parsedQuery.joinType = "PARTHASH";
        //parsedQuery.buffAvailable = stoi(tokenizedQuery[12]);
    }
    if(tokenizedQuery.size() == 13 &&  (tokenizedQuery[3] != "USING" || tokenizedQuery[1] != "<-" || tokenizedQuery[11] != "BUFFER")){
        cout<<"Syntax Error"<<endl;
        return false;


    }
    if(tokenizedQuery.size() == 13){
        int f = 1;
        for(int i = 0 ; i < tokenizedQuery[12].length(); i++){
            if(tokenizedQuery[12][i] >= 48 && tokenizedQuery[12][i] <= 57){

            }
            else{
                f = 0;
            } 


        }
        if(f == 1)
            parsedQuery.buffAvailable = stoi(tokenizedQuery[12]);
        else{
            cout<<"BUFFER SIZE IS NOT ALPHA NUMERICAL"<<endl;
            return false;

        }
    }
    
    
    parsedQuery.joinResultRelationName = tokenizedQuery[0];
    parsedQuery.joinFirstRelationName = tokenizedQuery[5];
    parsedQuery.joinSecondRelationName = tokenizedQuery[6];
    parsedQuery.joinFirstColumnName = tokenizedQuery[8];
    parsedQuery.joinSecondColumnName = tokenizedQuery[10];

    string binaryOperator = tokenizedQuery[9];
   

    if (binaryOperator == "<"){
        parsedQuery.joinBinaryOperator = LESS_THAN;
        parsedQuery.joinOp = 0;
    }
    else if (binaryOperator == ">"){
        parsedQuery.joinBinaryOperator = GREATER_THAN;
        parsedQuery.joinOp = 1;

    }
    else if (binaryOperator == ">=" || binaryOperator == "=>"){
        parsedQuery.joinBinaryOperator = GEQ;
        parsedQuery.joinOp =  2;
    }
    else if (binaryOperator == "<=" || binaryOperator == "=<"){
        parsedQuery.joinBinaryOperator = LEQ;
        parsedQuery.joinOp = 3;
    }
    else if (binaryOperator == "=="){
        parsedQuery.joinBinaryOperator = EQUAL;
        parsedQuery.joinOp = 4;
    }
    else if (binaryOperator == "!="){
        parsedQuery.joinBinaryOperator = NOT_EQUAL;
        parsedQuery.joinOp = 5;
    }
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    return true;
}

bool semanticParseJOIN()
{
    logger.log("semanticParseJOIN");

    if (tableCatalogue.isTable(parsedQuery.joinResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.joinFirstRelationName) || !tableCatalogue.isTable(parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.joinFirstColumnName, parsedQuery.joinFirstRelationName) || !tableCatalogue.isColumnFromTable(parsedQuery.joinSecondColumnName, parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    return true;
}

int hashFunction(int key, int M){
    float c = (sqrt(5)-1)/2;
    float fraction = (key*c) - (int)(key*c);
    // cout << M*fraction << endl;
    return (M * fraction);
}
bool get(vector<int> r1 , vector<int> r2, int counter1, int counter2){
    if(r1[counter1] < r2[counter2]  && parsedQuery.joinOp == 0){
        return true;

    }
     if(r1[counter1] > r2[counter2]  && parsedQuery.joinOp == 1){
        return true;

    }
     if(r1[counter1] >= r2[counter2]  && parsedQuery.joinOp == 2){
        return true;

    }
     if(r1[counter1] <= r2[counter2]  && parsedQuery.joinOp == 3){
        return true;

    }
     if(r1[counter1] == r2[counter2]  && parsedQuery.joinOp == 4){
        return true;

    }
     if(r1[counter1] != r2[counter2]  && parsedQuery.joinOp == 5){
        return true;

    }
    return false;
    



}
void executeJOIN()
{
    logger.log("executeJOIN");
    BLOCK_READ = 0;
    BLOCK_WRITE = 0;
    if (parsedQuery.joinType == "NESTED")
    {
        logger.log("nestedexecuteJOIN");
        Table table1 = *(tableCatalogue.getTable(parsedQuery.joinFirstRelationName));
        Table table2 = *(tableCatalogue.getTable(parsedQuery.joinSecondRelationName));

        vector<string> columns;

        //If both tables are the same i.e. CROSS a a, then names are indexed as a1 and a2
        if (table1.tableName == table2.tableName)
        {
            parsedQuery.joinFirstRelationName += "1";
            parsedQuery.joinSecondRelationName += "2";
        }

        //Creating list of column names
        int counterTable1 = -1;
        int counterTable2 = -1;
        for (int columnCounter = 0; columnCounter < table1.columnCount; columnCounter++)
        {
            string columnName = table1.columns[columnCounter];
            if (columnName == parsedQuery.joinFirstColumnName)
            {
                counterTable1 = columnCounter;
            }
            if (table2.isColumn(columnName))
            {
                columnName = parsedQuery.joinFirstRelationName + "_" + columnName;
            }
            columns.emplace_back(columnName);
        }

        for (int columnCounter = 0; columnCounter < table2.columnCount; columnCounter++)
        {
            string columnName = table2.columns[columnCounter];
            if (columnName == parsedQuery.joinSecondColumnName)
            {
                counterTable2 = columnCounter;
            }
            if (table1.isColumn(columnName))
            {
                columnName = parsedQuery.joinSecondRelationName + "_" + columnName;
            }
            columns.emplace_back(columnName);
        }

        Table *resultantTable = new Table(parsedQuery.joinResultRelationName, columns);
        tableCatalogue.insertTable(resultantTable);
        int numBlocks = parsedQuery.buffAvailable;
        numBlocks -= 2;
        vector<int> row(resultantTable->columnCount, 0);
        vector<vector<int>> rowsInPage(resultantTable->maxRowsPerBlock, row);
        int pageCounter = 0;
        for (int i = 0; i < table1.blockCount; i = i + numBlocks)
        {
            vector<Page> p;
            for (int j = i; j <= min(i + numBlocks - 1, (int)table1.blockCount - 1); j++)
            {
                p.push_back(bufferManager.getPage(table1.tableName, j)); //crazy
            }
            for (int j = 0; j < table2.blockCount; j++)
            {
                Page s = bufferManager.getPage(table2.tableName, j);
                for (int k = 0; k < p.size(); k++)
                {
                    int cnt1 = 0;
                    while (1)
                    {
                        vector<int> r1 = p[k].getRow(cnt1);
                        if (r1.size() == 0)
                            break;
                        else
                        {
                            int cnt2 = 0;
                            while (1)
                            {
                                vector<int> r2 = s.getRow(cnt2);
                                if (r2.size() == 0)
                                {
                                    // cnt1 = 1000000000;
                                    break;
                                }
                                else
                                {
                                    if (get(r1,r2,counterTable1,counterTable2) )
                                    {
                                        vector<int> resultantRow;
                                        resultantRow.reserve(resultantTable->columnCount);
                                        resultantRow = r1;
                                        resultantRow.insert(resultantRow.end(), r2.begin(), r2.end());
                                        rowsInPage[pageCounter++] = resultantRow;
                                        resultantTable->rowCount++;
                                        if(pageCounter == resultantTable->maxRowsPerBlock){
                                            bufferManager.writePage(resultantTable->tableName, resultantTable->blockCount, rowsInPage, pageCounter);
                                            resultantTable->blockCount++;
                                            resultantTable->rowsPerBlockCount.emplace_back(pageCounter);
                                            pageCounter = 0;
                                        }
                                    }
                                }
                                cnt2++;
                            }
                        }

                        cnt1++;
                    }
                }
            }
        }
        if(pageCounter){
            bufferManager.writePage(resultantTable->tableName, resultantTable->blockCount, rowsInPage, pageCounter);
            resultantTable->blockCount++;
            resultantTable->rowsPerBlockCount.emplace_back(pageCounter);
            pageCounter = 0;
        }
    }
    else if(parsedQuery.joinType == "PARTHASH"){
        logger.log("executingHASHJOIN");
        Table table1 = *(tableCatalogue.getTable(parsedQuery.joinFirstRelationName));
        Table table2 = *(tableCatalogue.getTable(parsedQuery.joinSecondRelationName));
        string columnR = parsedQuery.joinFirstColumnName;
        string columnS = parsedQuery.joinSecondColumnName;
        vector<string> columns,columns1,columns2;

        //If both tables are the same i.e. CROSS a a, then names are indexed as a1 and a2
        if (table1.tableName == table2.tableName)
        {
            parsedQuery.joinFirstRelationName += "1";
            parsedQuery.joinSecondRelationName += "2";
        }

        //Creating list of column names
        int counterTable1 = -1;
        int counterTable2 = -1;
        for (int columnCounter = 0; columnCounter < table1.columnCount; columnCounter++)
        {
            string columnName = table1.columns[columnCounter];
            if (columnName == parsedQuery.joinFirstColumnName)
            {
                counterTable1 = columnCounter;
            }
            if (table2.isColumn(columnName))
            {
                columnName = parsedQuery.joinFirstRelationName + "_" + columnName;
            }
            columns1.emplace_back(columnName);
            columns.emplace_back(columnName);
        }

        for (int columnCounter = 0; columnCounter < table2.columnCount; columnCounter++)
        {
            string columnName = table2.columns[columnCounter];
            if (columnName == parsedQuery.joinSecondColumnName)
            {
                counterTable2 = columnCounter;
            }
            if (table1.isColumn(columnName))
            {
                columnName = parsedQuery.joinSecondRelationName + "_" + columnName;
            }
            columns2.emplace_back(columnName);
            columns.emplace_back(columnName);
        }

        Table* resultantTable = new Table(parsedQuery.joinResultRelationName, columns);
        tableCatalogue.insertTable(resultantTable);
        // Build Partitions of first relation
        
        int M = parsedQuery.buffAvailable - 1;
        int partitionIndex;
        vector<Table *> partitionsR(M);
        for(int i=0; i<M ; i++){
            partitionsR[i] = new Table(parsedQuery.joinFirstRelationName + "_partition" + to_string(i),columns1);
            tableCatalogue.insertTable(partitionsR[i]);
        }

        vector<int> row(table1.columnCount, 0);
        vector<vector<int>> rowsInPage(table1.maxRowsPerBlock, row);
        vector<vector<vector<int> > > allPartitions(M,rowsInPage);
        vector<int> pageCounters(M,0);
        for(int i=0;i<table1.blockCount;i++){
            Page page = bufferManager.getPage(table1.tableName, i);
            for(int row = 0; row<table1.rowsPerBlockCount[i]; row++){
                vector<int> cols;
                cols = page.getRow(row);
                for(int col = 0; col < cols.size(); col++){
                    if(table1.columns[col] == columnR){
                        partitionIndex = hashFunction(cols[col],M);
                        for (int columnCounter = 0; columnCounter < partitionsR[partitionIndex]->columnCount; columnCounter++)
                        {
                            allPartitions[partitionIndex][pageCounters[partitionIndex]][columnCounter] = cols[columnCounter];
                        }
                        pageCounters[partitionIndex]++;
                        partitionsR[partitionIndex]->rowCount++;
                        if (pageCounters[partitionIndex] == partitionsR[partitionIndex]->maxRowsPerBlock)
                        {
                            bufferManager.writePage(partitionsR[partitionIndex]->tableName, partitionsR[partitionIndex]->blockCount, allPartitions[partitionIndex], pageCounters[partitionIndex]);
                            partitionsR[partitionIndex]->blockCount++;
                            partitionsR[partitionIndex]->rowsPerBlockCount.emplace_back(pageCounters[partitionIndex]);
                            pageCounters[partitionIndex] = 0;
                        }
                    }
                }
            }
        }
        for(int partitionIndex = 0; partitionIndex < pageCounters.size(); partitionIndex++){
            if (pageCounters[partitionIndex])
            {
                bufferManager.writePage(partitionsR[partitionIndex]->tableName, partitionsR[partitionIndex]->blockCount, allPartitions[partitionIndex], pageCounters[partitionIndex]);
                partitionsR[partitionIndex]->blockCount++;
                partitionsR[partitionIndex]->rowsPerBlockCount.emplace_back(pageCounters[partitionIndex]);
                pageCounters[partitionIndex] = 0;
            }
        }
        logger.log("builtPartitionsRJOIN");
        // Build Partitions of second relation
        vector<Table *> partitionsS(M);
        for(int i=0; i<M ; i++){
            partitionsS[i] = new Table(parsedQuery.joinSecondRelationName + "_partition" + to_string(i),columns2);
            tableCatalogue.insertTable(partitionsS[i]);
        }
        row.clear();
        row.shrink_to_fit();
        rowsInPage.clear();
        rowsInPage.shrink_to_fit();
        allPartitions.clear();
        allPartitions.shrink_to_fit();
        row.resize(table2.columnCount, 0);
        rowsInPage.resize(table2.maxRowsPerBlock, row);
        allPartitions.resize(M,rowsInPage);
        pageCounters.resize(M,0);
        for(int i=0;i<table2.blockCount;i++){
            Page page = bufferManager.getPage(table2.tableName, i);
            for(int row = 0; row<table2.rowsPerBlockCount[i]; row++){
                vector<int> cols;
                cols = page.getRow(row);
                for(int col = 0; col < cols.size(); col++){
                    if(table2.columns[col] == columnS){
                        partitionIndex = hashFunction(cols[col],M);
                        for (int columnCounter = 0; columnCounter < partitionsS[partitionIndex]->columnCount; columnCounter++)
                        {
                            allPartitions[partitionIndex][pageCounters[partitionIndex]][columnCounter] = cols[columnCounter];
                        }
                        pageCounters[partitionIndex]++;
                        partitionsS[partitionIndex]->rowCount++;
                        if (pageCounters[partitionIndex] == partitionsS[partitionIndex]->maxRowsPerBlock)
                        {
                            bufferManager.writePage(partitionsS[partitionIndex]->tableName, partitionsS[partitionIndex]->blockCount, allPartitions[partitionIndex], pageCounters[partitionIndex]);
                            partitionsS[partitionIndex]->blockCount++;
                            partitionsS[partitionIndex]->rowsPerBlockCount.emplace_back(pageCounters[partitionIndex]);
                            pageCounters[partitionIndex] = 0;
                        }
                    }
                }
            }
        }
        for(int partitionIndex = 0; partitionIndex < pageCounters.size(); partitionIndex++){
            if (pageCounters[partitionIndex])
            {
                bufferManager.writePage(partitionsS[partitionIndex]->tableName, partitionsS[partitionIndex]->blockCount, allPartitions[partitionIndex], pageCounters[partitionIndex]);
                partitionsS[partitionIndex]->blockCount++;
                partitionsS[partitionIndex]->rowsPerBlockCount.emplace_back(pageCounters[partitionIndex]);
                pageCounters[partitionIndex] = 0;
            }
        }
        logger.log("builtPartitionsSJOIN");

        // nested join between corresponding partitions of R and S
        int numBlocks = parsedQuery.buffAvailable;
        numBlocks-=2;
        row.clear();
        row.shrink_to_fit();
        rowsInPage.clear();
        rowsInPage.shrink_to_fit();
        allPartitions.clear();
        allPartitions.shrink_to_fit();
        row.resize(resultantTable->columnCount, 0);
        rowsInPage.resize(resultantTable->maxRowsPerBlock, row);
        int pageCounter = 0;
        for(int partitionIndex = 0; partitionIndex < M; partitionIndex++){
            
            logger.log("JOIN partitions");
            for (int i = 0; i < partitionsR[partitionIndex]->blockCount; i = i + numBlocks)
            {
                vector<Page> p;
                for (int j = i; j <= min(i + numBlocks - 1, (int)partitionsR[partitionIndex]->blockCount - 1); j++)
                {
                    p.push_back(bufferManager.getPage(partitionsR[partitionIndex]->tableName, j)); //crazy
                }
                for (int j = 0; j < partitionsS[partitionIndex]->blockCount; j++)
                {
                    Page s = bufferManager.getPage(partitionsS[partitionIndex]->tableName, j);
                    for (int k = 0; k < p.size(); k++)
                    {
                        int cnt1 = 0;
                        while (1)
                        {
                            vector<int> r1 = p[k].getRow(cnt1);
                            if (r1.size() == 0)
                                break;
                            else
                            {
                                int cnt2 = 0;
                                while (1)
                                {
                                    vector<int> r2 = s.getRow(cnt2);
                                    if (r2.size() == 0)
                                    {
                                        // cnt1 = 1000000000;
                                        break;
                                    }
                                    else
                                    {
                                        if (get(r1,r2,counterTable1,counterTable2))
                                        {
                                            vector<int> resultantRow;
                                            resultantRow.reserve(resultantTable->columnCount);
                                            resultantRow = r1;
                                            resultantRow.insert(resultantRow.end(), r2.begin(), r2.end());
                                            rowsInPage[pageCounter++] = resultantRow;
                                            resultantTable->rowCount++;
                                            if(pageCounter == resultantTable->maxRowsPerBlock){
                                                bufferManager.writePage(resultantTable->tableName, resultantTable->blockCount, rowsInPage, pageCounter);
                                                resultantTable->blockCount++;
                                                resultantTable->rowsPerBlockCount.emplace_back(pageCounter);
                                                pageCounter = 0;
                                            }
                                        }
                                    }
                                    cnt2++;
                                }
                            }

                            cnt1++;
                        }
                    }
                }
            }
        }
        if(pageCounter){
            bufferManager.writePage(resultantTable->tableName, resultantTable->blockCount, rowsInPage, pageCounter);
            resultantTable->blockCount++;
            resultantTable->rowsPerBlockCount.emplace_back(pageCounter);
            pageCounter = 0;
        }
        logger.log("Deleteing partitions");
        for(int i=0;i<M;i++){
            tableCatalogue.deleteTable(partitionsR[i]->tableName);
            tableCatalogue.deleteTable(partitionsS[i]->tableName);
        }
    }
    cout << "#BLOCK READS: " << BLOCK_READ << " " << "#BLOCK WRITES: " << BLOCK_WRITE << endl;
    cout << "#BLOCK ACCESSES: " << BLOCK_READ + BLOCK_WRITE << endl; 
    return;
}
