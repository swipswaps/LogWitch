/*
 * LogEntryParser_Logfile.cpp
 *
 *  Created on: May 13, 2011
 *      Author: sven
 */

#include "LogEntryParser_Logfile.h"

 #include <QRegExp>
#include "LogEntryFactory.h"
#include "LogData/LogEntryAttributeFactory.h"
#include "LogData/LogEntryAttributes.h"
#include <QtCore/QtCore>

LogEntryParser_Logfile::LogEntryParser_Logfile( const QString &filename)
	: m_abort(false )
	, logfile( filename )
	, logfileStreamReady( false )
	, lineMessageRegex( new QRegExp("^([\\d-]+\\s+[\\d\\,\\:]+)\\s+-\\s+(.*)\\s+-\\s+\\[(.*)\\]\\s+-\\s+(.*)$") )
	, cellRegex( "\\s+-\\s+" )
	, timeFormat( "yyyy-MM-dd HH:mm:ss,zzz" )
{
	lineMessageRegex->setMinimal(true);

	// PReparing attributes factory
	myFactory.getLogEntryAttributeFactory()->addField("Severity");
	myFactory.getLogEntryAttributeFactory()->addField("Component");
	myFactory.getLogEntryAttributeFactory()->addField("File source");
	myFactory.getLogEntryAttributeFactory()->disallowAddingFields();

}

LogEntryParser_Logfile::~LogEntryParser_Logfile()
{
	m_abort = true;
	wait();
}

void LogEntryParser_Logfile::startEmiting()
{
    if (!isRunning() && !m_abort )
        start(LowPriority);
}

boost::shared_ptr<const LogEntryAttributeFactory> LogEntryParser_Logfile::getLogEntryAttributeFactory() const
{
	return myFactory.getLogEntryAttributeFactory();
}

void LogEntryParser_Logfile::init()
{
    if (!logfile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    logfileStream.setDevice( &logfile );
    logfileStreamReady = true;
}

void LogEntryParser_Logfile::run()
{
	init();

	int i = 0;

	forever
	{
		if ( m_abort)
			return;
		TSharedLogEntry entry( getNextLogEntry() );

        if( entry )
        	emit newEntry( entry );
        else
        	m_abort = true;

        //if( i++ > 35 )
        //	return;
        i++;
        qDebug() << "Entry: " << i;
	}
}

TSharedLogEntry LogEntryParser_Logfile::getNextLogEntry()
{
	TSharedLogEntry entryReturn;

	if( logfileStreamReady )
	{
		bool entryComplete = false;

		while( !entryComplete  )
		{
			if( stashedLine.isEmpty() && !logfileStream.atEnd() )
				stashedLine = logfileStream.readLine();

			if( logfileStream.atEnd() && stashedLine.isEmpty()  )
			{
				// End of logfile
				entryComplete = true;
			}
			else
			{
				//qDebug() << "StashedLine = " << stashedLine;
				int idx = -1;
				if( (idx = lineMessageRegex->indexIn( stashedLine )) != -1 )
				{
					if( entry ) // first entry
					{
						//qDebug() << "Appending Message to last entry = " << message;

						entry->setMessage( message );
						entryComplete = true;
						entryReturn = entry;
					}

					QDateTime timestamp( QDateTime::fromString ( lineMessageRegex->cap(1), timeFormat ) );

					entry = myFactory.generateLogEntry( timestamp );
					// File Source
					entry->getAttributes().setAttribute( boost::shared_ptr<QString>(new QString(lineMessageRegex->cap(3)) ), 2 );
					message = lineMessageRegex->cap(4);

					QStringList lst = lineMessageRegex->cap(2).split( cellRegex );
					if( lst.size() >= 2 )
					{
						// Severity
						entry->getAttributes().setAttribute( boost::shared_ptr<QString>(new QString(lst[0]) ), 0 );
						// Component
						entry->getAttributes().setAttribute( boost::shared_ptr<QString>(new QString(lst[1]) ), 1 );
					}

					/*
					qDebug() << "Entry detected: timestamp = "<< timestamp
							<< " 0 " << lineMessageRegex->cap(0)
							<< " 1 " << lineMessageRegex->cap(1)
							<< " 2 " << lineMessageRegex->cap(2)
							<< " 3 " << lineMessageRegex->cap(3)
							<< " 4 " << lineMessageRegex->cap(4)
							<< " 5 " << lineMessageRegex->cap(5)
							<< " 6 " << lineMessageRegex->cap(6)
							<< " IDX: " << idx;
							*/

				}
				else
				{
					//qDebug() << " appending stashed line to message";
					message += stashedLine;
				}

				stashedLine = "";
			}
		}
    }

    return entryReturn;
}
