#include "method.h"

int sort_ave(int a[],int n)//ц╟ещеепР
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
