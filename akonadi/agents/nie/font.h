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

#ifndef _FONT_H_
#define _FONT_H_

namespace Nepomuk {
        class Contact;
}

#include "informationelement.h"
#include <nepomuk/nepomuk_export.h>

namespace Nepomuk {

/**
 * A font. 
 */
    class NEPOMUK_EXPORT Font : public InformationElement
    {
    public:
        /**
         * Create a new empty and invalid Font instance
         */
        Font();
        /**
         * Default copy constructor
         */
        Font( const Font& );
        Font( const Resource& );
        /**
         * Create a new Font instance representing the resource
         * referenced by \a uriOrIdentifier.
         */
        Font( const QString& uriOrIdentifier );
        /**
         * Create a new Font instance representing the resource
         * referenced by \a uri.
         */
        Font( const QUrl& uri );
        ~Font();

        Font& operator=( const Font& );

            /**
             * Get property 'fontFamily'. The name of the font family. 
             */
            QStringList fontFamilys() const;

            /**
             * Set property 'fontFamily'. The name of the font family. 
             */
            void setFontFamilys( const QStringList& value );

            /**
             * Add a value to property 'fontFamily'. The name of the font family. 
             */
            void addFontFamily( const QString& value );

            /**
             * \return The URI of the property 'fontFamily'. 
             */
            static QUrl fontFamilyUri();

            /**
             * Get property 'foundry'. The foundry, the organization that 
             * created the font. 
             */
            QList<Contact> foundrys() const;

            /**
             * Set property 'foundry'. The foundry, the organization that 
             * created the font. 
             */
            void setFoundrys( const QList<Contact>& value );

            /**
             * Add a value to property 'foundry'. The foundry, the organization 
             * that created the font. 
             */
            void addFoundry( const Contact& value );

            /**
             * \return The URI of the property 'foundry'. 
             */
            static QUrl foundryUri();

            /**
             * Retrieve a list of all available Font resources. This list consists 
             * of all resource of type Font that are stored in the local Nepomuk 
             * meta data storage and any changes made locally. Be aware that 
             * in some cases this list can get very big. Then it might be better 
             * to use libKNep directly. 
             */
            static QList<Font> allFonts();


        /**
         * \return The URI of the resource type that is used in Font instances.
         */
        static QString resourceTypeUri();

    protected:
       Font( const QString& uri, const QUrl& type );
       Font( const QUrl& uri, const QUrl& type );
   };
}

#endif
