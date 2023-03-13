#include "method.h"

/**
@brief	冒泡排序求均值
@Note		函数中舍去了一个最大值和一个最小值
@param	数据数组，数据长度
@retval	None
*/
int sort_ave(int a[],int n)//冒泡排序
{
	int i,j,temp;
	for(i=0;i<n-1;i++){
		for(j=0;j<n-1-i;j++){
			if(a[j]>a[j+1]){
				temp=a[j];
				a[j]=a[j+1];
				a[j+1]=temp;
			}
 
		}
	}
	int along = (sizeof(a) / sizeof(a[0]));
	int average_val = 0;
	for(int i = 1;i<along ;i++)
	{
		average_val += a[i];
	}
	average_val = average_val / (along-2);
	return average_val;
}
