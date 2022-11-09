#include"semanticParser.h"

void executeCommand();

void executeCLEAR();
void executeCROSS();
void executeDISTINCT();
void executeEXPORT();
void executeINDEX();
void executeJOIN();
void executeLIST();
void executeLOAD();
void executeLOADMATRIX();
void executePRINT();
void executePROJECTION();
void executeRENAME();
void executeSELECTION();
void executeSORT();
void executeSOURCE();
void executePRINTMATRIX();
void executeEXPORTMATRIX();
void executeTRANSPOSE();
void executeGROUP();

bool evaluateBinOp(int value1, int value2, BinaryOperator binaryOperator);
void printRowCount(int rowCount);