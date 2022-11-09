# Details regarding handling matrices

## Page layout used to store matrices

The page size that we decided to choose is 4KB to accomidate decent number of elements in a page and thus resulting in lower number of disk accesses. Each element in the matrix will of type int, so when written into a page it takes up 11 bytes of space (at max, in the case of entire matrix being 1e9 and a space or newline). So number of elements in each page becomes 372 (i.e. $\frac{4096}{11}$). Page0 stores first 372 elements, Page1 stores next 372 elements and so on untill all the elements get placed in a page.

To access the elements and to print or export them, we made a vector<int>singleRow which gets filled up with all the elements of one row from pages. Once the vector gets n elements we wrote the vector onto stdout(PRINT) or corresponding csv file(EXPORT) respectively based on the type of command. We even made sure to print just first 20 rows and 20 columns in the case of PRINT.


## Compression technique used for sparse matrices and the corresponding page layout

In the case of sparse matrices we used compressed sparse row technique. In this technique we will store all the non zero elements in the form of 3 arrays.

1. V array which stores the value of nonzero elements in row major order.
2. C array stores the column index of each non zero element in row major order.
3. R array will be of size n+1 and the range (R[i],R[i+1]-1) tells that column index and value of non zero values of $i^{th}$ row are present in the index range (R[i],R[i+1]-1) of V and C arrays.

As mentioned earlier each page stores 372 elements, we fill the pages with V array elements first once this gets done, we will start filling the pages with C array elements and at last we will fill pages with R array elements. let CNT be # non-zero elements, so first $\frac{CNT}{372}$ pages will store elements of V array, next $\frac{CNT}{372}$ pages will store elements of C array and atlast $\frac{n+1}{372}$ pages will store elements of R array.
    
    

## Compression ratio as a function of the percentage of sparseness of the matrix

Let percentage of sparseness be k% and # non-zero elements becomes $(1-\frac{k}{100})*n^2$(Let it be CNT). In the case of no compression we will store $n^2$ number of elements and if we use the compression technique mentioned above we need to store CNT elements of array V, CNT elements of array C and n+1 elements of array R. So in total it stores 2*CNT + n elements. 

Ratio of compression = $2*(1-\frac{k}{100}) + \frac{1}{n}$
## Describe your in-place transpose operation and how(if) it needs to be changed or altered for sparse matrices


To transpose a matrix we traversed through all pairs of (i,j) present in the upper triangular matrix. For each (i,j) we brought the 2 pages corresponding to (i,j), (j,i) and build 2 vectors of size 372 which stores all the elements present in the pages and swaps (i,j) element present in the first vector with (j,i) element present in the second vector and finally wrote back the vectors into corresponding pages.

Element (i,j) becomes $(i*n + j)^{th}$ element of the matrix, and it belongs to the ${\frac{(i*n + j)}{372}}^{th}$ page and offset in the page becomes (i*n + j) % 372. ($0 \le i \lt n $ and $0 \le j \lt n $).

## for sparse matrices:
There is no change in the blocks of the page . The interpretation of the stored data is viewed differently if there is a transpose. There is one variable to tell us in what way we should interpret the current storage and every time we execute the transpose command we simply flip that variable (isTranspose).While printing and while extracting we use this variable to interpret correctly and execute the commands accordingly.

## Methods:
Different kinds of page layout for Sparse and non sparse matrices <br/>
Different print ,extract and transpose for sparse and non sparse matrices.<br/>
Different print , extract methods for if sparse matrix is transposed and for if sparse matrix is not transposed. 


