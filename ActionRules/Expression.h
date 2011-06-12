/*
 * Expression.h
 *
 *  Created on: Jun 5, 2011
 *      Author: sven
 */

#ifndef EXPRESSION_H_
#define EXPRESSION_H_
#include "LogData/LogEntry.h"

class Expression
{
public:
    virtual ~Expression();

    virtual bool isValid( ) const = 0;

    virtual bool match( TconstSharedLogEntry &entry ) const = 0;

    virtual std::ostream &out( std::ostream &o, bool extended = false ) const = 0;
};

std::ostream& operator<< (std::ostream &o, const Expression &e);

typedef boost::shared_ptr<Expression> TSharedExpression;
typedef boost::shared_ptr<const Expression> TconstSharedExpression;


#endif /* EXPRESSION_H_ */
