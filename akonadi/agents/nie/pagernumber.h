/*
 *
 * $Id: $
 *
 * This file is part of the Nepomuk KDE project.
 * Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */

/*
 * This file has been generated by the Nepomuk Resource class generator.
 * DO NOT EDIT THIS FILE.
 * ANY CHANGES WILL BE LOST.
 */

#ifndef _PAGERNUMBER_H_
#define _PAGERNUMBER_H_

namespace Nepomuk {
}

#include "messagingnumber.h"
#include <nepomuk/nepomuk_export.h>

namespace Nepomuk {

/**
 * A pager phone number. Inspired by the (TYPE=pager) parameter 
 * of the TEL property as defined in RFC 2426 sec 3.3.1. 
 */
    class NEPOMUK_EXPORT PagerNumber : public MessagingNumber
    {
    public:
        /**
         * Create a new empty and invalid PagerNumber instance
         */
        PagerNumber();
        /**
         * Default copy constructor
         */
        PagerNumber( const PagerNumber& );
        PagerNumber( const Resource& );
        /**
         * Create a new PagerNumber instance representing the resource
         * referenced by \a uriOrIdentifier.
         */
        PagerNumber( const QString& uriOrIdentifier );
        /**
         * Create a new PagerNumber instance representing the resource
         * referenced by \a uri.
         */
        PagerNumber( const QUrl& uri );
        ~PagerNumber();

        PagerNumber& operator=( const PagerNumber& );

            /**
             * Retrieve a list of all available PagerNumber resources. This 
             * list consists of all resource of type PagerNumber that are stored 
             * in the local Nepomuk meta data storage and any changes made locally. 
             * Be aware that in some cases this list can get very big. Then it 
             * might be better to use libKNep directly. 
             */
            static QList<PagerNumber> allPagerNumbers();


        /**
         * \return The URI of the resource type that is used in PagerNumber instances.
         */
        static QString resourceTypeUri();

    protected:
       PagerNumber( const QString& uri, const QUrl& type );
       PagerNumber( const QUrl& uri, const QUrl& type );
   };
}

#endif
