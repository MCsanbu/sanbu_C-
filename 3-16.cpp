#include<iostream>//头文件 
#include<cstdlib>
using namespace std;//命名空间 
int main(){
	int a,x;
	cin>>x;
	a=x/4;
	switch (x%4)
	{
		case 0:cout<<a<<"支4元笔";break;
		case 1:cout<<(a-1)<<"支4元笔+1支5元笔";break;
		case 2:cout<<(a-1)<<"支4元笔+1支6元笔";break;
		case 3:cout<<(a-2)<<"支4元笔+1支6元笔+1支五元笔";break;
		default:cout<<"input error"
	}
	return 0; 
}
