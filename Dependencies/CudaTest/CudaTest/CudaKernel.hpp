#ifndef CUDAKERNEL_HPP
#define CUDAKERNEL_HPP

#include <vector>
void cuda_preload_data(float *data);
void cuda_main();
void color_pack(const std::vector<int> indices,const unsigned int& R, const unsigned int& C,float *colors,const float *data);
void color_pack_test();
#endif // CUDAKERNEL_HPP
