#include "Database.h"
#include <map>
#include <QStringList>
#include <QMutexLocker>
#undef max
#undef min

Database::Database():
    coverage_data_packed_(NULL),
    coverage_data_size_(0),
    data_mutex_(QMutex::Recursive)
{

}

Database::Database(const Database& db):
    coverage_data_packed_(NULL),
    coverage_data_size_(0),
    data_mutex_(QMutex::Recursive)
{
    min_ = std::vector<float>(db.min_);
    max_ = std::vector<float>(db.max_);
    average_ = std::vector<float>(db.average_);
    header_row_ = HeaderRow(db.header_row_);
    eigen_values_ = std::vector<float>(db.eigen_values_);
    inactive_subjects_ = std::vector<int>(db.inactive_subjects_);
    subjects_data_ = std::vector<int>(db.subjects_data_);
    subjects_coverage_data_ = std::vector<int>(db.subjects_coverage_data_);

    data_ = Data(db.data_);
    coverage_data_size_ = db.coverage_data_size_;

    if(coverage_data_packed_){delete [] coverage_data_packed_;}

    coverage_data_packed_ = new float[coverage_data_size_];
    memcpy(coverage_data_packed_,db.coverage_data_packed_,coverage_data_size_ * sizeof(float));
}

void Database::operator=(const Database& db)
{
    min_ = std::vector<float>(db.min_);
    max_ = std::vector<float>(db.max_);
    average_ = std::vector<float>(db.average_);
    header_row_ = HeaderRow(db.header_row_);
    eigen_values_ = std::vector<float>(db.eigen_values_);
    inactive_subjects_ = std::vector<int>(db.inactive_subjects_);
    subjects_data_ = std::vector<int>(db.subjects_data_);
    subjects_coverage_data_ = std::vector<int>(db.subjects_coverage_data_);

    data_ = Data(db.data_);
    coverage_data_size_ = db.coverage_data_size_;

    if(coverage_data_packed_){delete [] coverage_data_packed_;}

    coverage_data_packed_ = new float[coverage_data_size_];
    memcpy(coverage_data_packed_,db.coverage_data_packed_,coverage_data_size_ * sizeof(float));
}

void Database::dbgData(const int& size){
    int rows = (size==-1)?NumRows():size;

    QString temp = "";
    for(int c = 0; c < NumColumns(); ++c){
        temp += header_row_[c] + " ";
    }
    qDebug() << temp;

    for(int r = 0; r < rows; ++r){
        temp = "";
        for(int c = 0; c < NumColumns(); ++c){
            temp += QString::number(data_[r][c]) + " ";
        }
        qDebug() << temp;
    }

}
void Database::set_inactive_subjects(std::vector<int> inactive_subjects){
    QMutexLocker locker( &data_mutex_ );
    inactive_subjects_ = inactive_subjects;
}

std::vector<int> Database::get_inactive_subjects(){
    QMutexLocker locker(&data_mutex_);
    return inactive_subjects_;
}
void Database::dbgVerifyIntegrity(){
    qDebug() << "Coverage subject count: " << subjects_coverage_data_.size();
    qDebug() << "Coverage row count: " << coverage_data_.size();

    qDebug() << "PCA subject count: " << subjects_data_.size();
    qDebug() << "PCA row count: " << data_.size();

    FilterByCoverage();

    qDebug() << "Coverage subject count: " << subjects_coverage_data_.size();
    qDebug() << "Coverage row count: " << coverage_data_.size();

    qDebug() << "PCA subject count: " << subjects_data_.size();
    qDebug() << "PCA row count: " << data_.size();
}

