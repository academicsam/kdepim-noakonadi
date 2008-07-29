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

#ifndef _INFORMATIONELEMENT_H_
#define _INFORMATIONELEMENT_H_

namespace Nepomuk {
        class DataSource;
        class InformationElement;
        class Contact;
        class DataObject;
}

#include <nepomuk/resource.h>
#include <nepomuk/nepomuk_export.h>

namespace Nepomuk {

/**
 * A unit of content the user works with. This is a superclass for 
 * all interpretations of a DataObject. 
 */
    class NEPOMUK_EXPORT InformationElement : public Resource
    {
    public:
        /**
         * Create a new empty and invalid InformationElement instance
         */
        InformationElement();
        /**
         * Default copy constructor
         */
        InformationElement( const InformationElement& );
        InformationElement( const Resource& );
        /**
         * Create a new InformationElement instance representing the resource
         * referenced by \a uriOrIdentifier.
         */
        InformationElement( const QString& uriOrIdentifier );
        /**
         * Create a new InformationElement instance representing the resource
         * referenced by \a uri.
         */
        InformationElement( const QUrl& uri );
        ~InformationElement();

        InformationElement& operator=( const InformationElement& );

            /**
             * Get property 'characterSet'. Characterset in which the content 
             * of the InformationElement was created. Example: ISO-8859-1, 
             * UTF-8. One of the registered character sets at http://www.iana.org/assignments/character-sets. 
             * This characterSet is used to interpret any textual parts of 
             * the content. If more than one characterSet is used within one 
             * data object, use more specific properties. 
             */
            QStringList characterSets() const;

            /**
             * Set property 'characterSet'. Characterset in which the content 
             * of the InformationElement was created. Example: ISO-8859-1, 
             * UTF-8. One of the registered character sets at http://www.iana.org/assignments/character-sets. 
             * This characterSet is used to interpret any textual parts of 
             * the content. If more than one characterSet is used within one 
             * data object, use more specific properties. 
             */
            void setCharacterSets( const QStringList& value );

            /**
             * Add a value to property 'characterSet'. Characterset in which 
             * the content of the InformationElement was created. Example: 
             * ISO-8859-1, UTF-8. One of the registered character sets at 
             * http://www.iana.org/assignments/character-sets. This 
             * characterSet is used to interpret any textual parts of the content. 
             * If more than one characterSet is used within one data object, 
             * use more specific properties. 
             */
            void addCharacterSet( const QString& value );

            /**
             * \return The URI of the property 'characterSet'. 
             */
            static QUrl characterSetUri();

            /**
             * Get property 'informationElementDate'. A point or period 
             * of time associated with an event in the lifecycle of an Information 
             * Element. A common superproperty for all date-related properties 
             * of InformationElements in the NIE Framework. 
             */
            QList<QDateTime> informationElementDates() const;

            /**
             * Set property 'informationElementDate'. A point or period 
             * of time associated with an event in the lifecycle of an Information 
             * Element. A common superproperty for all date-related properties 
             * of InformationElements in the NIE Framework. 
             */
            void setInformationElementDates( const QList<QDateTime>& value );

            /**
             * Add a value to property 'informationElementDate'. A point 
             * or period of time associated with an event in the lifecycle of 
             * an Information Element. A common superproperty for all date-related 
             * properties of InformationElements in the NIE Framework. 
             */
            void addInformationElementDate( const QDateTime& value );

            /**
             * \return The URI of the property 'informationElementDate'. 
             */
            static QUrl informationElementDateUri();

            /**
             * Get property 'rootElementOf'. DataObjects extracted from 
             * a single data source are organized into a containment tree. 
             * This property links the root of that tree with the datasource 
             * it has been extracted from 
             */
            QList<DataSource> rootElementOfs() const;

            /**
             * Set property 'rootElementOf'. DataObjects extracted from 
             * a single data source are organized into a containment tree. 
             * This property links the root of that tree with the datasource 
             * it has been extracted from 
             */
            void setRootElementOfs( const QList<DataSource>& value );

            /**
             * Add a value to property 'rootElementOf'. DataObjects extracted 
             * from a single data source are organized into a containment tree. 
             * This property links the root of that tree with the datasource 
             * it has been extracted from 
             */
            void addRootElementOf( const DataSource& value );

            /**
             * \return The URI of the property 'rootElementOf'. 
             */
            static QUrl rootElementOfUri();

