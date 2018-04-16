#include <iostream>
#include <vector>
#include <time.h>
#include <unistd.h>
#include <queue>
#include <functional>
#include <cilk/cilk.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <sys/time.h>
#include <cilk/reducer_opadd.h>


#define SIZE 50


double wctime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + 1E-6 * tv.tv_usec;
}

using namespace std;
vector<int> prefixSum(vector<int> &nums,int low,int high){

    int n=high-low+1;

    vector<int> A(n/2);
    vector<int> res(n);

    res[0]=nums[0];

    if(n>1){
        cilk_for(int i=0;i<n/2;i++){
        A[i]=nums[2*i]+nums[2*i+1];
    }

    vector<int> temp=prefixSum(A,low,low+(high-low)/2);
    cilk_for(int i=1;i<n;i++){
        if(i%2==1)
            res[i]=temp[i/2];
        else
            res[i]=temp[(i-1)/2]+nums[i];
        }
    }
    
    return res;
}

int parPartition(vector<int> &nums,int x){

    int low=0;
    int high=nums.size()-1;

    int n=high-low+1;

    if(n==1)
        return low;

    vector<int> temp(n);
    vector<int> less(n);
    vector<int> greater(n);

    cilk_for (int i=0;i<n;i++){
        temp[i]=nums[low+i];
        if(temp[i]<x)
            less[i]=1;
        else
            less[i]=0;
        if(temp[i]>x)
            greater[i]=1;
        else
            greater[i]=0;
    }
    less=prefixSum(less,0,n-1);
    greater=prefixSum(greater,0,n-1);

    int k=low+less[n-1];
    nums[k]=x;

    cilk_for (int i=0;i<n;i++){
        if(temp[i]<x)
            nums[low+less[i]-1]=temp[i];
        else if(temp[i]>x)
            nums[k+greater[i]]=temp[i];
    }

    return k;
}

void merge(vector<int> &nums,int low,int mid,int high){

    int i,j,k;
    int n1=mid-low+1;
    int n2=high-mid;

    vector<int> left(n1);
    vector<int> right(n2);

    for(int i=0;i<n1;i++)
        left[i]=nums[low+i];
    for(int j=0;j<n2;j++)
        right[j]=nums[mid+1+j];

    i=0;
    j=0;
    k=low;

    while(i<n1&&j<n2){

        if(left[i]<=right[j]){
            nums[k]=left[i];
            i++;
        }
        else{
            nums[k]=right[j];
            j++;
        }
        k++;
        while(i<n1){
            nums[k]=left[i];
            i++;
            k++;
        }
        while(j<n2){
            nums[k]=right[j];
            j++;
            k++;
        }
    }  
} 

void mergeSort(vector<int> &nums,int low,int high){

    if(low<high){

        int mid=low+(high-low)/2;
        mergeSort(nums,low,mid);
        mergeSort(nums,mid+1,high);
        merge(nums,low,mid,high);
    }
}

void quickSort(vector<int> &nums,int low,int high){

    srand( time( NULL ) );

    if(low>=high)
        return;

    int n=high-low+1;

    if(n<=30){
        mergeSort(nums,low,high);
        return;
    }
    int indx=0;
    if(nums.size()%2==0)
        indx=(nums.size()/2+nums.size()/2)/2;
    else
        indx=nums.size()/2;

    int x=nums[indx];
    int k=parPartition(nums,x);
    cilk_spawn quickSort(nums,low,k-1);
    quickSort(nums,k+1,high);
}

int main() {
    srand(123);

    vector<int> A(SIZE);
    for (int i = 0; i < SIZE; i++)
    {
        int input=rand()%100;
        A[i]=input;
    }
    double start, end;
    start = wctime();
    cilk_spawn quickSort(A, 0, SIZE-1);
    cilk_sync;
    end = wctime();

    cout << "Time:" << end-start << endl;

    for(int i=0;i<A.size();i++)
        cout << A[i] << " ";

    return 0;

}
              