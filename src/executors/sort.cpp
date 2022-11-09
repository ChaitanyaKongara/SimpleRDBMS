#include "global.h"
/**
 * @brief File contains method to process SORT commands.
 * 
 * syntax:
 * R <- SORT relation_name BY column_name IN sorting_order
 * 
 * sorting_order = ASC | DESC 
 */
bool syntacticParseSORT()
{
    logger.log("syntacticParseSORT");
    if ((tokenizedQuery.size() != 8 && tokenizedQuery.size() != 10) || tokenizedQuery[4] != "BY" || tokenizedQuery[6] != "IN" || tokenizedQuery[1] != "<-")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = SORT;
    parsedQuery.sortResultRelationName = tokenizedQuery[0];
    parsedQuery.sortRelationName = tokenizedQuery[3];
    parsedQuery.sortColumnName = tokenizedQuery[5];
    string sortingStrategy = tokenizedQuery[7];
    if (sortingStrategy == "ASC")
        parsedQuery.sortingStrategy = ASC;
    else if (sortingStrategy == "DESC")
        parsedQuery.sortingStrategy = DESC;
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    if (tokenizedQuery.size() == 10)
    {
        if (tokenizedQuery[8] != "BUFFER")
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
    }

    if (tokenizedQuery.size() == 10)
    {
        
        int f = 1;
        for (int i = 0; i < tokenizedQuery[9].length(); i++)
        {
            if (tokenizedQuery[9][i] >= 48 && tokenizedQuery[9][i] <= 57)
            {
            }
            else
            {
                f = 0;
            }
        }
        if (f == 1)
            parsedQuery.sortBuffSize = stoi(tokenizedQuery[9]);
        else
        {
            cout << "BUFFER SIZE IS NOT ALPHA NUMERICAL" << endl;
            return false;
        }
        
    }
    else
    {
        parsedQuery.sortBuffSize = 10;
    }
    return true;
}

