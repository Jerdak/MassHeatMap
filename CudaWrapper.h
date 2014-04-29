#ifndef CUDAWRAPPER_H
#define CUDAWRAPPER_H

#define CUDA_HISTOGRAM_BINS 256

class CudaWrapper
{
public:
    CudaWrapper();
    ~CudaWrapper();

    // Compress raw row oriented data in to a single column
    void PreprocessPack(const int& color_size,const std::vector<int>& inactive_subjects,const unsigned int& R, const unsigned int&C,const float *data,int size);
    void Reduce(osg::ref_ptr<osg::Vec4Array> colors,const std::vector<int>& inactive_subjects,const unsigned int& R, const unsigned int&C,const float *data,int size);

    // Generate a normalized color lookup table using histogram equalization.
    //
    // Lookup table range is [0,CUDA_HISTOGRAM_BINS]
    void NormalizedHistogram(float *colors, const int& color_size, float &minc, float &maxc, const int&size);

    // Extract various color metrics like min/max color
    void Metrics(float *colors, const int& color_size, float &minc, float &maxc);
protected:
    bool validColor(const float& f);
private:
    float hist_[CUDA_HISTOGRAM_BINS];
    int lookup_[CUDA_HISTOGRAM_BINS];

    float valid_data_thresh_;
    float *colors_;
};

#endif // CUDAWRAPPER_H
