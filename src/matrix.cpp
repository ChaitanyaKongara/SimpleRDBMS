#include "global.h"
#include <iostream>

/**
 * @brief Construct a new Matrix:: Matrix object
 *
 */
Matrix::Matrix()
{
    logger.log("Matrix::Matrix");
}

// /**
//  * @brief Construct a new Matrix:: Matrix object used in the case where the data
//  * file is available and LOAD command has been called. This command should be
//  * followed by calling the load function;
//  *
//  * @param MatrixName
//  */
// Matrix::Matrix(string matrixName)
// {
//     logger.log("Matrix::Matrix");
//     this->sourceFileName = "../data/" + matrixName + ".csv";
//     this->matrixName = matrixName;
// }

/**
 * @brief Construct a new Matrix:: Matrix object used when an assignment command
 * is encountered. To create the Matrix object both the Matrix name and the
 * columns the Matrix holds should be specified.
 *
 * @param MatrixName 
 * @param columns 
 */
Matrix::Matrix(string matrixName)
{
    logger.log("Matrix::Matrix");
    this->sourceFileName = "../data/" + matrixName + ".csv";
    this->matrixName = matrixName;
    this->maxElementsPerBlock = (uint)((BLOCK_SIZE * 4096) / (11));
}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates Matrix
 * statistics.
 *
 * @return true if the Matrix has been successfully loaded 
 * @return false if an error occurred 
 */
