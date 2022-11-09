#include<bits/stdc++.h>
using namespace std;
int main(){
	srand(time(0));
	freopen("student.csv","w",stdout);
	cout << "ID, Age\n";
	for(int i=0;i<10000;i++){
		cout << rand()%50 << ", " << rand()%20 <<endl; 
	}	
	freopen("course.csv","w",stdout);
	cout << "Course_ID, Roll_Number\n";
	for(int i=0;i<10000;i++){
		cout << rand()%20 << ", " << rand()%50 <<endl; 
	freopen("bigstudent.csv","w",stdout);
	cout << "ID, Course, Class, Grade\n";
	for(int i=0;i<5000;i++){
		cout << rand()%50 << ", " << rand()%30 << ", " << rand()%20 << ", " << rand()%80 << endl; 
	}
	return 0;
}
