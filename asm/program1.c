
//#include <stdio.h>

void copy_array(int *a, int *b, int size) {
	int i;
	
	for (i = 0; i < size; i++){
		b[i] = a[i];
	}
}

int bubble_sort(int *array, int size) {
	int bubbled;
	int i, j;
	int temp_store;
	int passes;

	passes = 0;
	for (i = 0; i < size; i++){
		bubbled = 0;
		for (j = 0; j < (size - 1 - i); j++){
			if (array[j] > array[j+1]){
				temp_store = array[j];
				array[j] = array[j+1];
				array[j+1] = temp_store;
				bubbled = 1;
			}
			passes++;
		}
		if (bubbled == 0) i = size;  	// List is sorted, force early termination
	}
	return passes;
}

int insertion_sort(int *array, int size) {
	int i, j;
	int temp_store;
	int passes;

	passes = 0;
	for (i = 1; i < size; i++){
		j = i;
		while ((j > 0) && (array[j] < array[j-1])){
			temp_store = array[j];
			array[j] = array[j-1];
			array[j-1] = temp_store;

			passes++;

			j--;
		}
	}
	return passes;
}

int check_result(int *a, int *b, int size) {
int c;
int d;
int i;
int x;

c = 0;
for (i=0;i<size;i++){
	x = a[i]^b[i];
	if (x) {
		c = 1;
	}
	d += a[i];
}

return c;
     
}

int main(){
  int c;
  int d;
  int match;
  int j[250];
  int k[250];

  copy_array(&j[0], &k[0], 250);
  c = bubble_sort(&j[0], 250);
  d = insertion_sort(&k[0], 250);
  match = check_result(&j[0], &k[0], 250);

  if (c > d) match = match;
	else match = match + 1000000;

  return match;
}
