#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <QMutex>

class Database
{
public:
    typedef std::vector<float> DataRow;
    typedef std::vector<QString> HeaderRow;
    typedef std::vector<DataRow> Data;

    Database();
    Database(const Database& db);
    ~Database(){}

    void dbgData(const int& size=-1);
    void dbgVerifyIntegrity();

    void BuildMetrics();

    void AppendRow(const QStringList& tokens);
    void AppendCoverageRow(const QStringList& tokens);

    void FilterByCoverage();

    void SetHeader(const QStringList& tokens);

    bool LoadCoveragePack(const QString& file_name);
    void PackCoverageData();
    bool LoadData(const QString& file_name);
    bool SaveData(const QString& file_name);

    float Min(const size_t& column) ;
    float Max(const size_t& column) ;

    void Clear();
    bool Empty();

    size_t NumColumns() const;
    size_t NumRows() const;

    size_t NumPackColumns() const;
    size_t NumPackRows() const;

    float operator()(const int&r, const int& c){return data_[r][c];}
    void operator=(const Database& db);

    const Data& get_data()const{return data_;}
    const float* get_coverage_data()const{return coverage_data_packed_;}

    void set_inactive_subjects(std::vector<int> inactive_subjects);

    std::vector<int> get_inactive_subjects();

private:
    QMutex data_mutex_;
    Data data_;
    Data coverage_data_;

    std::vector<int> subjects_data_;
    std::vector<int> subjects_coverage_data_;

    HeaderRow header_row_;
    float* coverage_data_packed_;
    int coverage_data_size_;
    std::vector<float> min_;
    std::vector<float> max_;
    std::vector<float> average_;
    std::vector<int> inactive_subjects_;
};

#endif // DATABASE_H
