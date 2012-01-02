/*
 * ActionColorRow.h
 *
 *  Created on: Jun 5, 2011
 *      Author: sven
 */

#ifndef ACTIONDATAREWRITER_H_
#define ACTIONDATAREWRITER_H_
#include <map>

#include <QtCore/QVariant>

#include "ActionRules/Action.h"
#include "LogData/LogEntryParserModelConfiguration.h"

class ActionDataRewriter
    : public Action
{
public:
    ActionDataRewriter( TSharedConstLogEntryParserModelConfiguration configuration );
    ~ActionDataRewriter();

    bool modifyData( QVariant &var,  int column, int role) const;

    void addChangeSet( const QVariant &var, int role, const QString &column );

    void addChangeSet( const QVariant &var, int role );

    QVariant toDisplay( int role ) const;
private:
    class rc_key
    {
    public:
        rc_key(int role, int column = -1)
        : role( role )
        , column( column )
        { }

        int role;
        int column;

        bool operator<(const rc_key& k) const
        {
            return role < k.role || ( role == k.role && column < k.column );
        }
    };

    typedef std::map<rc_key, QVariant> TChangeSet;

    TChangeSet m_changes;

    TSharedConstLogEntryParserModelConfiguration m_cfg;
};

typedef boost::shared_ptr<ActionDataRewriter> TSharedActionDataRewriter;
typedef boost::shared_ptr<const ActionDataRewriter> TconstSharedActionDataRewriter;

#endif /* ACTIONCOLORROW_H_ */
