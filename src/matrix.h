#ifndef MATRIX_H
#define MATRIX_H
#include "cursor.h"

// enum IndexingStrategy
// {
//     BTREE,
//     HASH,
//     NOTHING
// };

/**
 * @brief The Matrix class holds all information related to a loaded matrices. It
 * also implements methods that interact with the parsers, executors, cursors
 * and the buffer manager. There are typically 2 ways a table object gets
 * created through the course of the workflow - the first is by using the LOAD
 * command and the second is to use assignment statements (SELECT, PROJECT,
 * JOIN, SORT, CROSS and DISTINCT). 
 *
 */
class Matrix
{

public:
    string sourceFileName = "";
    string matrixName = "";
    uint n;
    uint blockCount = 0;
    uint noofNonzeroElements = 0;
    uint maxElementsPerBlock = 0;
    vector<uint> elementsPerBlockCount;
    bool indexed = false;
    IndexingStrategy indexingStrategy = NOTHING;
    bool isSparse = false;
    bool isTranspose = false;
    bool extractColumnSize(string firstLine);
    bool blockify();
    Matrix();
    Matrix(string matrixName);
    Matrix(string matrixName, vector<string> columns);
    bool load();
    void print();
    void makePermanent();
    bool isPermanent();
    void getNextPage(Cursor *cursor);
    Cursor getCursor();
    void unload();
    void clearPageInMM();
    void Transpose();
    bool checkSparseness();
    /**
 * @brief Static function that takes a vector of valued and prints them out in a
 * comma seperated format.
 *
 * @tparam T current usaages include int and string
 * @param row 
 */
template <typename T>
void writeRow(vector<T> row, ostream &fout)
{
    logger.log("Table::printRow");
    for (int columnCounter = 0; columnCounter < row.size(); columnCounter++)
    {
        if (columnCounter != 0)
            fout << ", ";
        fout << row[columnCounter];
    }
    fout << endl;
}

/**
 * @brief Static function that takes a vector of valued and prints them out in a
 * comma seperated format.
 *
 * @tparam T current usaages include int and string
 * @param row 
 */
template <typename T>
void writeRow(vector<T> row)
{
    logger.log("Table::printRow");
    ofstream fout(this->sourceFileName, ios::app);
    this->writeRow(row, fout);
    fout.close();
}
};

#endif