            /**
             * Get property 'legal'. A common superproperty for all properties 
             * that point at legal information about an Information Element 
             */
            QStringList legals() const;

            /**
             * Set property 'legal'. A common superproperty for all properties 
             * that point at legal information about an Information Element 
             */
            void setLegals( const QStringList& value );

            /**
             * Add a value to property 'legal'. A common superproperty for 
             * all properties that point at legal information about an Information 
             * Element 
             */
            void addLegal( const QString& value );

            /**
             * \return The URI of the property 'legal'. 
             */
            static QUrl legalUri();

            /**
             * Get property 'isStoredAs'. Links the information element 
             * with the DataObject it is stored in. 
             */
            QList<DataObject> isStoredAses() const;

            /**
             * Set property 'isStoredAs'. Links the information element 
             * with the DataObject it is stored in. 
             */
            void setIsStoredAses( const QList<DataObject>& value );

            /**
             * Add a value to property 'isStoredAs'. Links the information 
             * element with the DataObject it is stored in. 
             */
            void addIsStoredAs( const DataObject& value );

            /**
             * \return The URI of the property 'isStoredAs'. 
             */
            static QUrl isStoredAsUri();

            /**
             * Get property 'language'. Language the InformationElement 
             * is expressed in. This property applies to the data object in 
             * its entirety. If the data object is divisible into parts expressed 
             * in multiple languages - more specific properties should be 
             * used. Users are encouraged to use the two-letter code specified 
             * in the RFC 3066 
             */
            QStringList languages() const;

            /**
             * Set property 'language'. Language the InformationElement 
             * is expressed in. This property applies to the data object in 
             * its entirety. If the data object is divisible into parts expressed 
             * in multiple languages - more specific properties should be 
             * used. Users are encouraged to use the two-letter code specified 
             * in the RFC 3066 
             */
            void setLanguages( const QStringList& value );

            /**
             * Add a value to property 'language'. Language the InformationElement 
             * is expressed in. This property applies to the data object in 
             * its entirety. If the data object is divisible into parts expressed 
             * in multiple languages - more specific properties should be 
             * used. Users are encouraged to use the two-letter code specified 
             * in the RFC 3066 
             */
            void addLanguage( const QString& value );

            /**
             * \return The URI of the property 'language'. 
             */
            static QUrl languageUri();

            /**
             * Get property 'mimeType'. The mime type of the resource, if available. 
             * Example: "text/plain". See http://www.iana.org/assignments/media-types/. 
             * This property applies to data objects that can be described 
             * with one mime type. In cases where the object as a whole has one 
             * mime type, while it's parts have other mime types, or there is 
             * no mime type that can be applied to the object as a whole, but some 
             * parts of the content have mime types - use more specific properties. 
             */
            QStringList mimeTypes() const;

            /**
             * Set property 'mimeType'. The mime type of the resource, if available. 
             * Example: "text/plain". See http://www.iana.org/assignments/media-types/. 
             * This property applies to data objects that can be described 
             * with one mime type. In cases where the object as a whole has one 
             * mime type, while it's parts have other mime types, or there is 
             * no mime type that can be applied to the object as a whole, but some 
             * parts of the content have mime types - use more specific properties. 
             */
            void setMimeTypes( const QStringList& value );

            /**
             * Add a value to property 'mimeType'. The mime type of the resource, 
             * if available. Example: "text/plain". See http://www.iana.org/assignments/media-types/. 
             * This property applies to data objects that can be described 
             * with one mime type. In cases where the object as a whole has one 
             * mime type, while it's parts have other mime types, or there is 
             * no mime type that can be applied to the object as a whole, but some 
             * parts of the content have mime types - use more specific properties. 
             */
            void addMimeType( const QString& value );

            /**
             * \return The URI of the property 'mimeType'. 
             */
            static QUrl mimeTypeUri();

            /**
             * Get property 'version'. The current version of the given data 
             * object. Exact semantics is unspecified at this level. Use more 
             * specific subproperties if needed. 
             */
            QStringList versions() const;

            /**
             * Set property 'version'. The current version of the given data 
             * object. Exact semantics is unspecified at this level. Use more 
             * specific subproperties if needed. 
             */
            void setVersions( const QStringList& value );

            /**
             * Add a value to property 'version'. The current version of the 
             * given data object. Exact semantics is unspecified at this level. 
             * Use more specific subproperties if needed. 
             */
            void addVersion( const QString& value );

