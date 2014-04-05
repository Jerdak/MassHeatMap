#ifndef DATABASE_H
#define DATABASE_H

#include <vector>

class Database
{
public:
    typedef std::vector<float> DataRow;
    typedef std::vector<QString> HeaderRow;
    typedef std::vector<DataRow> Data;

    Database();
    ~Database(){}

    void dbgData(const int& size=-1);

    void BuildMetrics();

    void AppendRow(const QStringList& tokens);
    void SetHeader(const QStringList& tokens);

    bool LoadData(const QString& file_name);
    bool SaveData(const QString& file_name);

    float Min(const size_t& column) ;
    float Max(const size_t& column) ;

    void Clear();
    bool Empty();

    size_t NumColumns();
    size_t NumRows() ;

    float operator()(const int&r, const int& c){return data_[r][c];}
    const Data& get_data(){return data_;}
private:
    Data data_;
    HeaderRow header_row_;

    std::vector<float> min_;
    std::vector<float> max_;
    std::vector<float> average_;
};

#endif // DATABASE_H
