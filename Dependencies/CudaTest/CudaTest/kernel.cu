#include "CudaKernel.hpp"
#include "EventTimer.h"
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/generate.h>
#include <thrust/reduce.h>
#include <thrust/functional.h>
#include <thrust/random.h>
#include <iostream>
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>       /* time */
#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }


thrust::host_vector<float> preload_data_host;
thrust::device_vector<float> preload_data_device;

// convert a linear index to a row index
template <typename T>
struct linear_index_to_row_index : public thrust::unary_function<T,T>
{
  T C; // number of columns
  
  __host__ __device__
  linear_index_to_row_index(T C) : C(C) {}

  __host__ __device__
  T operator()(T i)
  {
    return i / C;
  }
};
struct is_valid_column : thrust::unary_function<bool, bool>
{
    __host__ __device__
    bool operator()(const bool &x)
    {
        return x;
    }
};

void cuda_preload_data(float *data){
	preload_data_device.clear();
}

void color_pack(const std::vector<int>& indices,const unsigned int& R, const unsigned int& C,float *colors,const float *data){
	unsigned int NEWC = C;
	unsigned int RC = R*C;
	
	//printf("Color pack.  R: %d  C: %d\n",R,C);
	//printf("Inactive Subject Count: %d\n",indices.size());
	// generate an index map for faster lookup
	std::map<int,char> index_map;
	{
		for(auto index = indices.begin(); index != indices.end(); ++index){
			index_map[(*index)] = 0;
		}
		NEWC -= index_map.size();	//remove K columns from total
	}
	//printf("Index map created.  Size: %d  New column size: %d\n",index_map.size(),NEWC);


	thrust::device_vector<float> array(R * NEWC);
	thrust::host_vector<float> array_host(R * NEWC);
	//printf("created array: %d\n",array.size());
	//printf("remove %d columns\n",index_map.size());

	//Common::EventTimer ev("filter");
	//ev.BeforeEvent();
	{	// faster filter?

		// quick filter uses column oriented data struction were data are stored .  
		int cidx = 0,nc = 0;
		while(cidx < RC){
			int c = cidx/R;
			if(index_map.find(c) != index_map.end()){
				cidx += R;
				continue;
			}
			

			int ridx = nc;
			for(int i = 0; i < R; i++){
				array_host[ridx] = data[cidx + i];
				ridx += NEWC;
			}
			cidx += R;
			nc += 1;
		}
		
	}
	//ev.AfterEvent();
	//ev.Display();
	array = array_host;

	//Common::EventTimer ev2("reduce");
	//ev2.BeforeEvent();

	thrust::device_vector<float> row_sums(R);
	thrust::device_vector<float> row_indices(R);
	{	//reduce
		thrust::reduce_by_key(
			thrust::make_transform_iterator(thrust::counting_iterator<int>(0), linear_index_to_row_index<int>(NEWC)),
			thrust::make_transform_iterator(thrust::counting_iterator<int>(0), linear_index_to_row_index<int>(NEWC)) + (R*NEWC),
			array.begin(),
			row_indices.begin(),
			row_sums.begin(),
			thrust::equal_to<float>(),
			thrust::plus<float>()
		);
	}
	//ev2.AfterEvent();
	//ev2.Display();

	//Common::EventTimer ev3("repack");
	//ev3.BeforeEvent();
	thrust::host_vector<float> row_sums_host = row_sums;
	for(size_t i = 0; i < R;++i){
		colors[i] = row_sums_host[i];
	}
	//printf("repacked color[0]: %f\n",colors[0]);
	//ev3.AfterEvent();
	//ev3.Display();
}


void color_pack_test(){
	unsigned int R = 10777;     // number of rows
	unsigned int C = 2062;     // number of columns
	unsigned int RC = R*C;
	float *colors = new float[R];
	std::vector<int> indices;
	{
		indices.push_back(1);
		indices.push_back(5);
		indices.push_back(7);
	}
	float *data = new float[RC];
	{
		for(int i = 0; i < RC; ++i){
			data[i] = i%C;
		}
	}

	color_pack(indices,R,C,colors,data);
	printf("c: %f\n",colors[0]);
	printf("c: %f\n",colors[R-1]);
	delete [] data;
}

int thrust_main(void)
{
  int R = 10777;     // number of rows
  int C = 2062;     // number of columns
  int RC = R*C;

    // initialize data
  thrust::device_vector<float> array(R * C);
  thrust::host_vector<float> array_host(R * C);
  printf("created array: %d\n",array.size());

  for (size_t i = 0; i < array.size(); i++){
	  array_host[i] = i%C;
  }
  
  printf("host array filled\n");
  array = array_host;

  printf("a\n");
  // allocate storage for row sums and indices
  thrust::device_vector<float> row_sums(R);
  thrust::device_vector<float> row_indices(R);
  
  printf("b\n");
  // compute row sums by summing values with equal row indices
  thrust::reduce_by_key
    (thrust::make_transform_iterator(thrust::counting_iterator<int>(0), linear_index_to_row_index<int>(C)),
     thrust::make_transform_iterator(thrust::counting_iterator<int>(0), linear_index_to_row_index<int>(C)) + (R*C),
     array.begin(),
     row_indices.begin(),
     row_sums.begin(),
     thrust::equal_to<float>(),
     thrust::plus<float>());
  
  thrust::host_vector<float> row_sums_host = row_sums;
  printf("c: %f\n",row_sums_host[0]);
  printf("c: %f\n",row_sums_host[R-1]);
  // print data 
  /*for(int i = 0; i < R; i++)
  {
    std::cout << "[ ";
    for(int j = 0; j < C; j++)
      std::cout << array[i * C + j] << " ";
    std::cout << "] = " << row_sums[i] << "\n";
  }*/

  return 0;
}
void cuda_main(){
	//memory_test_copy_pinned();
	//thrust_main();
	color_pack_test();
}
