#include <qstring.h>

#ifndef KAB_ENUM_H
#define KAB_ENUM_H

#ifndef NDEBUG
#  include <iostream.h>
#  ifdef __GNUG__
#    define kabDebug(a) cerr << className() << ":" << __FUNCTION__ << " (" \
            << __LINE__ << "): " << (a) << endl;
#  else
#    define kabDebug(a) cerr << className() << ": " \
            << (a) << endl;
#  endif
#else
#  define kabDebug(a)
#endif
  
namespace KAB
{

const QString typeText      ("text");
const QString typeDate      ("date");
const QString typeTime      ("time");
const QString typeDateTime  ("date-time");
const QString typeEmail     ("email");
const QString typeImage     ("image");
const QString typeImageRef  ("image-ref");
const QString typeURL       ("url");
const QString typeInteger   ("integer");
const QString typeDouble    ("double");
const QString typeSound     ("sound");
const QString typeSoundRef  ("sound-ref");
const QString typeUTCOffset ("utc-offset");

  enum
ValueType
{
  Text,
  Date,
  Time,
  DateTime,
  Email,
  Image,
  ImageRef,
  URL,
  Integer,
  Double,
  Sound,
  SoundRef,
  UTCOffset,
  XValue,
  Any
};

  enum
CommunicationSpec {
  Telephone,
  Fax,
  Telex,
  PostalAddress,
  EmailAddress,
  TalkAddress,
};

  enum
WorkInfo {
  GeographicalInfo,
  CommunicationInfo,
  XField
};

  enum
HomeInfo {
  GeographicalInfo,
  CommunicationInfo,
  XField
};

  enum
PersonalInfo {
  FullName,
  DisplayName,
  Title,
  Names,
  Suffixes,
  Relations,
  Gender,
  CommunicationInfo,
  XField
};

class LocationInfo {
  GeographicalInfo
};

  enum
PersonSpec {
  PersonalInfo,
  HomeInfo,
  WorkInfo,
  Notes,
  XField
};

  enum
FieldType {
  Organisation,
  Department,
  JobTitle,
  Role,
  Birthday,
  Anniversaries,
  TalkAddresses,
  Keywords,
  DeliveryLabel,
  Email,
  Home,
  Work,
  Telephone,
  URL,
  Notes,
  LastMeeting,
  NextMeeting,
  XField
};
 
  ValueType
typeNameToEnum(const QString &);

  QString
valueTypeToString(ValueType);

}

#endif
// vim:ts=2:sw=2:tw=78:
