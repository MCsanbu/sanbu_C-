#include<iostream>//ͷ�ļ� 
#include<cstdlib>
using namespace std;//�����ռ� 
int main(){
	int a,x;
	cin>>x;
	a=x/4;
	switch (x%4)
	{
		case 0:cout<<a<<"֧4Ԫ��";break;
		case 1:cout<<(a-1)<<"֧4Ԫ��+1֧5Ԫ��";break;
		case 2:cout<<(a-1)<<"֧4Ԫ��+1֧6Ԫ��";break;
		case 3:cout<<(a-2)<<"֧4Ԫ��+1֧6Ԫ��+1֧��Ԫ��";break;
		default:cout<<"input error"
	}
	return 0; 
}