void Database::BuildMetrics(){
    min_.clear();
    max_.clear();
    average_.clear();

    min_.resize(NumColumns());
    max_.resize(NumColumns());
    average_.resize(NumColumns());

    std::fill(min_.begin(), min_.begin() + min_.size(),std::numeric_limits<float>::max());  //min gets max limit so that next comparison will be lower
    std::fill(max_.begin(), max_.begin() + max_.size(),std::numeric_limits<float>::min());  //max gets min limit so that next comparison will be lower
    std::fill(average_.begin(), average_.begin() + average_.size(),0.0f);

    for(int r = 0; r < NumRows(); ++r){
        for(int c = 0; c < NumColumns(); ++c){
            if(data_[r][c] < min_[c])min_[c] = data_[r][c];
            if(data_[r][c] > max_[c])max_[c] = data_[r][c];
            average_[c] += data_[r][c];
        }

    }
    for(int c = 0; c < NumColumns(); ++c){
        average_[c]/= float(NumRows());
    }
}
void Database::FilterByCoverage(){
    Data newData;
    std::vector<int> newSubjects;
    std::map<int,char> filterMap;
    for(int i = 0; i < subjects_coverage_data_.size(); ++i){
        filterMap[subjects_coverage_data_[i]] = 0;
    }
    for(int i = 0; i < subjects_data_.size();++i){
        if(filterMap.find(subjects_data_[i]) != filterMap.end()){
            newData.push_back(data_[i]);
            newSubjects.push_back(subjects_data_[i]);
        }
    }
    data_ = newData;
    subjects_data_ = newSubjects;

    for(int i = 0; i < data_.size(); ++i){
        qDebug() << subjects_data_[i] << ": " << data_[i][0] << " Size: " << data_[i].size();
    }

}

float Database::Min(const size_t& column) {
    if(column >= min_.size()){
        qDebug() << "Database::Min invalid column <" << column << ">";
        return -1;
    }
    return min_[column];
}

float Database::Max(const size_t& column) {
    if(column >= max_.size()){
        qDebug() << "Database::Max invalid column <" << column << ">";
        return -1;
    }
    return max_[column];
}
#include <QString>
void Database::AppendRow(const QStringList& tokens){
    if(tokens.size() != NumColumns()){
        QString temp = QString("Mismatched header/row combination.  Original Header Count: %1 New row Count: %2\n").arg(QString::number(NumColumns()),QString::number(tokens.size()));
        qDebug() <<  temp;
        return;
    }
    DataRow new_row;
    for(int i = 0; i < tokens.size(); i++){
        new_row.push_back(tokens[i].toFloat());
    }
    data_.push_back(new_row);
}

void Database::SetEigenValues(const QStringList& eigs){
    eigen_values_.clear();
    for(int i = 0; i < eigs.size(); i++){
        eigen_values_.push_back(eigs[i].toFloat());
    }
}

QString Database::get_header(const int& idx){
     if(idx<0 || idx >= header_row_.size()){
         qDebug() << "Index out of range, no such header <" << idx << ">\n";
         return QString("");
     }
     return header_row_[idx];
}

float Database::get_eigen_value(const int& idx){
    if(idx<0 || idx >= eigen_values_.size())return -1;  //quietly ignore and return dummy value so that that non-pca columns can continue
    return eigen_values_[idx];
}

void Database::SetHeader(const QStringList& tokens){
    if(NumColumns() != tokens.size() && NumColumns() != 0){
        QString temp = QString("Mismatched header/header combination.  Original Header Count: %1 New Header Count: %2\n").arg(QString::number(NumColumns()),QString::number(tokens.size()));
        qDebug() <<  temp;
        return;
    }
    header_row_.clear();
    header_map_.clear();
    for(int i = 0; i < tokens.size(); i++){
        header_row_.push_back(tokens[i]);
        header_map_[tokens[i]] = i;
    }
}

size_t Database::NumColumns() const{
    return header_row_.size();
}

size_t Database::NumRows() const{
    return data_.size();
}


void Database::AppendCoverageRow(const QStringList& tokens){
    if(tokens.size() != subjects_coverage_data_.size()){
        QString temp = QString("ERROR, CoverageRow.  Mismatched header/row combination.  Original Header Count: %1 New Header Count: %2\n").arg(QString::number(subjects_coverage_data_.size()),QString::number(tokens.size()));
        qDebug() <<  temp;
        return;
    }
    DataRow new_row;
    for(int i = 0; i < tokens.size(); i++){
        new_row.push_back(tokens[i].toFloat());

    }
    coverage_data_.push_back(new_row);
}

size_t Database::NumPackColumns() const{
    return subjects_coverage_data_.size();
}

