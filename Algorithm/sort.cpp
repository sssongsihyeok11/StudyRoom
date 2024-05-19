#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

void bubble_sort(int *A, int size){
    for(int i=0; i<size-1; i++){
        for(int j = i+1; j<size; j++){
            if(A[i] > A[j]){
                int temp = A[j];
                A[j] = A[i];
                A[i] = temp;
            }
        }
    }
}

void insert_sort(int *A, int size) {
    for (int i = 1; i < size; i++) {
        int key = A[i];
        int idx = i - 1;
        while (idx >= 0 && A[idx] > key) {
            A[idx + 1] = A[idx];
            idx--;
        }
        A[idx + 1] = key;
    }
}

void selection_sort(int *A, int size){
    for(int i=0; i<size; i++){
        int min = A[i];
        int minidx = i;
        for(int j=i+1; j<size; j++){
            if(min > A[j]) {
                min = A[j];
                minidx = j;
            }
        }
        int temp = A[i];
        A[i] = A[minidx];
        A[minidx] = temp;
    } 
}

// merge sort 다시 구현 해볼 것
int temp_arr[9];

void merge(int *arr, int start, int middle, int end){
	int i = start;
	int j = middle + 1;
	int k = start;
	
	//비교하여 데이터정렬 및 삽입
	while(i<=middle && j<=end){
		if(arr[i]<=arr[j]) temp_arr[k] = arr[i++];
		else temp_arr[k] = arr[j++];
		k++;
	}
	
	//남은 데이터 삽입
	if(i>middle){
		for(int t=j;t<=end;++t){
			temp_arr[k] = arr[t];
			++k;
		}
	}
	else{
		for(int t=i;t<=middle;++t){
			temp_arr[k] = arr[t];
			++k;
		}
	}
	
	//임시 저장용 배열에 저장된 값을 원래 배열에 넣어줌
	for(int t=start;t<=end;++t){
		arr[t] = temp_arr[t];
	}
}


void mergeSort(int *arr, int start, int end){
	//크기가 1 일대 까지 호출, 1단위 까지 쪼갬
	if(start < end){
		int middle = (start + end) / 2;
		mergeSort(arr, start, middle);
		mergeSort(arr, middle+1, end);
		//다시 병합
		merge(arr, start, middle, end);
	}
}

//if A = [0,1,2,3,4,5] -> start = 0, end = 5 
void quick_sort(int *A, int start, int end){
    if(start >= end) return;

    int pivot = A[end];
    int left = start;
    int right = end-1;
    while(left <=right){
        while(left<=right && A[left] <=pivot) left++;
        while(left<=right && A[right] >=pivot) right--;

        if(left<right){
            int temp = A[right];
            A[right] = A[left];
            A[left] = temp;
        }
    }
    int temp = A[end];
    A[end] = A[left];
    A[left] = temp;

    quick_sort(A,start,(left-1));
    quick_sort(A,(left+1), end);

}

void shell_sort(int *A, int gap, int sz){
    for(int i=0; i<gap; i++){
        for(int j=0;  ;j++){
            if(j*gap + i < sz){
                int v = A[j*gap + i];
                int l = j*gap + i;
                while(l > i && A[l-gap] > v){
                    A[l] = A[l - gap];
                    l = l-gap;
                }
                A[l] = v;
            }
            else break;
        }
    }
    if(gap>1) {
        gap = gap/2;
        shell_sort(A,gap,sz);
    }
}
void heapify(int *A, int k, int n) {
    int left = 2 * k;
    int right = 2 * k + 1;
    int smaller;
    
    if (right <= n) {
        if (A[left] < A[right]) {
            smaller = left;
        } else {
            smaller = right;
        }
    } else if (left <= n) {
        smaller = left;
    } else {
        return;
    }
    
    
    if (A[smaller] < A[k]) {
        int temp = A[smaller];
        A[smaller] = A[k];
        A[k] = temp;
        heapify(A, smaller, n);
    }
}

void build_min_heap(int *A, int n) {
    for (int i = n / 2; i > 0; i--) {
        heapify(A, i, n);
    }
}

void heap_sort(int *A, int n) {
    build_min_heap(A, n);
    
    for (int i = n; i > 1; i--) {
        int temp = A[i];
        A[i] = A[1];
        A[1] = temp;
        
        heapify(A, 1, i - 1);
    }
}

void counting_sort(int *A, int n, int range){
    int *count=  new int(range+1);
    int *B = new int(n);
    for(int i=0; i<=range; i++) count[i] = 0;
    for(int i=0; i<n; i++) count[A[i]] +=1;
    for(int i=1; i<=range; i++) count[i]+=count[i-1];

    for(int i=n-1; i>=0; i--){
        B[count[A[i]]-1] = A[i];
    }
    for(int i=0; i<n; i++) A[i] = B[i];  
}

int Find_Max_Digit(int *A, int n){
    int m = *std::max_element(A, A+n);
    
    int comp =1;
    int cnt = 0;
    while(comp <=m){
        comp*=10;
        cnt++;
    }

    return cnt;
}

int digit_number(int a, int k){
    int comp=1;
    int cnt = 1;
    int temp =a;
    int result =temp;

    if(k==1) return a%10;

    while(cnt != k){
        result = temp/10;
        cnt++;
        temp = result;
    }
    return result%10;
}

void radix_sort(int *A, int n){
    int digit = Find_Max_Digit(A,n);
    //std::cout << digit;
 
    std::vector<std::queue< int>> Q(10);
    
    for(int k =1; k<=digit; k++){
        for(int i=0; i<n; i++){
            int dg = digit_number(A[i],k);
            Q[dg].push(A[i]);
        }

        int p=0;
        for(int i=0; i<=9; i++){
            while(!Q[i].empty()){
                A[p] = Q[i].front();
                Q[i].pop();
                p = p+1;
            }
        }
    }
}