            /**
             * \return The URI of the property 'version'. 
             */
            static QUrl versionUri();

            /**
             * Get property 'links'. A linking relation. A piece of content 
             * links/mentions a piece of data 
             */
            QList<DataObject> linkses() const;

            /**
             * Set property 'links'. A linking relation. A piece of content 
             * links/mentions a piece of data 
             */
            void setLinkses( const QList<DataObject>& value );

            /**
             * Add a value to property 'links'. A linking relation. A piece 
             * of content links/mentions a piece of data 
             */
            void addLinks( const DataObject& value );

            /**
             * \return The URI of the property 'links'. 
             */
            static QUrl linksUri();

            /**
             * Get property 'generator'. Software used to "generate" the 
             * contents. E.g. a word processor name. 
             */
            QStringList generators() const;

            /**
             * Set property 'generator'. Software used to "generate" the 
             * contents. E.g. a word processor name. 
             */
            void setGenerators( const QStringList& value );

            /**
             * Add a value to property 'generator'. Software used to "generate" 
             * the contents. E.g. a word processor name. 
             */
            void addGenerator( const QString& value );

            /**
             * \return The URI of the property 'generator'. 
             */
            static QUrl generatorUri();

            /**
             * Get property 'description'. A textual description of the resource. 
             * This property may be used for any metadata fields that provide 
             * some meta-information or comment about a resource in the form 
             * of a passage of text. This property is not to be confused with 
             * nie:plainTextContent. Use more specific subproperties wherever 
             * possible. 
             */
            QStringList descriptions() const;

            /**
             * Set property 'description'. A textual description of the resource. 
             * This property may be used for any metadata fields that provide 
             * some meta-information or comment about a resource in the form 
             * of a passage of text. This property is not to be confused with 
             * nie:plainTextContent. Use more specific subproperties wherever 
             * possible. 
             */
            void setDescriptions( const QStringList& value );

            /**
             * Add a value to property 'description'. A textual description 
             * of the resource. This property may be used for any metadata fields 
             * that provide some meta-information or comment about a resource 
             * in the form of a passage of text. This property is not to be confused 
             * with nie:plainTextContent. Use more specific subproperties 
             * wherever possible. 
             */
            void addDescription( const QString& value );

            /**
             * \return The URI of the property 'description'. 
             */
            static QUrl descriptionUri();

            /**
             * Get property 'contentCreated'. The date of the content creation. 
             * This may not necessarily be equal to the date when the DataObject 
             * (i.e. the physical representation) itself was created. Compare 
             * with nie:created property. 
             */
            QDateTime contentCreated() const;

            /**
             * Set property 'contentCreated'. The date of the content creation. 
             * This may not necessarily be equal to the date when the DataObject 
             * (i.e. the physical representation) itself was created. Compare 
             * with nie:created property. 
             */
            void setContentCreated( const QDateTime& value );

            /**
             * \return The URI of the property 'contentCreated'. 
             */
            static QUrl contentCreatedUri();

            /**
             * Get property 'title'. Name given to an InformationElement 
             */
            QStringList titles() const;

            /**
             * Set property 'title'. Name given to an InformationElement 
             */
            void setTitles( const QStringList& value );

            /**
             * Add a value to property 'title'. Name given to an InformationElement 
             */
            void addTitle( const QString& value );

            /**
             * \return The URI of the property 'title'. 
             */
            static QUrl titleUri();

            /**
             * Get property 'contentLastModified'. The date of the last modification 
             * of the content. 
             */
            QDateTime contentLastModified() const;

            /**
             * Set property 'contentLastModified'. The date of the last modification 
             * of the content. 
             */
            void setContentLastModified( const QDateTime& value );

            /**
             * \return The URI of the property 'contentLastModified'. 
             */
            static QUrl contentLastModifiedUri();

            /**
             * Get property 'keyword'. Adapted DublinCore: The topic of the 
             * content of the resource, as keyword. No sentences here. Recommended 
             * best practice is to select a value from a controlled vocabulary 
             * or formal classification scheme. 
             */
            QStringList keywords() const;

            /**
             * Set property 'keyword'. Adapted DublinCore: The topic of the 
             * content of the resource, as keyword. No sentences here. Recommended 
             * best practice is to select a value from a controlled vocabulary 
             * or formal classification scheme. 
             */
            void setKeywords( const QStringList& value );

