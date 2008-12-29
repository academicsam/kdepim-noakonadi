/*
    Copyright (c) 2008 Stephen Kelly <steveire@gmail.com>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#ifndef AKONADIUPDATEADAPTER_H
#define AKONADIUPDATEADAPTER_H

#include <akonadi/collectionfetchjob.h>
#include <akonadi/itemfetchscope.h>

#include <kjob.h>
#include <akonadi/item.h>
#include <akonadi/collection.h>


namespace Akonadi
{
class Session;

/**
Updates akonadi about changes in the model.
@internal
*/
class EntityUpdateAdapter : public QObject
{
  Q_OBJECT
public:

  enum IncludeUnsub {
    IncludeUnsubscribed,
    DoNotIncludeUnsubscribed
  };

  EntityUpdateAdapter( Session *session,
                       ItemFetchScope scope,
                       QObject *parent = 0,
                       int includeUnsubscribed = EntityUpdateAdapter::DoNotIncludeUnsubscribed );

  ~EntityUpdateAdapter();

  void addEntities( Item::List newItems, Collection::List newCollections, Collection parent, int row );

  void fetchCollections( Collection col, CollectionFetchJob::Type = CollectionFetchJob::FirstLevel );

  /**
  Fetch job for a list of items.
  */
  void fetchItems( Item::List items );

  /**
   Fetch items in parent.
  */
  void fetchItems( Collection parent );

  void beginTransaction();
  void endTransaction();

  /**
  Moves entities from src to dst. If items are coming from mulitple sources, this must be called once for each source.
  */
  void moveEntities( Item::List movedItems, Collection::List movedCollections, Collection src, Collection dst, int row );
  void removeEntities( Item::List removedItems, Collection::List removedCollections, Collection parent );

  void updateEntities( Collection::List updatedCollections, Item::List updatedItems );
  void updateEntities( Collection::List updatedCollections );
  void updateEntities( Item::List updatedItems );

public Q_SLOTS:
  void listJobDone( KJob *job );
  void updateJobDone( KJob *job );
  // etc.

  // Make this private
  void itemsReceivedFromJob( Akonadi::Item::List list );

// Signals reemitted by jobs.
Q_SIGNALS:
  void collectionsReceived( Akonadi::Collection::List list );
  void itemsReceived( Akonadi::Item::List list, Collection::Id id );
  void collectionStatisticsChanged();

private:
  Session *m_session;
  QObject *m_job_parent;
  ItemFetchScope m_itemFetchScope;
  bool m_includeUnsubscribed;

  /**
  The id of the collection which starts an item fetch job. This is part of a hack with QObject::sender
  in itemsAdded to correctly insert items into the model.
  */
  static QByteArray ItemFetchCollectionId() {
    return "ItemFetchCollectionId";
  }
  // TODO d ptr.

};

}

#endif

