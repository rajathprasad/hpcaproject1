#include <iostream>

using namespace std;

int arr[8];
int mru_tag;

void LRU_hit(int hit_block)
{
	int temp = arr[hit_block];
	for(int j = hit_block ; j > 0; j--)
	{
		arr[j] = arr[j-1];
	}	
	arr[0] = temp;
	//valid[cache_index][0] = true;	
	cout<<"hit"<<endl;
}


void LRU_miss(int tag)
{
	for(int j = 8 -1; j > 0; j--)
	{
		arr[j] = arr[j-1];
		//valid[cache_index][j+1] = valid[cache_index][j];
		//dirty[cache_index][j+1] = dirty[cache_index][j];
	}
	arr[0] = tag;
	//valid[cache_index][0] = true;
	cout<<"miss"<<endl;
}

void NMRU_miss(int tag)
{
	cout<<"mru is "<<mru_tag<<", last element is "<<arr[7]<<endl;
	if (arr[7] == mru_tag)
	{
		for(int j = 8 - 2; j > 0; j--)
		{
			arr[j] = arr[j-1];
			
		}
		arr[0] = tag;
	}
	else  /*kick out last in array */
	{
		for(int j = 8 - 1; j > 0; j--)
		{
			arr[j] = arr[j-1];
		}
		arr[0] = tag;
	}

}


int main()
{
	
	int tag;
	while(1)
	{
		cout<<"enter number"<<endl;
		cin>> tag;
		bool hit = false;
		int hit_block;
		for(int i =0; i <8; i++)
		{
			if(arr[i] == tag )
			{
				hit = true;
				hit_block = i;
				break;
				}
		}

		if(hit)
		{
			mru_tag = tag;
		
		}
		else
		{
			NMRU_miss(tag);
			mru_tag = tag;
		}
		for(int i =0; i <8; i++)
			cout<<arr[i]<<" ";
		cout<<endl;
	}
	return 0;
}