            /**
             * Add a value to property 'keyword'. Adapted DublinCore: The 
             * topic of the content of the resource, as keyword. No sentences 
             * here. Recommended best practice is to select a value from a controlled 
             * vocabulary or formal classification scheme. 
             */
            void addKeyword( const QString& value );

            /**
             * \return The URI of the property 'keyword'. 
             */
            static QUrl keywordUri();

            /**
             * Get property 'isLogicalPartOf'. Generic property used to 
             * express 'logical' containment relationships between DataObjects. 
             * NIE extensions are encouraged to provide more specific subproperties 
             * of this one. It is advisable for actual instances of InformationElement 
             * to use those specific subproperties. Note the difference between 
             * 'physical' containment (isPartOf) and logical containment 
             * (isLogicalPartOf) 
             */
            QList<InformationElement> isLogicalPartOfs() const;

            /**
             * Set property 'isLogicalPartOf'. Generic property used to 
             * express 'logical' containment relationships between DataObjects. 
             * NIE extensions are encouraged to provide more specific subproperties 
             * of this one. It is advisable for actual instances of InformationElement 
             * to use those specific subproperties. Note the difference between 
             * 'physical' containment (isPartOf) and logical containment 
             * (isLogicalPartOf) 
             */
            void setIsLogicalPartOfs( const QList<InformationElement>& value );

            /**
             * Add a value to property 'isLogicalPartOf'. Generic property 
             * used to express 'logical' containment relationships between 
             * DataObjects. NIE extensions are encouraged to provide more 
             * specific subproperties of this one. It is advisable for actual 
             * instances of InformationElement to use those specific subproperties. 
             * Note the difference between 'physical' containment (isPartOf) 
             * and logical containment (isLogicalPartOf) 
             */
            void addIsLogicalPartOf( const InformationElement& value );

            /**
             * \return The URI of the property 'isLogicalPartOf'. 
             */
            static QUrl isLogicalPartOfUri();

            /**
             * Get property 'identifier'. An unambiguous reference to the 
             * InformationElement within a given context. Recommended best 
             * practice is to identify the resource by means of a string conforming 
             * to a formal identification system. 
             */
            QStringList identifiers() const;

            /**
             * Set property 'identifier'. An unambiguous reference to the 
             * InformationElement within a given context. Recommended best 
             * practice is to identify the resource by means of a string conforming 
             * to a formal identification system. 
             */
            void setIdentifiers( const QStringList& value );

            /**
             * Add a value to property 'identifier'. An unambiguous reference 
             * to the InformationElement within a given context. Recommended 
             * best practice is to identify the resource by means of a string 
             * conforming to a formal identification system. 
             */
            void addIdentifier( const QString& value );

            /**
             * \return The URI of the property 'identifier'. 
             */
            static QUrl identifierUri();

            /**
             * Get property 'plainTextContent'. Plain-text representation 
             * of the content of a InformationElement with all markup removed. 
             * The main purpose of this property is full-text indexing and 
             * search. Its exact content is considered application-specific. 
             * The user can make no assumptions about what is and what is not 
             * contained within. Applications should use more specific properties 
             * wherever possible. 
             */
            QStringList plainTextContents() const;

            /**
             * Set property 'plainTextContent'. Plain-text representation 
             * of the content of a InformationElement with all markup removed. 
             * The main purpose of this property is full-text indexing and 
             * search. Its exact content is considered application-specific. 
             * The user can make no assumptions about what is and what is not 
             * contained within. Applications should use more specific properties 
             * wherever possible. 
             */
            void setPlainTextContents( const QStringList& value );

            /**
             * Add a value to property 'plainTextContent'. Plain-text representation 
             * of the content of a InformationElement with all markup removed. 
             * The main purpose of this property is full-text indexing and 
             * search. Its exact content is considered application-specific. 
             * The user can make no assumptions about what is and what is not 
             * contained within. Applications should use more specific properties 
             * wherever possible. 
             */
            void addPlainTextContent( const QString& value );

            /**
             * \return The URI of the property 'plainTextContent'. 
             */
            static QUrl plainTextContentUri();

            /**
             * Get property 'relatedTo'. A common superproperty for all relations 
             * between a piece of content and other pieces of data (which may 
             * be interpreted as other pieces of content). 
             */
            QList<DataObject> relatedTos() const;

