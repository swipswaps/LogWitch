/*
 * LogEntryTableModel.h
 *
 *  Created on: May 14, 2011
 *      Author: sven
 */

#ifndef LOGENTRYTABLEMODEL_H_
#define LOGENTRYTABLEMODEL_H_

#include <boost/shared_ptr.hpp>

#include <QAbstractTableModel>
#include <QtCore/QtCore>
#include "LogData/LogEntry.h"

class LogEntryParser;
class LogEntryParserModelConfiguration;

class LogEntryTableModel: public QAbstractTableModel
{
	Q_OBJECT

public:
	LogEntryTableModel( boost::shared_ptr<LogEntryParser> parser );
	virtual ~LogEntryTableModel();

	QString getModelName() const { return m_ModelName; };

	void startModel() ;

    int rowCount(const QModelIndex &parent) const;

    int columnCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    TconstSharedLogEntry getEntryByIndex( const QModelIndex &index ) const;

    boost::shared_ptr<const LogEntryParserModelConfiguration> getParserModelConfiguration() const;

public slots:
	void insertEntry( TSharedLogEntry entry );

	void clearTable();

private:
    typedef std::vector<TconstSharedLogEntry> TLogEntryTable;

    TLogEntryTable m_table;

    boost::shared_ptr<const LogEntryParserModelConfiguration> m_modelConfiguration;

    QString m_dateTimeConversionString;


    boost::shared_ptr<LogEntryParser> m_entryLoader;

    static const int m_fixedFirstColumns = 3;

    QString m_ModelName;
};

#endif /* LOGENTRYTABLEMODEL_H_ */
