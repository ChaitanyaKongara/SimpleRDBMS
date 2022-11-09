# Simple RDBMS

An integer-only, read-only Relational Database Management System.

## Compilation Instructions

<br>

```cd``` into the SimpleRDBMS directory
```
cd SimpleRDBMS
```
```cd``` into the source directory (called ```src```)
```
cd src
```
To compile
```
make clean && make
```

## To run

Post compilation, an executable names ```server``` will be created in the ```src``` directory
```
./server
```

## To understand how to write queries go through [Grammar](./Grammar.md).


---

### Implemented the RDBMS from [SimpleRA](https://github.com/SimpleRA/SimpleRA) public Template.

<br>

Added Join, Sort, GroupBy functionalities and also handles Matrices and compresses in the case of a sparse matrix. 