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

#ifndef _DATASOURCE_H_
#define _DATASOURCE_H_

namespace Nepomuk {
        class InformationElement;
        class DataObject;
}

#include <nepomuk/resource.h>
#include <nepomuk/nepomuk_export.h>

namespace Nepomuk {

/**
 * A superclass for all entities from which DataObjects can be 
 * extracted. Each entity represents a native application or 
 * some other system that manages information that may be of interest 
 * to the user of the Semantic Desktop. Subclasses may include 
 * FileSystems, Mailboxes, Calendars, websites etc. The exact 
 * choice of subclasses and their properties is considered application-specific. 
 * Each data extraction application is supposed to provide it's 
 * own DataSource ontology. Such an ontology should contain supported 
 * data source types coupled with properties necessary for the 
 * application to gain access to the data sources. (paths, urls, 
 * passwords etc...) 
 */
    class NEPOMUK_EXPORT DataSource : public Resource
    {
    public:
        /**
         * Create a new empty and invalid DataSource instance
         */
        DataSource();
        /**
         * Default copy constructor
         */
        DataSource( const DataSource& );
        DataSource( const Resource& );
        /**
         * Create a new DataSource instance representing the resource
         * referenced by \a uriOrIdentifier.
         */
        DataSource( const QString& uriOrIdentifier );
        /**
         * Create a new DataSource instance representing the resource
         * referenced by \a uri.
         */
        DataSource( const QUrl& uri );
        ~DataSource();

        DataSource& operator=( const DataSource& );

            /**
             * Get all resources that have this resource set as property 'dataSource'. 
             * Marks the provenance of a DataObject, what source does a data 
             * object come from. \sa ResourceManager::allResourcesWithProperty 
             */
            QList<DataObject> dataSourceOf() const;

            /**
             * Get all resources that have this resource set as property 'rootElementOf'. 
             * DataObjects extracted from a single data source are organized 
             * into a containment tree. This property links the root of that 
             * tree with the datasource it has been extracted from \sa ResourceManager::allResourcesWithProperty 
             */
            QList<InformationElement> rootElementOfOf() const;

            /**
             * Retrieve a list of all available DataSource resources. This 
             * list consists of all resource of type DataSource that are stored 
             * in the local Nepomuk meta data storage and any changes made locally. 
             * Be aware that in some cases this list can get very big. Then it 
             * might be better to use libKNep directly. 
             */
            static QList<DataSource> allDataSources();


        /**
         * \return The URI of the resource type that is used in DataSource instances.
         */
        static QString resourceTypeUri();

    protected:
       DataSource( const QString& uri, const QUrl& type );
       DataSource( const QUrl& uri, const QUrl& type );
   };
}

#endif