size_t Database::NumPackRows() const {
    return coverage_data_.size();
}
void Database::PackCoverageData(){
    if(coverage_data_packed_)delete [] coverage_data_packed_;
    int rows = coverage_data_.size();
    int cols = subjects_coverage_data_.size();

    coverage_data_size_ = rows * cols;
    coverage_data_packed_ = new float[coverage_data_size_];
    int idx = 0;
    for(int c = 0; c < cols; ++c){
        for(int r = 0; r <rows;++r){
            idx = c * rows + r;
            coverage_data_packed_[idx] = coverage_data_[r][c];
        }
    }
}

bool Database::LoadCoveragePack(const QString& file_name){
    QFile file(file_name);
    subjects_coverage_data_.clear();
    if(coverage_data_packed_)delete [] coverage_data_packed_;

    if(file.open(QIODevice::ReadOnly| QIODevice::Text)){
        QString header_line = file.readLine().simplified();
        QStringList headers = header_line.split(',');

        // first row of packed data contains subject names
        for(int i = 0; i < headers.size(); ++i){
            subjects_coverage_data_.push_back(headers[i].toInt());
        }

        int count = -1;
        // remaining rows are all
        while(!file.atEnd()){
            count += 1;
            QString line = file.readLine().simplified();
            QStringList tokens = line.split(',');

            AppendCoverageRow(tokens);
        }
        file.close();
        PackCoverageData();
        return true;
    }

    qDebug() << "Unable to open CSV file <" << file_name << "> for reading\n";
}

bool Database::LoadData(const QString& file_name){
    QFile file(file_name);
    if(file.open(QIODevice::ReadOnly| QIODevice::Text)){
        QString header_line = file.readLine().simplified();
        QStringList headers = header_line.split(',');
        headers.removeAt(0);    //pop first column, this value identifies the subject name column
        SetHeader(headers);

        while(!file.atEnd()){
            QString line = file.readLine().simplified();
            QStringList tokens = line.split(',');

            //column 0 is for subjects, extract and pop from tokens.
            subjects_data_.push_back(tokens[0].toInt());
            tokens.removeAt(0);


            AppendRow(tokens);
        }
        file.close();
        BuildMetrics();
        return true;
    }
    qDebug() << "Unable to open CSV file <" << file_name << "> for reading\n";

    return false;
}



Database::DataColumn Database::get_column(const int& idx){
    DataColumn ret;
    if(idx < 0 || idx >= NumColumns()){
        qDebug() << "Invalid index for get_column: 0 <= " << idx << " <= " << NumColumns() << "\n";
        return ret;
    }

    for(int i = 0; i < NumRows(); ++i){
        ret.push_back(data_[i][idx]);
    }
    return ret;
}
void Database::append_column(const DataColumn& col, const QString& name, const bool& rebuild_metrics){
    if(col.size() != NumRows()){
        qDebug() << "Cannot append column of length <" << col.size() << "> to <" << NumRows() << "> rows.  Must be equal length.\n";
        return;
    }
    header_row_.push_back(name);
    header_map_[name] = header_row_.size() - 1;
    for(size_t i = 0; i < col.size(); ++i){
        data_[i].push_back(col[i]);
    }
    if(rebuild_metrics)BuildMetrics();
}
bool Database::SaveData(const QString& file_name){
    QFile file(file_name);
    if(file.open(QIODevice::WriteOnly| QIODevice::Text)){
        QTextStream out(&file);
        out << "SubjectID";
        for(int c = 0; c < NumColumns(); ++c){
            out << "," + header_row_[c];
        }
        out <<"\n";

        for(int r = 0; r < NumRows(); ++r){
            out << subjects_data_[r];
            for(int c = 0; c < NumColumns(); ++c){
                out << ","+QString::number(data_[r][c]);
            }
            out << "\n";
        }
    } else {
        qDebug() << "Unable to open CSV file <" << file_name << "> for writing\n";

        return false;
    }
    return true;
}

int Database::get_header_index(const QString& title){
    return header_map_[title];
}

void Database::Clear(){
    data_.clear();
    header_row_.clear();
    header_map_.clear();
    coverage_data_.clear();
    subjects_data_.clear();
    subjects_coverage_data_.clear();
    min_.clear();
    max_.clear();
    average_.clear();

    if(coverage_data_packed_){
        delete [] coverage_data_packed_;
        coverage_data_packed_ = NULL;
    }
}

bool Database::Empty(){
    return data_.empty();
}
