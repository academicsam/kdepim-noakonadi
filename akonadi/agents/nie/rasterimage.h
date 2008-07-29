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

#ifndef _RASTERIMAGE_H_
#define _RASTERIMAGE_H_

namespace Nepomuk {
}

#include "image.h"
#include <nepomuk/nepomuk_export.h>

namespace Nepomuk {

/**
 * A raster image. 
 */
    class NEPOMUK_EXPORT RasterImage : public Image
    {
    public:
        /**
         * Create a new empty and invalid RasterImage instance
         */
        RasterImage();
        /**
         * Default copy constructor
         */
        RasterImage( const RasterImage& );
        RasterImage( const Resource& );
        /**
         * Create a new RasterImage instance representing the resource
         * referenced by \a uriOrIdentifier.
         */
        RasterImage( const QString& uriOrIdentifier );
        /**
         * Create a new RasterImage instance representing the resource
         * referenced by \a uri.
         */
        RasterImage( const QUrl& uri );
        ~RasterImage();

        RasterImage& operator=( const RasterImage& );

            /**
             * Retrieve a list of all available RasterImage resources. This 
             * list consists of all resource of type RasterImage that are stored 
             * in the local Nepomuk meta data storage and any changes made locally. 
             * Be aware that in some cases this list can get very big. Then it 
             * might be better to use libKNep directly. 
             */
            static QList<RasterImage> allRasterImages();


        /**
         * \return The URI of the resource type that is used in RasterImage instances.
         */
        static QString resourceTypeUri();

    protected:
       RasterImage( const QString& uri, const QUrl& type );
       RasterImage( const QUrl& uri, const QUrl& type );
   };
}

#endif
