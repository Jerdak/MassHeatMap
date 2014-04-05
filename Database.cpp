#include "Database.h"
#undef max
#undef min

Database::Database()
{

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

    qDebug() << "Min size: " << min_.size();
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

void Database::AppendRow(const QStringList& tokens){
    if(tokens.size() != NumColumns()){
        QString temp = QString("Mismatched header/row combination.  Original Header Count: %d New Header Count: %d\n").arg(NumColumns(),tokens.size());
        qDebug() <<  temp;
        return;
    }
    DataRow new_row;
    for(int i = 0; i < tokens.size(); i++){
        new_row.push_back(tokens[i].toFloat());
    }
    data_.push_back(new_row);
}

void Database::SetHeader(const QStringList& tokens){
    if(NumColumns() != tokens.size() && NumColumns() != 0){
        QString temp = QString("Mismatched header/header combination.  Original Header Count: %d New Header Count: %d\n").arg(NumColumns(),tokens.size());
        qDebug() <<  temp;
        return;
    }
    header_row_.clear();
    for(int i = 0; i < tokens.size(); i++){
        header_row_.push_back(tokens[i]);
    }
}

size_t Database::NumColumns(){
    return header_row_.size();
}

size_t Database::NumRows(){
    return data_.size();
}

bool Database::LoadData(const QString& file_name){
    QFile file(file_name);
    if(file.open(QIODevice::ReadOnly| QIODevice::Text)){
        QString header_line = file.readLine().simplified();
        QStringList headers = header_line.split(',');
        SetHeader(headers);

        while(!file.atEnd()){
            QString line = file.readLine().simplified();
            QStringList tokens = line.split(',');
            AppendRow(tokens);
        }
        file.close();
        BuildMetrics();
        return true;
    }
    qDebug() << "Unable to open CSV file <" << file_name << ">\n";

    return false;
}

bool Database::SaveData(const QString& file_name){
    Q_UNUSED(file_name);

    qDebug() << "CSV SaveData not impl'd\n";
    return false;
}

void Database::Clear(){
    data_.clear();
}

bool Database::Empty(){
    return data_.empty();
}
