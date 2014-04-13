#ifndef CUDAWRAPPER_H
#define CUDAWRAPPER_H

class CudaWrapper
{
public:
    CudaWrapper();
    void Reduce(osg::ref_ptr<osg::Vec4Array> colors_,const std::vector<int> active_subjects,const unsigned int& R, const unsigned int&C,const float *data,int size);
};

#endif // CUDAWRAPPER_H