            /**
             * Set property 'relatedTo'. A common superproperty for all relations 
             * between a piece of content and other pieces of data (which may 
             * be interpreted as other pieces of content). 
             */
            void setRelatedTos( const QList<DataObject>& value );

            /**
             * Add a value to property 'relatedTo'. A common superproperty 
             * for all relations between a piece of content and other pieces 
             * of data (which may be interpreted as other pieces of content). 
             */
            void addRelatedTo( const DataObject& value );

            /**
             * \return The URI of the property 'relatedTo'. 
             */
            static QUrl relatedToUri();

            /**
             * Get property 'comment'. A user comment about an InformationElement. 
             */
            QStringList comments() const;

            /**
             * Set property 'comment'. A user comment about an InformationElement. 
             */
            void setComments( const QStringList& value );

            /**
             * Add a value to property 'comment'. A user comment about an InformationElement. 
             */
            void addComment( const QString& value );

            /**
             * \return The URI of the property 'comment'. 
             */
            static QUrl commentUri();

            /**
             * Get property 'contentSize'. The size of the content. This property 
             * can be used whenever the size of the content of an InformationElement 
             * differs from the size of the DataObject. (e.g. because of compression, 
             * encoding, encryption or any other representation issues). 
             * The contentSize in expressed in bytes. 
             */
            QList<qint64> contentSizes() const;

            /**
             * Set property 'contentSize'. The size of the content. This property 
             * can be used whenever the size of the content of an InformationElement 
             * differs from the size of the DataObject. (e.g. because of compression, 
             * encoding, encryption or any other representation issues). 
             * The contentSize in expressed in bytes. 
             */
            void setContentSizes( const QList<qint64>& value );

            /**
             * Add a value to property 'contentSize'. The size of the content. 
             * This property can be used whenever the size of the content of 
             * an InformationElement differs from the size of the DataObject. 
             * (e.g. because of compression, encoding, encryption or any 
             * other representation issues). The contentSize in expressed 
             * in bytes. 
             */
            void addContentSize( const qint64& value );

            /**
             * \return The URI of the property 'contentSize'. 
             */
            static QUrl contentSizeUri();

            /**
             * Get property 'subject'. An overall topic of the content of a 
             * InformationElement 
             */
            QStringList subjects() const;

            /**
             * Set property 'subject'. An overall topic of the content of a 
             * InformationElement 
             */
            void setSubjects( const QStringList& value );

            /**
             * Add a value to property 'subject'. An overall topic of the content 
             * of a InformationElement 
             */
            void addSubject( const QString& value );

            /**
             * \return The URI of the property 'subject'. 
             */
            static QUrl subjectUri();

            /**
             * Get property 'Part'. Generic property used to express 'physical' 
             * containment relationships between DataObjects. NIE extensions 
             * are encouraged to provide more specific subproperties of this 
             * one. It is advisable for actual instances of DataObjects to 
             * use those specific subproperties. Note to the developers: 
             * Please be aware of the distinction between containment relation 
             * and provenance. The hasPart relation models physical containment, 
             * an InformationElement (a nmo:Message) can have a 'physical' 
             * part (an nfo:Attachment). Also, please note the difference 
             * between physical containment (hasPart) and logical containment 
             * (hasLogicalPart) the former has more strict meaning. They 
             * may occur independently of each other. 
             */
            QList<DataObject> parts() const;

            /**
             * Set property 'Part'. Generic property used to express 'physical' 
             * containment relationships between DataObjects. NIE extensions 
             * are encouraged to provide more specific subproperties of this 
             * one. It is advisable for actual instances of DataObjects to 
             * use those specific subproperties. Note to the developers: 
             * Please be aware of the distinction between containment relation 
             * and provenance. The hasPart relation models physical containment, 
             * an InformationElement (a nmo:Message) can have a 'physical' 
             * part (an nfo:Attachment). Also, please note the difference 
             * between physical containment (hasPart) and logical containment 
             * (hasLogicalPart) the former has more strict meaning. They 
             * may occur independently of each other. 
             */
            void setParts( const QList<DataObject>& value );

