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

#ifndef _CONTACT_H_
#define _CONTACT_H_

namespace Nepomuk {
        class InformationElement;
        class ContactGroup;
        class Contact;
        class Email;
        class DataObject;
        class Font;
        class Message;
        class FileDataObject;
}

#include "role.h"
#include <nepomuk/nepomuk_export.h>

namespace Nepomuk {

/**
 * A Contact. A piece of data that can provide means to identify 
 * or communicate with an entity. 
 */
    class NEPOMUK_EXPORT Contact : public Role
    {
    public:
        /**
         * Create a new empty and invalid Contact instance
         */
        Contact();
        /**
         * Default copy constructor
         */
        Contact( const Contact& );
        Contact( const Resource& );
        /**
         * Create a new Contact instance representing the resource
         * referenced by \a uriOrIdentifier.
         */
        Contact( const QString& uriOrIdentifier );
        /**
         * Create a new Contact instance representing the resource
         * referenced by \a uri.
         */
        Contact( const QUrl& uri );
        ~Contact();

        Contact& operator=( const Contact& );

            /**
             * Get property 'Location'. Geographical location of the contact. 
             * Inspired by the 'GEO' property specified in RFC 2426 Sec. 3.4.2 
             */
            QStringList locations() const;

            /**
             * Set property 'Location'. Geographical location of the contact. 
             * Inspired by the 'GEO' property specified in RFC 2426 Sec. 3.4.2 
             */
            void setLocations( const QStringList& value );

            /**
             * Add a value to property 'Location'. Geographical location 
             * of the contact. Inspired by the 'GEO' property specified in 
             * RFC 2426 Sec. 3.4.2 
             */
            void addLocation( const QString& value );

            /**
             * \return The URI of the property 'Location'. 
             */
            static QUrl locationUri();

            /**
             * Get property 'key'. An encryption key attached to a contact. 
             * Inspired by the KEY property defined in RFC 2426 sec. 3.7.2 
             */
            QList<DataObject> keys() const;

            /**
             * Set property 'key'. An encryption key attached to a contact. 
             * Inspired by the KEY property defined in RFC 2426 sec. 3.7.2 
             */
            void setKeys( const QList<DataObject>& value );

            /**
             * Add a value to property 'key'. An encryption key attached to 
             * a contact. Inspired by the KEY property defined in RFC 2426 sec. 
             * 3.7.2 
             */
            void addKey( const DataObject& value );

            /**
             * \return The URI of the property 'key'. 
             */
            static QUrl keyUri();

            /**
             * Get property 'sound'. Sound clip attached to a Contact. The 
             * DataObject refered to by this property is usually interpreted 
             * as an nfo:Audio. Inspired by the SOUND property defined in RFC 
             * 2425 sec. 3.6.6. 
             */
            QList<DataObject> sounds() const;

            /**
             * Set property 'sound'. Sound clip attached to a Contact. The 
             * DataObject refered to by this property is usually interpreted 
             * as an nfo:Audio. Inspired by the SOUND property defined in RFC 
             * 2425 sec. 3.6.6. 
             */
            void setSounds( const QList<DataObject>& value );

            /**
             * Add a value to property 'sound'. Sound clip attached to a Contact. 
             * The DataObject refered to by this property is usually interpreted 
             * as an nfo:Audio. Inspired by the SOUND property defined in RFC 
             * 2425 sec. 3.6.6. 
             */
            void addSound( const DataObject& value );

            /**
             * \return The URI of the property 'sound'. 
             */
            static QUrl soundUri();

            /**
             * Get property 'photo'. Photograph attached to a Contact. The 
             * DataObject refered to by this property is usually interpreted 
             * as an nfo:Image. Inspired by the PHOTO property defined in RFC 
             * 2426 sec. 3.1.4 
             */
            QList<DataObject> photos() const;

            /**
             * Set property 'photo'. Photograph attached to a Contact. The 
             * DataObject refered to by this property is usually interpreted 
             * as an nfo:Image. Inspired by the PHOTO property defined in RFC 
             * 2426 sec. 3.1.4 
             */
            void setPhotos( const QList<DataObject>& value );

            /**
             * Add a value to property 'photo'. Photograph attached to a Contact. 
             * The DataObject refered to by this property is usually interpreted 
             * as an nfo:Image. Inspired by the PHOTO property defined in RFC 
             * 2426 sec. 3.1.4 
             */
            void addPhoto( const DataObject& value );

            /**
             * \return The URI of the property 'photo'. 
             */
            static QUrl photoUri();

            /**
             * Get property 'belongsToGroup'. Links a Contact with a ContactGroup 
             * it belongs to. 
             */
            QList<ContactGroup> belongsToGroups() const;

            /**
             * Set property 'belongsToGroup'. Links a Contact with a ContactGroup 
             * it belongs to. 
             */
            void setBelongsToGroups( const QList<ContactGroup>& value );

            /**
             * Add a value to property 'belongsToGroup'. Links a Contact with 
             * a ContactGroup it belongs to. 
             */
            void addBelongsToGroup( const ContactGroup& value );