void bitonicMerge(int *A, int start, int sz, int direction){
    if(sz >1){
        int k =sz/2;
        for(int i = start; i<start + k; i++){
            if(direction ==(A[i]>A[i+k])){
                int temp = A[i];
                A[i] = A[i+k];
                A[i+k] = temp;
            }
        }
        bitonicMerge(A, start, k, direction);
        bitonicMerge(A, start+k, k, direction);
    }
}

void bitonicSourcetree(int *A, int start, int sz, int direction){
    if(sz >1){
        int k = sz/2;
        bitonicSourcetree(A, start, k, 1);
        bitonicSourcetree(A,start+k, k, 0);
        bitonicMerge(A, start, sz, direction);
    }
}

void bitonic_sort(int *A, int sz, int direction){
    bitonicSourcetree(A,0,sz,direction);
}

void print_sort_result(int*A, int sz){
    for(int i=0; i<sz; i++){
        std::cout << A[i] << " " ;
    }

    std::cout <<"\n\n";
}

void test_bubble_sort(){
    int A[7] = {13,7,9,3,11,6,12};
    int sz = 7;
    
    std:: cout << "before bubble sorting \n" << std::endl;
    print_sort_result(A,sz);

    std:: cout << "after bubble sorting \n" << std::endl;
    bubble_sort(A,sz);
    print_sort_result(A,sz);
}

void test_insert_sort(){
    int A[7] = {13,7,9,3,11,6,12};
    int sz = 7;
    
    std:: cout << "before insert sorting \n" << std::endl;
    print_sort_result(A,sz);

    std:: cout << "after insert sorting \n" << std::endl;
    insert_sort(A,sz);
    print_sort_result(A,sz);
}

void test_selection_sort(){
    int A[7] = {13,7,9,3,11,6,12};
    int sz = 7;
    
    std:: cout << "before selection sorting \n" << std::endl;
    print_sort_result(A,sz);

    std:: cout << "after selection sorting \n" << std::endl;
    selection_sort(A,sz);
    print_sort_result(A,sz);
}

void test_merge_sort(){
    int A[7] = {13,7,9,3,11,6,12};
    int sz = 7;
    
    std:: cout << "before merge sorting \n" << std::endl;
    print_sort_result(A,sz);

    std:: cout << "after merge sorting \n" << std::endl;
    mergeSort(A,0,sz-1);
    print_sort_result(A,sz);
}

void test_quick_sort(){
    int A[7] = {13,7,9,3,11,6,12};
    int sz = sizeof(A)/sizeof(A[0]);
    
    std:: cout << "before quick sorting \n" << std::endl;
    print_sort_result(A,sz);

    std:: cout << "after quick sorting \n" << std::endl;
    quick_sort(A,0,sz-1);
    print_sort_result(A,sz);
}

void test_shell_sort(){
    int A[7] = {13,7,9,3,11,6,12};
    int sz = sizeof(A)/sizeof(A[0]);
    int gap = 4;
    std:: cout << "before shell sorting \n" << std::endl;
    print_sort_result(A,sz);

    std:: cout << "after shell sorting \n" << std::endl;
    shell_sort(A,gap,sz);
    print_sort_result(A,sz);
}

void test_heap_sort(){
    int A[8] = {0,13,7,9,3,11,6,12};
    int sz = sizeof(A)/sizeof(A[0]);
    std:: cout << "before heap sorting \n" << std::endl;
    print_sort_result(A,sz);

    std:: cout << "after heap sorting \n" << std::endl;
    heap_sort(A,sz-1);
    print_sort_result(A,sz);
}

void test_count_sort(){
    int A[8] = {0,13,7,9,3,11,6,12};
    int sz = sizeof(A)/sizeof(A[0]);
    std:: cout << "before count sorting \n" << std::endl;
    print_sort_result(A,sz);

    std:: cout << "after count sorting \n" << std::endl;
    counting_sort(A,sz,13);
    print_sort_result(A,sz);
}

void test_radix_sort(){
    int A[7] = {13,7,9,3,11,6,12};
    int sz = sizeof(A)/sizeof(A[0]);
    
    std:: cout << "before radix sorting \n" << std::endl;
    print_sort_result(A,sz);

    std:: cout << "after radix sorting \n" << std::endl;
    radix_sort(A,sz);
    print_sort_result(A,sz);
}

void test_bitonic_sort(){
    int A[8] = {0,13,7,9,3,11,6,12};
    int sz = sizeof(A)/sizeof(A[0]);
    
    std:: cout << "before bitonic sorting \n" << std::endl;
    print_sort_result(A,sz);

    std:: cout << "after bitonic sorting \n" << std::endl;
    bitonic_sort(A,sz,1);
    print_sort_result(A,sz);
}
int main(){
    std::cout <<"==================================\n\n";
    test_bubble_sort();
    std::cout <<"==================================\n\n";
    test_insert_sort();
    std::cout <<"==================================\n\n";
    test_selection_sort();
    std::cout <<"==================================\n\n";
    test_merge_sort();
    std::cout <<"==================================\n\n";
    test_quick_sort();
    std::cout <<"==================================\n\n";
    test_shell_sort();
    std::cout <<"==================================\n\n";
    test_heap_sort();
    std::cout <<"==================================\n\n";
    test_count_sort();
    std::cout <<"==================================\n\n";
    //test_radix_sort();
    //std::cout <<"==================================\n\n";
    test_bitonic_sort();
    std::cout <<"==================================\n\n";
}