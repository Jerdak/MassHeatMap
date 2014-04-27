#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <QMutex>
#include <map>

class Database
{
public:
    typedef std::vector<float> DataRow;
    typedef std::vector<float> DataColumn;

    typedef std::vector<QString> HeaderRow;
    typedef std::vector<DataRow> Data;

    Database();
    Database(const Database& db);
    ~Database(){}

    void dbgData(const int& size=-1);

    // Verify measurement data and coverage data have equivalent shapes
    void dbgVerifyIntegrity();

    // Build internal metrics like min, max, average
    void BuildMetrics();

    // Append row of measurement data to data cache
    void AppendRow(const QStringList& tokens);

    // Append row of coverage data to coverage data cache.
    void AppendCoverageRow(const QStringList& tokens);

    // Downsample measurement data by the subjects from the coverage map
    //
    // Note:
    //   Occasionally it is necessary to decrease computational complexity by limiting the amount
    //   of coverage data we allow.  Rather than maintaining multiple copies of measurement and coverage data
    //   with matching subject lists MassHeatMap removes subjects that don't have coverage from the data. But not vice versa
    void FilterByCoverage();

    // Manually set header row
    void SetHeader(const QStringList& tokens);

    void SetEigenValues(const QStringList& eigs);
    float get_eigen_value(const int& idx);
    // Assumes coverage data
    //
    // Format:
    //   Row oriented
    //      First row = subject names
    //      Rows [1...N] = Vertices[0...N-1]
    //   Example:
    //     column:  0   1   2   3
    //      row[0]:  5   12  10  6
    //      row[1]: 1.2 4.5 1.8 5.6
    //    So data[1][1] means vertex[0] for subject[5] is 1.2
    //       data[1][2] means vertex[0] for subject[12] is 4.5
    bool LoadCoveragePack(const QString& file_name);

    // Pack coverage data as a 1D array for fast copy to CUDA
    void PackCoverageData();

    // Load measurement or pca data. Data should be row oriented.
    //
    // Format:
    //   Row oriented
    //      First row = measurement name
    //      First Column = subject name
    //      Rows [1...N] = Subject measurements
    bool LoadData(const QString& file_name);
    bool SaveData(const QString& file_name);

    float Min(const size_t& column) ;
    float Max(const size_t& column) ;

    // Clear all data from measurement and coverage maps
    void Clear();

    // Return true IFF no measurement or coverge data exists
    bool Empty();

    size_t NumColumns() const;
    size_t NumRows() const;

    size_t NumPackColumns() const;
    size_t NumPackRows() const;

    float operator()(const int&r, const int& c){return data_[r][c];}
    void operator=(const Database& db);

    // Get const-ref to raw measurement data
    const Data& get_data()const{return data_;}

    // Get const-ptr to raw packed coverage data
    const float* get_coverage_data()const{return coverage_data_packed_;}

    QString get_header(const int& idx);

    // Set subjects that have been excluded by a filter
    void set_inactive_subjects(std::vector<int> inactive_subjects);

    // Return a copy of all subjects excluded by a filter
    std::vector<int> get_inactive_subjects();

    DataColumn get_column(const int& idx);
    void append_column(const DataColumn& col, const QString& name, const bool& rebuild_metrics=true);

    std::vector<int> get_subjects(){return subjects_data_;}
    void set_subjects(std::vector<int> subjects){subjects_data_ = subjects;}
    int get_header_index(const QString& title);

    std::vector<float> get_eigen_values(){return eigen_values_;}
    void set_eigen_values(const std::vector<float> & eigen_values){
        eigen_values_ = std::vector<float>(eigen_values);
    }

private:

    // Lockable mutex to control threaded data access
    QMutex data_mutex_;

    // Measuremetn data
    Data data_;

    // Coverage Data
    Data coverage_data_;

    // Subjects from measurement data's first row
    std::vector<int> subjects_data_;

    // Subjects from coverage data's first row
    std::vector<int> subjects_coverage_data_;

    std::map<QString,int> header_map_;
    // Header row of measurement data, represents # of measurements or PCs
    HeaderRow header_row_;

    // 1D packed coverage data for CUDA
    float* coverage_data_packed_;

    // Size of coverage data
    int coverage_data_size_;

    // Smallest value for a measurement
    std::vector<float> min_;

    // Largest value for a measurement
    std::vector<float> max_;

    // Average value for all measuremetns
    std::vector<float> average_;

    std::vector<float> eigen_values_;
    //
    std::vector<int> inactive_subjects_;
};

#endif // DATABASE_H
