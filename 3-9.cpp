#include<iostream>//头文件 
#include<cstdlib>
using namespace std;//命名空间 
int main(){
	int x,y,z1,z2;
	x=7;y=8;
	z1=y-(x++);
	z2=y-(++x);
	cout<<z1<<" "<<z2<<endl;
	system("pause");
	return 0;
} 