bool Matrix::load()
{
    logger.log("Matrix::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line))
    {
        fin.close();
        if (this->extractColumnSize(line))
        {
            this->checkSparseness();
            if (this->blockify())
                return true;
        }
    }
    fin.close();
    return false;
}

/**
 * @brief Function extracts column names from the header line of the .csv data
 * file. 
 *
 * @param line 
 * @return true if column names successfully extracted (i.e. no column name
 * repeats)
 * @return false otherwise
 */
bool Matrix::extractColumnSize(string firstLine)
{
    logger.log("Matrix::extractColumnNames");
    string word;
    stringstream s(firstLine);
    int columnSize = 0;
    while (getline(s, word, ','))
    {
        word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
        ++columnSize;
    }
    this->n = columnSize;
    return true;
}

/**
 * @brief Function is used to check whether the matrix is a sparse matrix or not. 
 *
 * @param line 
 * @return true if sparse
 * @return false otherwise
 */
bool Matrix::checkSparseness()
{
    logger.log("Matrix::checkSparseness");
    ifstream fin(this->sourceFileName, ios::in);

    string line, word;
    while (getline(fin, line))
    {
        stringstream s(line);
        for (int columnCounter = 0; columnCounter < this->n; ++columnCounter)
        {
            if (!getline(s, word, ','))
                return false;
            if (stoi(word))
                this->noofNonzeroElements++;
        }
    }
    fin.close();
    if ((10 * this->noofNonzeroElements) < (4 * this->n * this->n))
        return this->isSparse = true;
    return this->isSparse = false;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size. 
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Matrix::blockify()
{
    logger.log("Matrix::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    if (this->isSparse)
    {
        // Building V array
        vector<vector<int>> elementsInPage(this->maxElementsPerBlock, vector<int>(1));

        string line, word;
        int pageCounter = 0;
        while (getline(fin, line))
        {
            stringstream s(line);
            for (int columnCounter = 0; columnCounter < this->n; ++columnCounter)
            {
                if (pageCounter == this->maxElementsPerBlock)
                {
                    bufferManager.writePage(this->matrixName, this->blockCount, elementsInPage, pageCounter);
                    this->blockCount++;
                    this->elementsPerBlockCount.emplace_back(pageCounter);
                    pageCounter = 0;
                }
                if (!getline(s, word, ','))
                    return false;
                if (stoi(word))
                    elementsInPage[pageCounter][0] = stoi(word), ++pageCounter;
            }
        }
        if (pageCounter)
        {
            bufferManager.writePage(this->matrixName, this->blockCount, elementsInPage, pageCounter);
            this->blockCount++;
            this->elementsPerBlockCount.emplace_back(pageCounter);
            pageCounter = 0;
        }
        fin.close();
        fin.seekg(0);
        // Building C array
        ifstream fin1(this->sourceFileName, ios::in);
        pageCounter = 0;
        while (getline(fin1, line))
        {
            stringstream s(line);
            for (int columnCounter = 0; columnCounter < this->n; ++columnCounter)
            {
                if (pageCounter == this->maxElementsPerBlock)
                {
                    bufferManager.writePage(this->matrixName, this->blockCount, elementsInPage, pageCounter);
                    this->blockCount++;
                    this->elementsPerBlockCount.emplace_back(pageCounter);
                    pageCounter = 0;
                }
                if (!getline(s, word, ','))
                    return false;
                if (stoi(word))
                    elementsInPage[pageCounter][0] = columnCounter, ++pageCounter;
            }
        }
        if (pageCounter)
        {
            bufferManager.writePage(this->matrixName, this->blockCount, elementsInPage, pageCounter);
            this->blockCount++;
            this->elementsPerBlockCount.emplace_back(pageCounter);
            pageCounter = 0;
        }
        fin1.close();
        fin1.seekg(0);
        // Building R array
        ifstream fin2(this->sourceFileName, ios::in);
        elementsInPage[0][0] = 0;
        pageCounter = 1;
        int cnt = 0;
        while (getline(fin2, line))
        {
            stringstream s(line);
            for (int columnCounter = 0; columnCounter < this->n; ++columnCounter)
            {
                if (pageCounter == this->maxElementsPerBlock)
                {
                    bufferManager.writePage(this->matrixName, this->blockCount, elementsInPage, pageCounter);
                    this->blockCount++;
                    this->elementsPerBlockCount.emplace_back(pageCounter);
                    pageCounter = 0;
                }
                if (!getline(s, word, ','))
                    return false;
                if (stoi(word))
                    ++cnt;
            }
            elementsInPage[pageCounter++][0] = cnt;
        }
        if (pageCounter)
        {
            bufferManager.writePage(this->matrixName, this->blockCount, elementsInPage, pageCounter);
            this->blockCount++;
            this->elementsPerBlockCount.emplace_back(pageCounter);
            pageCounter = 0;
        }
        fin2.close();

        return true;
    }
    else
    {
        vector<vector<int>> elementsInPage(this->maxElementsPerBlock, vector<int>(1));

        string line, word;
        int pageCounter = 0;
        while (getline(fin, line))
        {
            stringstream s(line);
            for (int columnCounter = 0; columnCounter < this->n; ++columnCounter, ++pageCounter)
            {
                if (pageCounter == this->maxElementsPerBlock)
                {
                    bufferManager.writePage(this->matrixName, this->blockCount, elementsInPage, pageCounter);
                    this->blockCount++;
                    this->elementsPerBlockCount.emplace_back(pageCounter);
                    pageCounter = 0;
                }
                if (!getline(s, word, ','))
                    return false;
                elementsInPage[pageCounter][0] = stoi(word);
            }
        }
        if (pageCounter)
        {
            bufferManager.writePage(this->matrixName, this->blockCount, elementsInPage, pageCounter);
            this->blockCount++;
            this->elementsPerBlockCount.emplace_back(pageCounter);
            pageCounter = 0;
        }
        fin.close();
        return true;
    }
}

void Matrix::clearPageInMM()
{
    for (int i = 0; i < this->blockCount; i++)
    {
        Page page = bufferManager.getPage(this->matrixName, i);
        if (page.rows[0].size() != 0)
        {
            page.rows[0].clear();
            return;
        }
    }
}

void Matrix::print()
{
    logger.log("Matrix::print");
    // uint count = min((long long)PRINT_COUNT, this->rowCount);

    //print headings
    // this->writeRow(this->columns, cout);

    //Cursor cursor(this->MatrixName, 0);
    if (!this->isSparse)
    {
        vector<int> row;
        int pageIndex = 0;

        vector<int> singleRow;
        for (int rowCounter = 0; rowCounter < 20 && rowCounter < this->n;)
        {

            Page page = bufferManager.getPage(this->matrixName, pageIndex);
            pageIndex++;

            vector<vector<int>> pageElements = page.rows;
            // if(pageElements[0].size() == 0){
            //     this->clearPageInMM();
            //     page.readPage();
            // }

            int indexinpage = 0;
            while (indexinpage < this->elementsPerBlockCount[pageIndex - 1])
            {
                singleRow.push_back(pageElements[indexinpage][0]);
                indexinpage++;

                if (this->n > 20 && singleRow.size() == 20)
                {
                    this->writeRow(singleRow, cout);
                }
                if (singleRow.size() == this->n)
                {
                    if (this->n <= 20)
                        this->writeRow(singleRow, cout);
                    singleRow.clear();
                    rowCounter++;
                }
                if (rowCounter == 20)
                {
                    break;
                }
            }
        }
    }
    else
    {
        int off1 = (this->noofNonzeroElements + this->maxElementsPerBlock - 1) / this->maxElementsPerBlock;
        int off2 = 2 * off1;
        //cout<<off1<<" "<<off2<<endl;
        if (!this->isTranspose)
        {
            int count = 0;
            for (int i = 0; i < this->n && i < 20; i++)
            {
                //  cout<<i<<endl;
                int prevCol = -1;

                int off3 = (off2) + (i + 1 + this->maxElementsPerBlock - 1) / this->maxElementsPerBlock;
                int off4 = (off2) + (i + 2 + this->maxElementsPerBlock - 1) / this->maxElementsPerBlock;
                //cout<<i<<" "<<off3<<" "<<off4<<endl;
                Page page1 = bufferManager.getPage(this->matrixName, off3 - 1);
                Page page2 = bufferManager.getPage(this->matrixName, off4 - 1);
                int pageOf1 = (i + 1) % this->maxElementsPerBlock;
                int pageOf2 = (i + 2) % this->maxElementsPerBlock;
                if (pageOf2 == 0)
                {

                    pageOf2 = this->maxElementsPerBlock;
                }
                if (pageOf1 == 0)
                {

                    pageOf1 = this->maxElementsPerBlock;
                }
                int a1 = page1.rows[pageOf1 - 1][0];
                int a2 = page2.rows[pageOf2 - 1][0];
                int numEleInCurRow = a2 - a1;

                //  cout<<i<<" "<<off3<<" "<<off4<<" "<<a1<<" "<<a2<<endl;

                for (int k = 0; k < numEleInCurRow; k++)
                {
                    int pg = off1 + (count + k + 1 + this->maxElementsPerBlock - 1) / this->maxElementsPerBlock;
                    int pageOf1 = (count + k + 1) % this->maxElementsPerBlock;

                    if (pageOf1 == 0)
                    {
                        pageOf1 = this->maxElementsPerBlock;
                    }
                    Page page3 = bufferManager.getPage(this->matrixName, pg - 1);
                    int colval = page3.rows[pageOf1 - 1][0];
                    int pg1 = (k + 1 + this->maxElementsPerBlock - 1) / this->maxElementsPerBlock;
                    Page page4 = bufferManager.getPage(this->matrixName, pg1 - 1);
                    int elem = page4.rows[pageOf1 - 1][0];
                    // cout<<pg1<<" "<<pageOf1<<endl;
                    int z;
                    for (z = prevCol + 1; z < colval && z < 19; z++)
                    {
                        if (z != 0)
                            cout << ',';
                        cout << 0;
                    }
                    if (z == 19 && colval != 19)
                    {
                        cout << ',';
                        cout << '0';
                        cout << endl;
                        break;
                    }
                    else
                    {
                        if (colval != 0)
                            cout << ',';
                        cout << elem;
                        if (colval == 19 || colval + 1 == this->n)
                            cout << endl;
                        prevCol = colval;
                    }
                }
                int fl = 0;
                while (prevCol < 20 && prevCol + 1 < this->n)
                {
                    if (prevCol != -1)
                        cout << ',';
                    cout << 0;
                    fl = 1;
                    prevCol++;
                }
                if (fl)
                    cout << endl;
                count = a2;
            }
        }
        else
        {
            cout << "TRANSPOSE" << endl;
            for (int row = 0; row < this->n && row < 20; row++)

            {

                int pageIdx = 0;

                int maxPages = off1 + ((this->noofNonzeroElements + this->maxElementsPerBlock - 1) / this->maxElementsPerBlock);
                int firstPage = 2 * off1;
                int count = 0;
                int flag = 1;
                // cout<<row<<" "<<firstPage<<" "<<maxPages<<endl;
                int prevCol = -1;
                for (int j = firstPage; j <= maxPages; j++)
                {
                    Page page1 = bufferManager.getPage(this->matrixName, j - 1);
                    for (int z = 0; z < this->elementsPerBlockCount[j]; z++)
                    {
                        count++;

                        //  cout<<page1.rows[z][0]<<endl;
                        if (page1.rows[z][0] == row)
                        {
                            for (int i = 0; i < this->n; i++)
                            {

                                int off3 = (off2) + (i + 1 + this->maxElementsPerBlock - 1) / this->maxElementsPerBlock;
                                int off4 = (off2) + (i + 2 + this->maxElementsPerBlock - 1) / this->maxElementsPerBlock;

                                Page page1 = bufferManager.getPage(this->matrixName, off3 - 1);
                                Page page2 = bufferManager.getPage(this->matrixName, off4 - 1);
                                int pageOf1 = (i + 1) % this->maxElementsPerBlock;
                                int pageOf2 = (i + 2) % this->maxElementsPerBlock;
                                if (pageOf2 == 0)
                                {

                                    pageOf2 = this->maxElementsPerBlock;
                                }
                                if (pageOf1 == 0)
                                {

                                    pageOf1 = this->maxElementsPerBlock;
                                }
                                int a1 = page1.rows[pageOf1 - 1][0];
                                int a2 = page2.rows[pageOf2 - 1][0];
                                int numEleInCurRow = a2 - a1;
                                // cout<<a1<<" "<<a2<<" "<<i<<endl;
                                if (a1 <= count && count <= a2)
                                {
                                    // cout << "row"
                                    //      << " " << row << "PREV " << prevCol << endl;
                                    int colval = i;
                                    //cout<<"fafff"<<" "<<colval <<" "<<row<<" "<<"FAFAF"<<endl;
                                    int b;
                                    for (b = prevCol + 1; b < colval && b < 19; b++)
                                    {
                                        if (b != 0)
                                            cout << ',';
                                        cout << 0;
                                    }
                                    int pageOf1 = (count) % this->maxElementsPerBlock;

                                    if (pageOf1 == 0)
                                    {
                                        pageOf1 = this->maxElementsPerBlock;
                                    }
                                    int pg1 = (count + this->maxElementsPerBlock - 1) / this->maxElementsPerBlock;
                                    Page page4 = bufferManager.getPage(this->matrixName, pg1 - 1);
                                    int elem = page4.rows[pageOf1 - 1][0];
                                    if (b == 18 && colval != 19)
                                    {
                                        cout << ',';
                                        cout << '0';
                                        cout << endl;
                                        flag = 0;
                                        break;
                                    }
                                    else
                                    {
                                        if (colval != 0)
                                            cout << ",";
                                        cout << elem;
                                        prevCol = colval;

                                        if (b == 19 || b + 1 == this->n)
                                        {
                                            cout << endl;
                                            flag = 0;
                                        }
                                        break;

                                        // cout << endl;
                                    }
                                }
                            }
                        }
                        if (flag == 0)
                        {
                            break;
                        }
                    }
                    //cout<<count<<endl;

                    for (int g = prevCol + 1; g < this->n && g < 20; g++)
                    {
                        if (g != 0)
                            cout << ',';
                        cout << '0';
                    }
                    if (flag)
                        cout << endl;
                }
            }
        }
    }
}

void Matrix::Transpose()
{
    if (!this->isSparse)
    {
        for (int i = 1; i <= this->n; i++)
        {
            for (int j = i + 1; j <= this->n; j++)
            {
                bufferManager.clear();
                int ele1 = (i - 1) * (this->n) + j;
                int pageId1 = (ele1) / this->maxElementsPerBlock;
                int pageOf1 = (ele1) % this->maxElementsPerBlock;

                int ele2 = (j - 1) * (this->n) + i;
                int pageId2 = (ele2) / this->maxElementsPerBlock;
                int pageOf2 = (ele2) % this->maxElementsPerBlock;

                if (pageOf2 == 0)
                {
                    pageId2--;
                    pageOf2 = this->maxElementsPerBlock;
                }
                if (pageOf1 == 0)
                {
                    pageId1--;
                    pageOf1 = this->maxElementsPerBlock;
                }
                if (pageId2 != pageId1)
                {

                    Page page1 = bufferManager.getPage(this->matrixName, pageId1);
                    Page page2 = bufferManager.getPage(this->matrixName, pageId2);

                    int a1 = page1.rows[pageOf1 - 1][0];
                    int a2 = page2.rows[pageOf2 - 1][0];
                    page1.rows[pageOf1 - 1][0] = a2;
                    page2.rows[pageOf2 - 1][0] = a1;

                    page1.writePage();
                    page2.writePage();
                }
                else
                {
                    Page page1 = bufferManager.getPage(this->matrixName, pageId1);
                    int a1 = page1.rows[pageOf1 - 1][0];
                    int a2 = page1.rows[pageOf2 - 1][0];
                    page1.rows[pageOf1 - 1][0] = a2;
                    page1.rows[pageOf2 - 1][0] = a1;
                    page1.writePage();
                }
            }
        }
    }
    else
    {
        this->isTranspose = 1 - this->isTranspose;
    }
}

/**
 * @brief This function returns one row of the Matrix using the cursor object. It
 * returns an empty row is all rows have been read.
 *
 * @param cursor 
 * @return vector<int> 
 */
void Matrix::getNextPage(Cursor *cursor)
{
    logger.log("Matrix::getNext");

    if (cursor->pageIndex < this->blockCount - 1)
    {
        cursor->nextPage(cursor->pageIndex + 1);
    }
}

/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Matrix::makePermanent()
{
    logger.log("Matrix::makePermanent");
    // if(!this->isPermanent())
    //     bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->matrixName + ".csv";
    ofstream fout(newSourceFile, ios::out);

    //print headings
    //this->writeRow(1, fout);

    //  Cursor cursor(this->matrixName, 0);
    // vector<int> row;
    // for (int rowCounter = 0; rowCounter < this->n; rowCounter++)
    // {
    //     row = cursor.getNext();
    //     this->writeRow(row, fout);
    // }
    vector<int> row;
    int pageIndex = 0;

    vector<int> singleRow;
    if (!this->isSparse)
    {
        for (int rowCounter = 0; rowCounter < this->n;)
        {

            Page page = bufferManager.getPage(this->matrixName, pageIndex);
            pageIndex++;

            vector<vector<int>> pageElements = page.rows;
            // if(pageElements[0].size() == 0){
            //     this->clearPageInMM();
            //     page.readPage();
            // }

            int indexinpage = 0;
            while (indexinpage < this->elementsPerBlockCount[pageIndex - 1])
            {
                // singleRow.push_back(pageElements[indexinpage][0]);
                fout << pageElements[indexinpage][0];
                indexinpage++;

                if (singleRow.size() == this->n)
                {
                    fout << endl;
                    // this->writeRow(singleRow, fout);
                    // singleRow.clear();
                    rowCounter++;
                }
            }
        }
    }

    else
    {
        int off1 = (this->noofNonzeroElements + this->maxElementsPerBlock - 1) / this->maxElementsPerBlock;
        int off2 = 2 * off1;
        //cout<<off1<<" "<<off2<<endl;
        if (!this->isTranspose)
        {
            int count = 0;
            for (int i = 0; i < this->n; i++)
            {
                //  cout<<i<<endl;
                int prevCol = -1;

                int off3 = (off2) + (i + 1 + this->maxElementsPerBlock - 1) / this->maxElementsPerBlock;
                int off4 = (off2) + (i + 2 + this->maxElementsPerBlock - 1) / this->maxElementsPerBlock;
                //cout<<i<<" "<<off3<<" "<<off4<<endl;
                Page page1 = bufferManager.getPage(this->matrixName, off3 - 1);
                Page page2 = bufferManager.getPage(this->matrixName, off4 - 1);
                int pageOf1 = (i + 1) % this->maxElementsPerBlock;
                int pageOf2 = (i + 2) % this->maxElementsPerBlock;
                if (pageOf2 == 0)
                {

                    pageOf2 = this->maxElementsPerBlock;
                }
                if (pageOf1 == 0)
                {

                    pageOf1 = this->maxElementsPerBlock;
                }
                int a1 = page1.rows[pageOf1 - 1][0];
                int a2 = page2.rows[pageOf2 - 1][0];
                int numEleInCurRow = a2 - a1;

                //  cout<<i<<" "<<off3<<" "<<off4<<" "<<a1<<" "<<a2<<endl;

                for (int k = 0; k < numEleInCurRow; k++)
                {
                    int pg = off1 + (count + k + 1 + this->maxElementsPerBlock - 1) / this->maxElementsPerBlock;
                    int pageOf1 = (count + k + 1) % this->maxElementsPerBlock;

                    if (pageOf1 == 0)
                    {
                        pageOf1 = this->maxElementsPerBlock;
                    }
                    Page page3 = bufferManager.getPage(this->matrixName, pg - 1);
                    int colval = page3.rows[pageOf1 - 1][0];
                    int pg1 = (k + 1 + this->maxElementsPerBlock - 1) / this->maxElementsPerBlock;
                    Page page4 = bufferManager.getPage(this->matrixName, pg1 - 1);
                    int elem = page4.rows[pageOf1 - 1][0];
                    // cout<<pg1<<" "<<pageOf1<<endl;
                    int z;
                    for (z = prevCol + 1; z < colval; z++)
                    {
                        if (z != 0)
                            fout << ',';
                        fout << 0;
                    }

                    if (colval != 0)
                        fout << ',';
                    fout << elem;
                    if (colval + 1 == this->n)
                        fout << endl;
                    prevCol = colval;
                }
                int fl = 0;
                while (prevCol + 1 < this->n)
                {
                    if (prevCol != -1)
                        fout << ',';
                    fout << 0;
                    fl = 1;
                    prevCol++;
                }
                if (fl)
                    fout << endl;
                count = a2;
            }
        }
        else
        {
          
            for (int row = 0; row < this->n ; row++)

            {

                int pageIdx = 0;

                int maxPages = off1 + ((this->noofNonzeroElements + this->maxElementsPerBlock - 1) / this->maxElementsPerBlock);
                int firstPage = 2 * off1;
                int count = 0;
                int flag = 1;
                // cout<<row<<" "<<firstPage<<" "<<maxPages<<endl;
                int prevCol = -1;
                for (int j = firstPage; j <= maxPages; j++)
                {
                    Page page1 = bufferManager.getPage(this->matrixName, j - 1);
                    for (int z = 0; z < this->elementsPerBlockCount[j]; z++)
                    {
                        count++;

                        //  cout<<page1.rows[z][0]<<endl;
                        if (page1.rows[z][0] == row)
                        {
                            for (int i = 0; i < this->n; i++)
                            {

                                int off3 = (off2) + (i + 1 + this->maxElementsPerBlock - 1) / this->maxElementsPerBlock;
                                int off4 = (off2) + (i + 2 + this->maxElementsPerBlock - 1) / this->maxElementsPerBlock;

                                Page page1 = bufferManager.getPage(this->matrixName, off3 - 1);
                                Page page2 = bufferManager.getPage(this->matrixName, off4 - 1);
                                int pageOf1 = (i + 1) % this->maxElementsPerBlock;
                                int pageOf2 = (i + 2) % this->maxElementsPerBlock;
                                if (pageOf2 == 0)
                                {

                                    pageOf2 = this->maxElementsPerBlock;
                                }
                                if (pageOf1 == 0)
                                {

                                    pageOf1 = this->maxElementsPerBlock;
                                }
                                int a1 = page1.rows[pageOf1 - 1][0];
                                int a2 = page2.rows[pageOf2 - 1][0];
                                int numEleInCurRow = a2 - a1;
                                // cout<<a1<<" "<<a2<<" "<<i<<endl;
                                if (a1 <= count && count <= a2)
                                {
                                    // cout << "row"
                                    //      << " " << row << "PREV " << prevCol << endl;
                                    int colval = i;
                                    //cout<<"fafff"<<" "<<colval <<" "<<row<<" "<<"FAFAF"<<endl;
                                    int b;
                                    for (b = prevCol + 1; b < colval ; b++)
                                    {
                                        if (b != 0)
                                            fout << ',';
                                        fout << 0;
                                    }
                                    int pageOf1 = (count) % this->maxElementsPerBlock;

                                    if (pageOf1 == 0)
                                    {
                                        pageOf1 = this->maxElementsPerBlock;
                                    }
                                    int pg1 = (count + this->maxElementsPerBlock - 1) / this->maxElementsPerBlock;
                                    Page page4 = bufferManager.getPage(this->matrixName, pg1 - 1);
                                    int elem = page4.rows[pageOf1 - 1][0];
                                  
                                 
                                        if (colval != 0)
                                            fout << ",";
                                        fout << elem;
                                        prevCol = colval;

                                        if (b + 1 == this->n)
                                        {
                                            fout << endl;
                                            flag = 0;
                                        }
                                        break;
                                        

                                        // cout << endl;
                                    
                                }
                            }
                        }
                        if (flag == 0)
                        {
                            break;
                        }
                    }
                    //cout<<count<<endl;

                    for (int g = prevCol + 1; g < this->n ; g++)
                    {    
                        if (g != 0)
                            fout << ',';
                        fout << '0';
                    }
                    if (flag)
                        fout << endl;
                }
            }
        }
    }

    fout.close();
}

/**
 * @brief Function to check if Matrix is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Matrix::isPermanent()
{
    logger.log("Matrix::isPermanent");
    if (this->sourceFileName == "../data/" + this->matrixName + ".csv")
        return true;
    return false;
}

/**
 * @brief The unload function removes the Matrix from the database by deleting
 * all temporary files created as part of this Matrix
 *
 */
void Matrix::unload()
{
    logger.log("Matrix::~unload");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        bufferManager.deleteFile(this->matrixName, pageCounter);
    if (!isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
}

/**
 * @brief Function that returns a cursor that reads rows from this Matrix
 * 
 * @return Cursor 
 */
Cursor Matrix::getCursor()
{
    logger.log("Matrix::getCursor");
    Cursor cursor(this->matrixName, 0);
    return cursor;
}
