#include "PrincipalComponentAnalyzer.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

PrincipalComponentAnalyzer::PrincipalComponentAnalyzer()
{
}
#include <QStringList>
Database PrincipalComponentAnalyzer::GeneratePCATable(const Database& input_db){
    Database ret;
    cv::Mat data_mat(input_db.NumRows(),input_db.NumColumns()-1,CV_32F);

    for(int r = 0; r < input_db.NumRows(); r++){
        for(int c = 1; c < input_db.NumColumns(); c++){
            data_mat.at<float>(r,c-1) = input_db.get_data()[r][c];
        }
    }
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

    QStringList headers;

    for(int i = 0; i < result.cols; i++){
        headers << "pca"+QString::number(i);
    }
    printf("headers: %d" ,headers.count());
    ret.SetHeader(headers);

    for(int r = 0; r < result.rows; r++){
        QStringList row;
        for(int c = 0; c < result.cols; c++){
            row << QString::number(result.at<float>(r,c));

        }
        ret.AppendRow(row);
    }
    return ret;
}
