#include "global.h"

/**
 * @brief 
 * SYNTAX:  <NEWTABLE> <- GROUP BY <grouping_attribute> FROM <tablename> RETURN MAX | MIN | SUM | AVG (attribute)
 * 
 */
bool syntacticParseGROUP()
{
    logger.log("syntacticParseGROUP");
    if (tokenizedQuery.size() != 9)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = GROUP;
    parsedQuery.grpRelation = tokenizedQuery[6];
    parsedQuery.grpAttribute = tokenizedQuery[4];
    parsedQuery.aggregateOp = tokenizedQuery[8].substr(0, 3);
    int l = tokenizedQuery[8].length();
    parsedQuery.aggregateAttr = tokenizedQuery[8].substr(4, l - 5);
    parsedQuery.grpResult = tokenizedQuery[0];
    if (tokenizedQuery[1] != "<-" || tokenizedQuery[3] != "BY" || tokenizedQuery[5] != "FROM" || tokenizedQuery[7] != "RETURN" || tokenizedQuery[8][3] != '(' || tokenizedQuery[8][l - 1] != ')')
    {
        // cout << tokenizedQuery[8][4] << " " << tokenizedQuery[8][l - 1] << endl;
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    return true;
}

bool semanticParseGROUP()
{
    logger.log("semanticParseCROSS");
    //Both tables must exist and resultant table shouldn't
    if (tableCatalogue.isTable(parsedQuery.grpResult))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.grpRelation))
    {
        cout << "SEMANTIC ERROR: grp relation don't exist" << endl;
        return false;
    }
    if (!tableCatalogue.isColumnFromTable(parsedQuery.grpAttribute, parsedQuery.grpRelation) || !tableCatalogue.isColumnFromTable(parsedQuery.aggregateAttr, parsedQuery.grpRelation))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    if (parsedQuery.aggregateOp != "MAX" && parsedQuery.aggregateOp != "MIN" && parsedQuery.aggregateOp != "AVG" && parsedQuery.aggregateOp != "SUM")
    {
        cout << "SEMANTIC ERROR: aggregate operator doesnt exist doesnt exist" << endl;
        return false;
    }

    logger.log("semantic parsing and syntactic parsing is successfull");
    return true;
}

void executeGROUP()
{
    logger.log("executeGROUP");

    BLOCK_READ = 0;
    BLOCK_WRITE = 0;
    Table table1 = *(tableCatalogue.getTable(parsedQuery.grpRelation));

    int grpAttrIdx = -1;
    int argAttrIdx = -1;
    for (int columnCounter = 0; columnCounter < table1.columnCount; columnCounter++)
    {
        string columnName = table1.columns[columnCounter];
        if (columnName == parsedQuery.aggregateAttr)
        {
            argAttrIdx = columnCounter;
        }
        if (columnName == parsedQuery.grpAttribute)
        {
            grpAttrIdx = columnCounter;
        }

        //columns.emplace_back(columnName);
    }

    vector<string> columns;

    //Creating list of column names
    columns.push_back(parsedQuery.grpAttribute);
    columns.push_back(parsedQuery.aggregateOp + parsedQuery.aggregateAttr);

    Table *resultantTable = new Table(parsedQuery.grpResult, columns);
    tableCatalogue.insertTable(resultantTable);
    map<int, int> m;
    vector<int> grpAttrDistinctVals;

    for (int i = 0; i < table1.blockCount; i++)
    {

        Page p = bufferManager.getPage(table1.tableName, i);
        int cnt = 0;
        while (1)
        {
            vector<int> r = p.getRow(cnt);
            if (r.size() == 0)
            {
                break;
            }
            else
            {
                if (m.find(r[grpAttrIdx]) == m.end())
                {
                    grpAttrDistinctVals.push_back(r[grpAttrIdx]);
                }
                m[r[grpAttrIdx]] = 1;
            }
            cnt++;
        }
    }
    int op;
    if (parsedQuery.aggregateOp == "MAX")
        op = 0;
    if (parsedQuery.aggregateOp == "MIN")
        op = 1;
    if (parsedQuery.aggregateOp == "SUM")
        op = 2;
    if (parsedQuery.aggregateOp == "AVG")
        op = 3;

    int pageCounter = 0;
    vector<int> row(resultantTable->columnCount, 0);
    vector<vector<int>> rowsInPage(resultantTable->maxRowsPerBlock, row);
    for (int j = 0; j < grpAttrDistinctVals.size(); j++)
    {
        int val = grpAttrDistinctVals[j];
        int cnt1 = 0;
        long long int sum = 0;
        if (op == 1)
            sum = 1000000000;
        for (int i = 0; i < table1.blockCount; i++)
        {

            Page p = bufferManager.getPage(table1.tableName, i);
            int cnt = 0;
            while (1)
            {
                vector<int> r = p.getRow(cnt);
                if (r.size() == 0)
                {
                    break;
                }
                else
                {
                    if (r[grpAttrIdx] == val)
                    {
                        if (op == 2 || op == 3)
                            sum += (r[argAttrIdx]);
                        if (op == 0)
                            sum = max(sum, (long long int)r[argAttrIdx]);
                        if (op == 1)
                            sum = min(sum, (long long int)r[argAttrIdx]);
                        if (op == 3)
                            cnt1++;
                    }
                }
                cnt++;
            }
        }
        if (op == 3)
            sum = sum / cnt1;
        cnt1 = 0;
        vector<int> resRow;
        resRow.push_back(val);
        resRow.push_back(sum);
        rowsInPage[pageCounter++] = resRow;
        resultantTable->rowCount++;
        if (pageCounter == resultantTable->maxRowsPerBlock)
        {
            bufferManager.writePage(resultantTable->tableName, resultantTable->blockCount, rowsInPage, pageCounter);
            resultantTable->blockCount++;
            resultantTable->rowsPerBlockCount.emplace_back(pageCounter);
            pageCounter = 0;
        }
    }

    if (pageCounter != 0)
    {
        bufferManager.writePage(resultantTable->tableName, resultantTable->blockCount, rowsInPage, pageCounter);
        resultantTable->blockCount++;
        resultantTable->rowsPerBlockCount.emplace_back(pageCounter);
        pageCounter = 0;
    }
    cout << "#BLOCK READS: " << BLOCK_READ << " " << "#BLOCK WRITES: " << BLOCK_WRITE << endl;
    cout << "#BLOCK ACCESSES: " << BLOCK_READ + BLOCK_WRITE << endl;
    return;
}