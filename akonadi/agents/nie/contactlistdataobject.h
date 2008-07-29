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

#ifndef _CONTACTLISTDATAOBJECT_H_
#define _CONTACTLISTDATAOBJECT_H_

namespace Nepomuk {
        class ContactList;
}

#include "dataobject.h"
#include <nepomuk/nepomuk_export.h>

namespace Nepomuk {

/**
 * An entity occuring on a contact list (usually interpreted as 
 * an nco:Contact) 
 */
    class NEPOMUK_EXPORT ContactListDataObject : public DataObject
    {
    public:
        /**
         * Create a new empty and invalid ContactListDataObject instance
         */
        ContactListDataObject();
        /**
         * Default copy constructor
         */
        ContactListDataObject( const ContactListDataObject& );
        ContactListDataObject( const Resource& );
        /**
         * Create a new ContactListDataObject instance representing the resource
         * referenced by \a uriOrIdentifier.
         */
        ContactListDataObject( const QString& uriOrIdentifier );
        /**
         * Create a new ContactListDataObject instance representing the resource
         * referenced by \a uri.
         */
        ContactListDataObject( const QUrl& uri );
        ~ContactListDataObject();

        ContactListDataObject& operator=( const ContactListDataObject& );

            /**
             * Get all resources that have this resource set as property 'containsContact'. 
             * A property used to group contacts into contact groups. This 
             * property was NOT defined in the VCARD standard. See documentation 
             * for the 'ContactList' class for details \sa ResourceManager::allResourcesWithProperty 
             */
            QList<ContactList> containsContactOf() const;

            /**
             * Retrieve a list of all available ContactListDataObject resources. 
             * This list consists of all resource of type ContactListDataObject 
             * that are stored in the local Nepomuk meta data storage and any 
             * changes made locally. Be aware that in some cases this list can 
             * get very big. Then it might be better to use libKNep directly. 
             */
            static QList<ContactListDataObject> allContactListDataObjects();


        /**
         * \return The URI of the resource type that is used in ContactListDataObject instances.
         */
        static QString resourceTypeUri();

    protected:
       ContactListDataObject( const QString& uri, const QUrl& type );
       ContactListDataObject( const QUrl& uri, const QUrl& type );
   };
}

#endif