            /**
             * \return The URI of the property 'belongsToGroup'. 
             */
            static QUrl belongsToGroupUri();

            /**
             * Get property 'representative'. An object that represent an 
             * object represented by this Contact. Usually this property 
             * is used to link a Contact to an organization, to a contact to the 
             * representative of this organization the user directly interacts 
             * with. An equivalent for the 'AGENT' property defined in RFC 
             * 2426 Sec. 3.5.4 
             */
            QList<Contact> representatives() const;

            /**
             * Set property 'representative'. An object that represent an 
             * object represented by this Contact. Usually this property 
             * is used to link a Contact to an organization, to a contact to the 
             * representative of this organization the user directly interacts 
             * with. An equivalent for the 'AGENT' property defined in RFC 
             * 2426 Sec. 3.5.4 
             */
            void setRepresentatives( const QList<Contact>& value );

            /**
             * Add a value to property 'representative'. An object that represent 
             * an object represented by this Contact. Usually this property 
             * is used to link a Contact to an organization, to a contact to the 
             * representative of this organization the user directly interacts 
             * with. An equivalent for the 'AGENT' property defined in RFC 
             * 2426 Sec. 3.5.4 
             */
            void addRepresentative( const Contact& value );

            /**
             * \return The URI of the property 'representative'. 
             */
            static QUrl representativeUri();

            /**
             * Get property 'note'. A note about the object represented by 
             * this Contact. An equivalent for the 'NOTE' property defined 
             * in RFC 2426 Sec. 3.6.2 
             */
            QStringList notes() const;

            /**
             * Set property 'note'. A note about the object represented by 
             * this Contact. An equivalent for the 'NOTE' property defined 
             * in RFC 2426 Sec. 3.6.2 
             */
            void setNotes( const QStringList& value );

            /**
             * Add a value to property 'note'. A note about the object represented 
             * by this Contact. An equivalent for the 'NOTE' property defined 
             * in RFC 2426 Sec. 3.6.2 
             */
            void addNote( const QString& value );

            /**
             * \return The URI of the property 'note'. 
             */
            static QUrl noteUri();

            /**
             * Get property 'nickname'. A nickname of the Object represented 
             * by this Contact. This is an equivalen of the 'NICKNAME' property 
             * as defined in RFC 2426 Sec. 3.1.3. 
             */
            QStringList nicknames() const;

            /**
             * Set property 'nickname'. A nickname of the Object represented 
             * by this Contact. This is an equivalen of the 'NICKNAME' property 
             * as defined in RFC 2426 Sec. 3.1.3. 
             */
            void setNicknames( const QStringList& value );

            /**
             * Add a value to property 'nickname'. A nickname of the Object 
             * represented by this Contact. This is an equivalen of the 'NICKNAME' 
             * property as defined in RFC 2426 Sec. 3.1.3. 
             */
            void addNickname( const QString& value );

            /**
             * \return The URI of the property 'nickname'. 
             */
            static QUrl nicknameUri();

            /**
             * Get property 'contactUID'. A value that represents a globally 
             * unique identifier corresponding to the individual or resource 
             * associated with the Contact. An equivalent of the 'UID' property 
             * defined in RFC 2426 Sec. 3.6.7 
             */
            QStringList contactUIDs() const;

            /**
             * Set property 'contactUID'. A value that represents a globally 
             * unique identifier corresponding to the individual or resource 
             * associated with the Contact. An equivalent of the 'UID' property 
             * defined in RFC 2426 Sec. 3.6.7 
             */
            void setContactUIDs( const QStringList& value );

            /**
             * Add a value to property 'contactUID'. A value that represents 
             * a globally unique identifier corresponding to the individual 
             * or resource associated with the Contact. An equivalent of the 
             * 'UID' property defined in RFC 2426 Sec. 3.6.7 
             */
            void addContactUID( const QString& value );

            /**
             * \return The URI of the property 'contactUID'. 
             */
            static QUrl contactUIDUri();

            /**
             * Get property 'fullname'. To specify the formatted text corresponding 
             * to the name of the object the Contact represents. An equivalent 
             * of the FN property as defined in RFC 2426 Sec. 3.1.1. 
             */
            QStringList fullnames() const;

            /**
             * Set property 'fullname'. To specify the formatted text corresponding 
             * to the name of the object the Contact represents. An equivalent 
             * of the FN property as defined in RFC 2426 Sec. 3.1.1. 
             */
            void setFullnames( const QStringList& value );

            /**
             * Add a value to property 'fullname'. To specify the formatted 
             * text corresponding to the name of the object the Contact represents. 
             * An equivalent of the FN property as defined in RFC 2426 Sec. 3.1.1. 
             */
            void addFullname( const QString& value );

            /**
             * \return The URI of the property 'fullname'. 
             */
            static QUrl fullnameUri();

            /**
             * Get property 'birthDate'. Birth date of the object represented 
             * by this Contact. An equivalent of the 'BDAY' property as defined 
             * in RFC 2426 Sec. 3.1.5. 
             */
            QDate birthDate() const;

