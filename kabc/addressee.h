/*** Warning! This file has been generated by the script makeaddressee ***/
#ifndef KABC_ADDRESSEE_H
#define KABC_ADDRESSEE_H
// $Id$

#include <qvaluelist.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qdatetime.h>

#include <ksharedptr.h>
#include <kurl.h>

#include "phonenumber.h"
#include "address.h"
#include "geo.h"
#include "timezone.h"

namespace KABC {

struct AddresseeData : public KShared
{
  QString uid;
  QString name;
  QString formattedName;
  QString familyName;
  QString givenName;
  QString additionalName;
  QString prefix;
  QString suffix;
  QString nickName;
  QDateTime birthday;
  QString mailer;
  TimeZone timeZone;
  Geo geo;
  QString title;
  QString role;
  QString organization;
  QString note;
  QString productId;
  QDateTime revision;
  QString sortString;
  KURL url;

  PhoneNumber::List phoneNumbers;
  Address::List addresses;
  QStringList emails;
  QStringList categories;
  QStringList custom;
};


class Addressee
{
  public:
    typedef QValueList<Addressee> List;

    Addressee();
    ~Addressee();

    Addressee( const Addressee & );
    Addressee &operator=( const Addressee & );

    bool isEmpty();

    void setUid( const QString &uid );
    QString uid() const;

    void setName( const QString &name );
    QString name() const;

    void setFormattedName( const QString &formattedName );
    QString formattedName() const;

    void setFamilyName( const QString &familyName );
    QString familyName() const;

    void setGivenName( const QString &givenName );
    QString givenName() const;

    void setAdditionalName( const QString &additionalName );
    QString additionalName() const;

    void setPrefix( const QString &prefix );
    QString prefix() const;

    void setSuffix( const QString &suffix );
    QString suffix() const;

    void setNickName( const QString &nickName );
    QString nickName() const;

    void setBirthday( const QDateTime &birthday );
    QDateTime birthday() const;

    void setMailer( const QString &mailer );
    QString mailer() const;

    void setTimeZone( const TimeZone &timeZone );
    TimeZone timeZone() const;

    void setGeo( const Geo &geo );
    Geo geo() const;

    void setTitle( const QString &title );
    QString title() const;

    void setRole( const QString &role );
    QString role() const;

    void setOrganization( const QString &organization );
    QString organization() const;

    void setNote( const QString &note );
    QString note() const;

    void setProductId( const QString &productId );
    QString productId() const;

    void setRevision( const QDateTime &revision );
    QDateTime revision() const;

    void setSortString( const QString &sortString );
    QString sortString() const;

    void setUrl( const KURL &url );
    KURL url() const;

    QString realName() const;
    
    void insertEmail( const QString &email, bool prefered=false );
    void removeEmail( const QString &email );
    QString preferredEmail() const;
    QStringList emails() const;
    
    void insertPhoneNumber( const PhoneNumber &phoneNumber );
    void removePhoneNumber( const PhoneNumber &phoneNumber );
    PhoneNumber phoneNumber( int type ) const;
    PhoneNumber::List phoneNumbers() const;
    PhoneNumber findPhoneNumber( const QString &id ) const;
    
    void insertAddress( const Address &address );
    void removeAddress( const Address &address );
    Address address( int type ) const;
    Address::List addresses() const;
    Address findAddress( const QString &id ) const;

    void insertCategory( const QString & );
    void removeCategory( const QString & );
    bool hasCategory( const QString & ) const;
    void setCategories( const QStringList & );
    QStringList categories() const;

    void insertCustom( const QString &app, const QString &name,
                       const QString &value );
    void removeCustom( const QString &app, const QString &name );
    QString custom( const QString &app, const QString &name ) const;
    void setCustoms( const QStringList & );
    QStringList customs() const;

    void dump() const;
  
  private:
    Addressee copy();
    void detach();
  
    KSharedPtr<AddresseeData> mData;
};

}

#endif
