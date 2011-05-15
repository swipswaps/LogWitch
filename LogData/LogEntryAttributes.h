/*
 * LogEntryAttributes.h
 *
 *  Created on: May 14, 2011
 *      Author: sven
 */

#ifndef LOGENTRYATTRIBUTES_H_
#define LOGENTRYATTRIBUTES_H_
#include <boost/shared_ptr.hpp>
#include <vector>

class LogEntryAttributeFactory;
class QString;

class LogEntryAttributes {
public:
	virtual ~LogEntryAttributes();

	LogEntryAttributes( LogEntryAttributeFactory *factory, const std::vector<boost::shared_ptr<QString> > &defAttributes );

	void setAttribute( boost::shared_ptr<QString>, int idx );

	boost::shared_ptr<const QString> getAttribute( int idx ) const;

private:
	std::vector<boost::shared_ptr<QString> > attributes;

	LogEntryAttributeFactory *myFactory;
};

#endif /* LOGENTRYATTRIBUTES_H_ */