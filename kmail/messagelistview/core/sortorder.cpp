/******************************************************************************
 *
 *  Copyright 2009 Thomas McGuire <mcguire@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *******************************************************************************/
#include "messagelistview/core/sortorder.h"

#include <KDebug>
#include <KLocale>

#include <QMetaEnum>

namespace KMail
{

namespace MessageListView
{

namespace Core
{

SortOrder::SortOrder()
  : mMessageSorting( SortMessagesByDateTime ),
    mMessageSortDirection( Descending ),
    mGroupSorting( NoGroupSorting ),
    mGroupSortDirection( Ascending )
{
}

QList< QPair< QString, int > > SortOrder::enumerateMessageSortingOptions( Aggregation::Threading t )
{
  QList< QPair< QString, int > > ret;
  ret.append( QPair< QString, int >( i18n( "None (Storage Order)" ), SortOrder::NoMessageSorting ) );
  ret.append( QPair< QString, int >( i18n( "by Date/Time" ), SortOrder::SortMessagesByDateTime ) );
  if ( t != Aggregation::NoThreading )
    ret.append( QPair< QString, int >( i18n( "by Date/Time of Most Recent in Subtree" ), SortOrder::SortMessagesByDateTimeOfMostRecent ) );
  ret.append( QPair< QString, int >( i18n( "by Sender" ), SortOrder::SortMessagesBySender ) );
  ret.append( QPair< QString, int >( i18n( "by Receiver" ), SortOrder::SortMessagesByReceiver ) );
  ret.append( QPair< QString, int >( i18n( "by Smart Sender/Receiver" ), SortOrder::SortMessagesBySenderOrReceiver ) );
  ret.append( QPair< QString, int >( i18n( "by Subject" ), SortOrder::SortMessagesBySubject ) );
  ret.append( QPair< QString, int >( i18n( "by Size" ), SortOrder::SortMessagesBySize ) );
  ret.append( QPair< QString, int >( i18n( "by Action Item Status" ), SortOrder::SortMessagesByActionItemStatus ) );
  return ret;
}

QList< QPair< QString, int > > SortOrder::enumerateMessageSortDirectionOptions( MessageSorting ms )
{
  QList< QPair< QString, int > > ret;
  if ( ms == SortOrder::NoMessageSorting )
    return ret;

  if (
       ( ms == SortOrder::SortMessagesByDateTime ) ||
       ( ms == SortOrder::SortMessagesByDateTimeOfMostRecent )
     )
  {
    ret.append( QPair< QString, int >( i18n( "Least Recent on Top" ), SortOrder::Ascending ) );
    ret.append( QPair< QString, int >( i18n( "Most Recent on Top" ), SortOrder::Descending ) );
    return ret;
  }

  ret.append( QPair< QString, int >( i18nc( "Sort order for messages", "Ascending" ), SortOrder::Ascending ) );
  ret.append( QPair< QString, int >( i18nc( "Sort order for messages", "Descending" ), SortOrder::Descending ) );
  return ret;
}


QList< QPair< QString, int > > SortOrder::enumerateGroupSortingOptions( Aggregation::Grouping g )
{
  QList< QPair< QString, int > > ret;
  if ( g == Aggregation::NoGrouping )
    return ret;
  ret.append( QPair< QString, int >( i18n( "None (Storage Order)" ), SortOrder::NoGroupSorting ) );
  if ( ( g == Aggregation::GroupByDate ) || ( g == Aggregation::GroupByDateRange ) )
    ret.append( QPair< QString, int >( i18n( "by Date/Time" ), SortOrder::SortGroupsByDateTime ) );
  ret.append( QPair< QString, int >( i18n( "by Date/Time of Most Recent Message in Group" ), SortOrder::SortGroupsByDateTimeOfMostRecent ) );
  if ( g == Aggregation::GroupBySenderOrReceiver )
    ret.append( QPair< QString, int >( i18n( "by Sender/Receiver" ), SortOrder::SortGroupsBySenderOrReceiver ) );
  if ( g == Aggregation::GroupBySender )
    ret.append( QPair< QString, int >( i18n( "by Sender" ), SortOrder::SortGroupsBySender ) );
  if ( g == Aggregation::GroupByReceiver )
    ret.append( QPair< QString, int >( i18n( "by Receiver" ), SortOrder::SortGroupsByReceiver ) );
  return ret;
}

QList< QPair< QString, int > > SortOrder::enumerateGroupSortDirectionOptions( Aggregation::Grouping g,
                                                                              GroupSorting gs )
{
  QList< QPair< QString, int > > ret;
  if ( g == Aggregation::NoGrouping )
    return ret;
  if ( gs == SortOrder::NoGroupSorting )
    return ret;
  if ( gs == SortOrder::SortGroupsByDateTimeOfMostRecent )
  {
    ret.append( QPair< QString, int >( i18n( "Least Recent on Top" ), SortOrder::Ascending ) );
    ret.append( QPair< QString, int >( i18n( "Most Recent on Top" ), SortOrder::Descending ) );
    return ret;
  }
  ret.append( QPair< QString, int >( i18nc( "Sort order for mail groups", "Ascending" ), SortOrder::Ascending ) );
  ret.append( QPair< QString, int >( i18nc( "Sort order for mail groups", "Descending" ), SortOrder::Descending ) );
  return ret;
}

typedef QPair< QString, int > Pair;
typedef QList< Pair > OptionList;
static bool optionListHasOption( const OptionList &optionList, int optionValue,
                                 int defaultOptionValue )
{
  foreach( const Pair &pair, optionList ) {
    if ( pair.second == optionValue ) {
      return true;
    }
  }
  if ( optionValue != defaultOptionValue )
    return false;
  else return true;
}

bool SortOrder::validForAggregation( const Aggregation *aggregation ) const
{
  OptionList messageSortings = enumerateMessageSortingOptions( aggregation->threading() );
  OptionList messageSortDirections = enumerateMessageSortDirectionOptions( mMessageSorting );
  OptionList groupSortings = enumerateGroupSortingOptions( aggregation->grouping() );
  OptionList groupSortDirections = enumerateGroupSortDirectionOptions( aggregation->grouping(),
                                                                       mGroupSorting );
  bool messageSortingOk = optionListHasOption( messageSortings,
                                               mMessageSorting, SortOrder().messageSorting() );
  bool messageSortDirectionOk = optionListHasOption( messageSortDirections, mMessageSortDirection,
                                                     SortOrder().messageSortDirection() );
  bool groupSortingOk = optionListHasOption( groupSortings, mGroupSorting,
                                             SortOrder().groupSorting() );
  bool groupSortDirectionOk = optionListHasOption( groupSortDirections, mGroupSortDirection,
                                                   SortOrder().groupSortDirection() );
  kDebug() << messageSortingOk << messageSortDirectionOk << groupSortingOk << groupSortDirectionOk;
  return messageSortingOk && messageSortDirectionOk &&
         groupSortingOk && groupSortDirectionOk;
}

bool SortOrder::readConfigHelper( KConfigGroup &conf, const QString &id )
{
  if ( !conf.hasKey( id + QString( "MessageSorting" ) ) )
    return false;
  mMessageSorting = messageSortingForName(
       conf.readEntry( id + QString( "MessageSorting" ) ) );
  mMessageSortDirection = sortDirectionForName(
       conf.readEntry( id + QString( "MessageSortDirection" ) ) );
  mGroupSorting = groupSortingForName(
       conf.readEntry( id + QString( "GroupSorting" ) ) );
  mGroupSortDirection = sortDirectionForName(
       conf.readEntry( id + QString( "GroupSortDirection" ) ) );
  return true;
}

void SortOrder::readConfig( KConfigGroup &conf, const QString &storageId,
                            bool *storageUsesPrivateSortOrder )
{
  SortOrder privateSortOrder, globalSortOrder;
  globalSortOrder.readConfigHelper( conf, "GlobalSortOrder" );
  *storageUsesPrivateSortOrder = privateSortOrder.readConfigHelper( conf, storageId );
  if ( *storageUsesPrivateSortOrder )
    *this = privateSortOrder;
  else
    *this = globalSortOrder;

  kDebug() << nameForMessageSorting( mMessageSorting );
  kDebug() << nameForSortDirection( mMessageSortDirection );
  kDebug() << nameForGroupSorting( mGroupSorting );
  kDebug() << nameForSortDirection( mGroupSortDirection );
}

void SortOrder::writeConfig( KConfigGroup &conf, const QString &storageId,
                             bool storageUsesPrivateSortOrder ) const
{
  QString id = storageId;
  if ( !storageUsesPrivateSortOrder ) {
    id = "GlobalSortOrder";
    conf.deleteEntry( storageId + QString( "MessageSorting" ) );
    conf.deleteEntry( storageId + QString( "MessageSortDirection" ) );
    conf.deleteEntry( storageId + QString( "GroupSorting" ) );
    conf.deleteEntry( storageId + QString( "GroupSortDirection" ) );
  }

   kDebug() << "Writing sort order for" << storageId << ", using ID" << id;
   kDebug() << nameForMessageSorting( mMessageSorting );
   kDebug() << nameForSortDirection( mMessageSortDirection );
   kDebug() << nameForGroupSorting( mGroupSorting );
   kDebug() << nameForSortDirection( mGroupSortDirection );
   conf.writeEntry( id + QString( "MessageSorting" ),
                    nameForMessageSorting( mMessageSorting ) );
   conf.writeEntry( id + QString( "MessageSortDirection" ),
                    nameForSortDirection( mMessageSortDirection ) );
   conf.writeEntry( id + QString( "GroupSorting" ),
                    nameForGroupSorting( mGroupSorting ) );
   conf.writeEntry( id + QString( "GroupSortDirection" ),
                    nameForSortDirection( mGroupSortDirection ) );

}

bool SortOrder::isValidMessageSorting( SortOrder::MessageSorting ms )
{
  switch( ms )
  {
    case SortOrder::NoMessageSorting:
    case SortOrder::SortMessagesByDateTime:
    case SortOrder::SortMessagesByDateTimeOfMostRecent:
    case SortOrder::SortMessagesBySenderOrReceiver:
    case SortOrder::SortMessagesBySender:
    case SortOrder::SortMessagesByReceiver:
    case SortOrder::SortMessagesBySubject:
    case SortOrder::SortMessagesBySize:
    case SortOrder::SortMessagesByActionItemStatus:
      // ok
    break;
    default:
      // b0rken
      return false;
    break;
  }

  return true;
}

const QString SortOrder::nameForSortDirection( SortDirection sortDirection )
{
  int index = staticMetaObject.indexOfEnumerator( "SortDirection" );
  return staticMetaObject.enumerator( index ).valueToKey( sortDirection );
}

const QString SortOrder::nameForMessageSorting( MessageSorting messageSorting )
{
  int index = staticMetaObject.indexOfEnumerator( "MessageSorting" );
  return staticMetaObject.enumerator( index ).valueToKey( messageSorting );
}

const QString SortOrder::nameForGroupSorting( GroupSorting groupSorting )
{
  int index = staticMetaObject.indexOfEnumerator( "GroupSorting" );
  return staticMetaObject.enumerator( index ).valueToKey( groupSorting );
}

SortOrder::SortDirection SortOrder::sortDirectionForName( const QString& name )
{
  int index = staticMetaObject.indexOfEnumerator( "SortDirection" );
  return static_cast<SortDirection>( staticMetaObject.enumerator( index ).keyToValue(
                              name.toLatin1().constData() ) );
}

SortOrder::MessageSorting SortOrder::messageSortingForName( const QString& name )
{
  int index = staticMetaObject.indexOfEnumerator( "MessageSorting" );
  return static_cast<MessageSorting>( staticMetaObject.enumerator( index ).keyToValue(
                              name.toLatin1().constData() ) );
}

SortOrder::GroupSorting SortOrder::groupSortingForName( const QString& name )
{
  int index = staticMetaObject.indexOfEnumerator( "GroupSorting" );
  return static_cast<GroupSorting>( staticMetaObject.enumerator( index ).keyToValue(
                              name.toLatin1().constData() ) );
}

} // namespace Core

} // namespace MessageListView

} // namespace KMail

#include "sortorder.moc"