#ifndef PRINCIPALCOMPONENTANALYZER_H
#define PRINCIPALCOMPONENTANALYZER_H

#include "Database.h"

class PrincipalComponentAnalyzer : public QObject
{
    Q_OBJECT;
public:
    PrincipalComponentAnalyzer();

public:
    Database GeneratePCATable(const Database& input_db);
private:
    Database *db_;
    QThread *worker_;
};

#endif // PRINCIPALCOMPONENTANALYZER_H
