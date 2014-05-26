#include "PrincipalComponentAnalyzer.h"
#include <QStringList>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

PrincipalComponentAnalyzer::PrincipalComponentAnalyzer()
{
}
Database PrincipalComponentAnalyzer::GeneratePCATable(const Database& input_db){
    Database ret;
    cv::Mat data_mat(input_db.NumRows(),input_db.NumColumns(),CV_32F);
    cv::Mat data_mean(1,input_db.NumColumns(),CV_32F);

    float min = std::numeric_limits<float>::max();
    float max = std::numeric_limits<float>::min();
    for(size_t c = 0; c < input_db.NumColumns(); c++){
        data_mean.at<float>(0,c)= 0.0f;
    }

    for(size_t r = 0; r < input_db.NumRows(); r++){
        for(size_t c = 0; c < input_db.NumColumns(); c++){
            data_mat.at<float>(r,c) = input_db.get_data()[r][c];
            data_mean.at<float>(0,c) += input_db.get_data()[r][c];
            if(data_mat.at<float>(r,c) < min)min=data_mat.at<float>(r,c);
            if(data_mat.at<float>(r,c) > max)max=data_mat.at<float>(r,c);
        }
    }
    for(size_t c = 0; c < input_db.NumColumns(); c++){
        data_mean.at<float>(0,c) /= (float)input_db.NumRows();
    }
    printf("Min data value: %f\n",min);
    printf("Max data value: %f\n",max);
    printf("Eigen rows: %d\n",data_mat.rows);
    printf("Eigen cols: %d\n",data_mat.cols);
    cv::PCA pca(data_mat,cv::Mat(),CV_PCA_DATA_AS_ROW);
    cv::Mat mean = pca.mean.clone();
    cv::Mat eigenvalues = pca.eigenvalues.clone();
    cv::Mat eigenvectors = pca.eigenvectors.clone();
    cv::Mat result;
    pca.project(data_mat,result);
    printf("Eigen rows: %d\n",result.rows);
    printf("Eigen cols: %d\n",result.cols);
    printf("EigenValue cols: %d\n",eigenvalues.cols);
    printf("EigenValue rows: %d\n",eigenvalues.rows);
    printf("mean cols: %d\n",mean.cols);
    printf("mean rows: %d\n",mean.rows);

    QStringList headers;

    for(int i = 0; i < result.cols; i++){
        headers << "pca"+QString::number(i);
    }
    printf("headers: %d\n" ,headers.count());
    ret.SetHeader(headers);

    float total_variance = 0.0f;
    for(int i = 0; i < eigenvalues.rows; i++){
        total_variance += eigenvalues.at<float>(i,0);
    }
    QStringList eigens;
    for(int i = 0; i < eigenvalues.rows; i++){
        if(i<10)printf("Eigen[%d]: %f = %f%\n",i,eigenvalues.at<float>(i,0),eigenvalues.at<float>(i,0)/total_variance);
        if(i<10)printf("Mean[%d]: %f\n",i,mean.at<float>(0,i));
        if(i<10)printf("Mean[%d]: %f\n",i,data_mean.at<float>(0,i));
        eigens<<QString::number(eigenvalues.at<float>(i,0)/total_variance);

    }
    ret.SetEigenValues(eigens);
    for(int r = 0; r < result.rows; r++){
        QStringList row;
        for(int c = 0; c < result.cols; c++){
            row << QString::number(result.at<float>(r,c));
        }
        ret.AppendRow(row);
    }
    return ret;
}