            /**
             * Set property 'birthDate'. Birth date of the object represented 
             * by this Contact. An equivalent of the 'BDAY' property as defined 
             * in RFC 2426 Sec. 3.1.5. 
             */
            void setBirthDate( const QDate& value );

            /**
             * \return The URI of the property 'birthDate'. 
             */
            static QUrl birthDateUri();

            /**
             * Get all resources that have this resource set as property 'contributor'. 
             * An entity responsible for making contributions to the content 
             * of the InformationElement. \sa ResourceManager::allResourcesWithProperty 
             */
            QList<InformationElement> contributorOf() const;

            /**
             * Get all resources that have this resource set as property 'creator'. 
             * Creator of a data object, an entity primarily responsible for 
             * the creation of the content of the data object. \sa ResourceManager::allResourcesWithProperty 
             */
            QList<InformationElement> creatorOf() const;

            /**
             * Get all resources that have this resource set as property 'publisher'. 
             * An entity responsible for making the InformationElement available. 
             * \sa ResourceManager::allResourcesWithProperty 
             */
            QList<InformationElement> publisherOf() const;

            /**
             * Get all resources that have this resource set as property 'representative'. 
             * An object that represent an object represented by this Contact. 
             * Usually this property is used to link a Contact to an organization, 
             * to a contact to the representative of this organization the 
             * user directly interacts with. An equivalent for the 'AGENT' 
             * property defined in RFC 2426 Sec. 3.5.4 \sa ResourceManager::allResourcesWithProperty 
             */
            QList<Contact> representativeOf() const;

            /**
             * Get all resources that have this resource set as property 'bcc'. 
             * A Contact that is to receive a bcc of the email. A Bcc (blind carbon 
             * copy) is a copy of an email message sent to a recipient whose email 
             * address does not appear in the message. \sa ResourceManager::allResourcesWithProperty 
             */
            QList<Email> bccOf() const;

            /**
             * Get all resources that have this resource set as property 'cc'. 
             * A Contact that is to receive a cc of the email. A cc (carbon copy) 
             * is a copy of an email message whose recipient appears on the recipient 
             * list, so that all other recipients are aware of it. \sa ResourceManager::allResourcesWithProperty 
             */
            QList<Email> ccOf() const;

            /**
             * Get all resources that have this resource set as property 'from'. 
             * The sender of the message \sa ResourceManager::allResourcesWithProperty 
             */
            QList<Message> fromOf() const;

            /**
             * Get all resources that have this resource set as property 'primaryRecipient'. 
             * The primary intended recipient of a message. \sa ResourceManager::allResourcesWithProperty 
             */
            QList<Message> primaryRecipientOf() const;

            /**
             * Get all resources that have this resource set as property 'recipient'. 
             * A common superproperty for all properties that link a message 
             * with its recipients. Please don't use this property directly. 
             * \sa ResourceManager::allResourcesWithProperty 
             */
            QList<Message> recipientOf() const;

            /**
             * Get all resources that have this resource set as property 'replyTo'. 
             * An address where the reply should be sent. \sa ResourceManager::allResourcesWithProperty 
             */
            QList<Message> replyToOf() const;

            /**
             * Get all resources that have this resource set as property 'secondaryRecipient'. 
             * A superproperty for all "additional" recipients of a message. 
             * \sa ResourceManager::allResourcesWithProperty 
             */
            QList<Message> secondaryRecipientOf() const;

            /**
             * Get all resources that have this resource set as property 'sender'. 
             * The person or agent submitting the message to the network, if 
             * other from the one given with the nmo:from property. Defined 
             * in RFC 822 sec. 4.4.2 \sa ResourceManager::allResourcesWithProperty 
             */
            QList<Message> senderOf() const;

            /**
             * Get all resources that have this resource set as property 'to'. 
             * The primary intended recipient of an email. \sa ResourceManager::allResourcesWithProperty 
             */
            QList<Email> toOf() const;

            /**
             * Get all resources that have this resource set as property 'fileOwner'. 
             * The owner of the file as defined by the file system access rights 
             * feature. \sa ResourceManager::allResourcesWithProperty 
             */
            QList<FileDataObject> fileOwnerOf() const;

            /**
             * Get all resources that have this resource set as property 'foundry'. 
             * The foundry, the organization that created the font. \sa ResourceManager::allResourcesWithProperty 
             */
            QList<Font> foundryOf() const;

            /**
             * Nepomuk does not support multiple inheritance. Thus, to access 
             * properties from all parent classes helper methods like this 
             * are introduced. The object returned represents the exact same 
             * resource. 
             */
            InformationElement toInformationElement() const;

            /**
             * Retrieve a list of all available Contact resources. This list 
             * consists of all resource of type Contact that are stored in the 
             * local Nepomuk meta data storage and any changes made locally. 
             * Be aware that in some cases this list can get very big. Then it 
             * might be better to use libKNep directly. 
             */
            static QList<Contact> allContacts();


        /**
         * \return The URI of the resource type that is used in Contact instances.
         */
        static QString resourceTypeUri();

    protected:
       Contact( const QString& uri, const QUrl& type );
       Contact( const QUrl& uri, const QUrl& type );
   };
}

#endif