bool semanticParseSORT()
{
    logger.log("semanticParseSORT");

    if (tableCatalogue.isTable(parsedQuery.sortResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.sortRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.sortColumnName, parsedQuery.sortRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }

    return true;
}
int columnIdx;
bool comp(const vector<int> &a, const vector<int> &b)
{
    if (parsedQuery.sortingStrategy == ASC)
        return a[columnIdx] < b[columnIdx];
    else
        return a[columnIdx] > b[columnIdx];
}

vector<int> getCrctRow(vector<pair<Page, int>>& p, vector<pair<Table, int>>& np, int colIdx)
{

    int mini = 1000000000;
    int maxi = -1;

    for (auto x : p)
    {
        if (x.second == -1)
        {

            continue;
        }
        auto row = x.first.getRow(x.second);
        if (row.size() == 0)
        {
            continue;
        }

        if (parsedQuery.sortingStrategy == ASC)
        {
            mini = min(row[colIdx], mini);
        }
        else
        {
            maxi = max(row[colIdx], maxi);
        }
    }

    int i = -1;
    for (auto x : p)
    {
        i++;
        if (x.second == -1)
        {
            continue;
        }

        auto row = x.first.getRow(x.second);

        if (parsedQuery.sortingStrategy == ASC && row[colIdx] == mini || parsedQuery.sortingStrategy == DESC && row[colIdx] == maxi)
        {
            vector<int> res = row;
          //  cout<<np[i].second<<" "<<x.second<<" "<<x.first.rowCount<<"  table  "<<i<<endl;

            if (x.second == x.first.rowCount - 1)
            {  // cout<<"a"<<endl;
                if (np[i].second != -1 && !(np[i].first.blockCount == np[i].second))
                {
                   
                 //   cout<<"b"<<endl;
                    x.first = bufferManager.getPage(np[i].first.tableName, np[i].second);
                   // cout<<"c"<<endl;
                    x.second = 0;
                    if (np[i].second == np[i].first.blockCount - 1)
                    {
                        np[i].second = -1;
                    }
                    else
                    {
                        np[i].second++;
                    }
                    if(np[i].second == np[i].first.blockCount){
                        np[i].second = -1;

                    }
                    p[i].first = x.first;
                    p[i].second = x.second;
                }
                else
                {
                    x.second = -1;
                    p[i].second  = -1;
                }
            }
            else
            {
                x.second++;
                p[i].second = x.second;
            }
            return row;
        }
    }
    return {};
}

void executeSORT()
{
    logger.log("executeSORT");
    BLOCK_READ = 0;
    BLOCK_WRITE = 0;
    Table table = *(tableCatalogue.getTable(parsedQuery.sortRelationName));
    vector<string> columns;
    columnIdx = -1;
    for (int i = 0; i < table.columns.size(); i++)
    {
        if (table.columns[i] == parsedQuery.sortColumnName)
        {
            columnIdx = i;
        }
        columns.emplace_back(table.columns[i]);
    }
    Table *tempTable = new Table("temporary", columns);
    Table *resultantTable = new Table(parsedQuery.sortResultRelationName, columns);
    tableCatalogue.insertTable(resultantTable);
    tableCatalogue.insertTable(tempTable);
    int pageCounter = 0;
    int numBlocks = parsedQuery.sortBuffSize;
    vector<int> row(resultantTable->columnCount, 0);
    vector<vector<int>> rowsInPage(resultantTable->maxRowsPerBlock, row);
    vector<vector<int>> rowsInBuffer((long long)parsedQuery.sortBuffSize * (long long)resultantTable->maxRowsPerBlock, row);
    logger.log("sorting phase is not done");

    for (int i = 0; i < table.blockCount; i += numBlocks)
    {
        vector<Page> p;
        for (int j = i; j <= min(i + numBlocks - 1, (int)table.blockCount - 1); j++)
        {
            p.push_back(bufferManager.getPage(table.tableName, j)); //crazy
        }
        int counter = 0;
        for (int k = 0; k < p.size(); k++)
        {
            int cnt = 0;
            while (1)
            {
                vector<int> r = p[k].getRow(cnt);
                if (r.size() == 0)
                {
                    break;
                }
                else
                {
                    rowsInBuffer[counter++] = r;
                }
                cnt++;
            }
        }
        sort(rowsInBuffer.begin(), rowsInBuffer.begin() + counter, comp);
        for (int j = 0; j < counter; j++)
        {
            int k = j;
            while (j < counter && j < (k + tempTable->maxRowsPerBlock))
            {
                rowsInPage[j - k] = rowsInBuffer[j];
                tempTable->rowCount++;
                j++;
            }
            bufferManager.writePage(tempTable->tableName, tempTable->blockCount, rowsInPage, j - k);
            tempTable->blockCount++;
            tempTable->rowsPerBlockCount.emplace_back(j - k);
            j--;
        }
    }

    //creating temp files after sortphase
    logger.log("sorting phase is done");
    cout<<ceil((double)(table.blockCount) / (numBlocks))<<endl;
    int jj = 0;
    for (int i = 0; i < ceil((double)(table.blockCount) / (numBlocks)); i = i + 1)
    {
      
        Table *tempTable = new Table("f" +to_string(i) + "_0M" + parsedQuery.sortResultRelationName, columns);
        tableCatalogue.insertTable(tempTable);
      //  cout<<i<<endl;
        int tj = jj;
        for ( ; jj < min((int)table.blockCount,tj + numBlocks);jj++)
        {
           // cout<<jj<<endl;
            int m = i / numBlocks;
            Page p = bufferManager.getPage("temporary", jj);
            bufferManager.writePage(tempTable->tableName, tempTable->blockCount, p.rows, p.rowCount);
            tempTable->blockCount++;
            tempTable->rowCount += p.rowCount;
            tempTable->rowsPerBlockCount.emplace_back(p.rowCount);
        }
    }
    logger.log("file creation for sort phase is done");
    //return;

    //merge phase
    int numPass = ceil((float)log2(table.blockCount) / log2(numBlocks - 1));
    cout << numPass << endl;
    if(numPass == 0){
          Page p = bufferManager.getPage("temporary",0);
            bufferManager.writePage(resultantTable->tableName, resultantTable->blockCount, p.rows, p.rowCount);
            resultantTable->blockCount++;
            resultantTable->rowCount += p.rowCount;
            resultantTable->rowsPerBlockCount.emplace_back(p.rowCount);


            tableCatalogue.deleteTable("temporary");
          return;
          

    }
    int j = ceil((float)table.blockCount / (numBlocks));
    for (int i = 1; i <= numPass; i++)
    {
        int q = ceil((float)j / (numBlocks - 1));
        cout << "pass number: " << i << "   numsubfiles present   " << j << " num subwill form"
             << " " << q << endl;
        int n = 1;
        while (n <= q)
        {
            vector<pair<Page, int>> p;
            vector<pair<Table, int>> np;

            for (int k = (n - 1) * (numBlocks - 1); k < min(n * (numBlocks - 1), j); k++)
            {
                cout<<k<<endl;
                string relName = "f" +to_string(k) + "_" + to_string(i - 1) + "M" + parsedQuery.sortResultRelationName;
                cout << relName << endl;
                Table temp = *(tableCatalogue.getTable(relName));
                // cout<<"fsasf"<<endl;
                Page pg = bufferManager.getPage(temp.tableName, 0);
                // cout<<"fafa"<<endl;

                p.push_back({pg, 0});
                np.push_back({temp, 1});
            }
            cout<<p.size()<<" "<<np.size()<<endl;
            
            Table *tempTable;
            if (i == numPass)
            {
                tempTable = new Table(parsedQuery.sortResultRelationName, columns);
            }
            else
            {
                tempTable = new Table("f" + to_string(n - 1) + "_" + to_string(i) + "M" + parsedQuery.sortResultRelationName, columns);
            }
            tableCatalogue.insertTable(tempTable);

            vector<int> row(tempTable->columnCount, 1);
            vector<vector<int>> rowsInPage(tempTable->maxRowsPerBlock, row);
            int pageCounter = 0;
            cout<<tempTable->blockCount<<endl;
            int cnt = 0;
            while (1)
            {
                //cout<<cnt<<endl;
                vector<int> row = getCrctRow(p, np, columnIdx);
                if (row.size() == 0)
                {
                    break;
                }
                else
                {
                    cnt++;
                  //  cout<<cnt<<endl;
                    rowsInPage[pageCounter++] = row;
                    tempTable->rowCount++;
                    if (pageCounter == tempTable->maxRowsPerBlock)
                    {
                        bufferManager.writePage(tempTable->tableName, tempTable->blockCount, rowsInPage, pageCounter);
                        tempTable->blockCount++;
                        tempTable->rowsPerBlockCount.emplace_back(pageCounter);
                        pageCounter = 0;
                    }
                }
            }
            cout<<tempTable->blockCount<<" "<<pageCounter<<endl;
            if (pageCounter != 0)
            {
                bufferManager.writePage(tempTable->tableName, tempTable->blockCount, rowsInPage, pageCounter);
                tempTable->blockCount++;
                tempTable->rowsPerBlockCount.emplace_back(pageCounter);
                pageCounter = 0;
            }
            n++;
          
        }
      //  break;
        j = q;
    }

    
      tableCatalogue.deleteTable("temporary");

    return;
}