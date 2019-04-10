#include <stdio.h>
#include <stdlib.h>

//int total_pages = 10;

//function for printing the elements of an array
void print(int array[], int s) {
	int temp=0, size = s;
	for(temp=0; temp<size; temp++)	printf("%d ", array[temp]);
	printf("\n");
}

int main() {
	printf("Enter number of pages :: ");
	int total_pages = 0;
	scanf("%d", &total_pages);

	int ref_str[total_pages], p_faults = 0, x, y, z, frames;
	
	for(x=0; x<total_pages; x++)	ref_str[x] = rand() % 10;	//generating random numbers for page reference
	printf("Randomly generated page numbers are ::\n");
	print(ref_str, total_pages);					//calling print function to print the array
	
	printf("Enter number of frames :: ");
	scanf("%d", &frames);

	int t_frames[frames];

	for(x=0; x<frames; x++)	t_frames[x] = -999;			//assinging an initial number to frames...

	for(x=0; x<total_pages; x++) {
		z = 0;
		for(y=0; y<frames; y++)
			if(ref_str[x] == t_frames[y]) {
				z++;
				p_faults--;
			}
		
		p_faults++;
		if((p_faults <= frames) && (z == 0))
   		{
     		t_frames[x] = ref_str[x];
    	}   
   		else if(z == 0)
   		{
      		t_frames[(p_faults - 1) % frames] = ref_str[x];
      	}
      	printf("\npage fault = %d\n", p_faults);
     	for(y = 0; y < frames; y++)
     	{     
       		printf("%d\t", t_frames[y]);
      	}
	}
	printf("\nTotal Page Faults:\t%d\n", p_faults);
  	return 0;
}