            /**
             * Add a value to property 'Part'. Generic property used to express 
             * 'physical' containment relationships between DataObjects. 
             * NIE extensions are encouraged to provide more specific subproperties 
             * of this one. It is advisable for actual instances of DataObjects 
             * to use those specific subproperties. Note to the developers: 
             * Please be aware of the distinction between containment relation 
             * and provenance. The hasPart relation models physical containment, 
             * an InformationElement (a nmo:Message) can have a 'physical' 
             * part (an nfo:Attachment). Also, please note the difference 
             * between physical containment (hasPart) and logical containment 
             * (hasLogicalPart) the former has more strict meaning. They 
             * may occur independently of each other. 
             */
            void addPart( const DataObject& value );

            /**
             * \return The URI of the property 'Part'. 
             */
            static QUrl partUri();

            /**
             * Get property 'LogicalPart'. Generic property used to express 
             * 'logical' containment relationships between DataObjects. 
             * NIE extensions are encouraged to provide more specific subproperties 
             * of this one. It is advisable for actual instances of InformationElement 
             * to use those specific subproperties. Note the difference between 
             * 'physical' containment (hasPart) and logical containment 
             * (hasLogicalPart) 
             */
            QList<InformationElement> logicalParts() const;

            /**
             * Set property 'LogicalPart'. Generic property used to express 
             * 'logical' containment relationships between DataObjects. 
             * NIE extensions are encouraged to provide more specific subproperties 
             * of this one. It is advisable for actual instances of InformationElement 
             * to use those specific subproperties. Note the difference between 
             * 'physical' containment (hasPart) and logical containment 
             * (hasLogicalPart) 
             */
            void setLogicalParts( const QList<InformationElement>& value );

            /**
             * Add a value to property 'LogicalPart'. Generic property used 
             * to express 'logical' containment relationships between DataObjects. 
             * NIE extensions are encouraged to provide more specific subproperties 
             * of this one. It is advisable for actual instances of InformationElement 
             * to use those specific subproperties. Note the difference between 
             * 'physical' containment (hasPart) and logical containment 
             * (hasLogicalPart) 
             */
            void addLogicalPart( const InformationElement& value );

            /**
             * \return The URI of the property 'LogicalPart'. 
             */
            static QUrl logicalPartUri();

            /**
             * Get property 'creator'. Creator of a data object, an entity 
             * primarily responsible for the creation of the content of the 
             * data object. 
             */
            QList<Contact> creators() const;

            /**
             * Set property 'creator'. Creator of a data object, an entity 
             * primarily responsible for the creation of the content of the 
             * data object. 
             */
            void setCreators( const QList<Contact>& value );

            /**
             * Add a value to property 'creator'. Creator of a data object, 
             * an entity primarily responsible for the creation of the content 
             * of the data object. 
             */
            void addCreator( const Contact& value );

            /**
             * \return The URI of the property 'creator'. 
             */
            static QUrl creatorUri();

            /**
             * Get property 'contributor'. An entity responsible for making 
             * contributions to the content of the InformationElement. 
             */
            QList<Contact> contributors() const;

            /**
             * Set property 'contributor'. An entity responsible for making 
             * contributions to the content of the InformationElement. 
             */
            void setContributors( const QList<Contact>& value );

            /**
             * Add a value to property 'contributor'. An entity responsible 
             * for making contributions to the content of the InformationElement. 
             */
            void addContributor( const Contact& value );

            /**
             * \return The URI of the property 'contributor'. 
             */
            static QUrl contributorUri();

            /**
             * Get property 'publisher'. An entity responsible for making 
             * the InformationElement available. 
             */
            QList<Contact> publishers() const;

            /**
             * Set property 'publisher'. An entity responsible for making 
             * the InformationElement available. 
             */
            void setPublishers( const QList<Contact>& value );

            /**
             * Add a value to property 'publisher'. An entity responsible 
             * for making the InformationElement available. 
             */
            void addPublisher( const Contact& value );

            /**
             * \return The URI of the property 'publisher'. 
             */
            static QUrl publisherUri();

            /**
             * Retrieve a list of all available InformationElement resources. 
             * This list consists of all resource of type InformationElement 
             * that are stored in the local Nepomuk meta data storage and any 
             * changes made locally. Be aware that in some cases this list can 
             * get very big. Then it might be better to use libKNep directly. 
             */
            static QList<InformationElement> allInformationElements();


        /**
         * \return The URI of the resource type that is used in InformationElement instances.
         */
        static QString resourceTypeUri();

    protected:
       InformationElement( const QString& uri, const QUrl& type );
       InformationElement( const QUrl& uri, const QUrl& type );
   };
}

#endif
