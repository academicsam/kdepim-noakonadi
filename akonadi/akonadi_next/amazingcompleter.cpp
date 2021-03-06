/*
    Copyright (c) 2009 Stephen Kelly <steveire@gmail.com>

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


#include "amazingcompleter.h"

#include <kselectionproxymodel.h>
#include <qlistview.h>

#include <kdebug.h>

using namespace Akonadi;

class Akonadi::AmazingCompleterPrivate
{
public:
  AmazingCompleterPrivate(AmazingCompleter *completer) //, QAbstractItemModel *model)
    :   m_matchingRole(Qt::DisplayRole),
        m_completionRole(Qt::DisplayRole),
        m_minumumLength(3),
        q_ptr(completer)
  {

  }

  QAbstractItemModel *m_model;
  KSelectionProxyModel *m_selectionProxyModel;
  QItemSelectionModel *m_itemSelectionModel;
  QAbstractItemView *m_view;
  QWidget *m_widget;
  int m_matchingRole;
  int m_completionRole;
  AmazingCompleter::ViewHandler m_viewHandler;
  QVariant m_matchData;
  int m_minumumLength;

  Q_DECLARE_PUBLIC(AmazingCompleter)
  AmazingCompleter *q_ptr;

};

AmazingCompleter::AmazingCompleter( /* QAbstractItemModel* model, */ QObject* parent)
    : QObject(parent), d_ptr(new AmazingCompleterPrivate(this) /* ,model) */)
{

}

AmazingCompleter::~AmazingCompleter()
{
  delete d_ptr;
}

void AmazingCompleter::setCompletionPrefixString(const QString& matchData)
{
  if (matchData.isEmpty())
    setCompletionPrefix(QVariant());
  else
    setCompletionPrefix(matchData);
}

void AmazingCompleter::setCompletionPrefix(const QVariant& matchData)
{
  Q_D(AmazingCompleter);
  d->m_matchData = matchData;
  d->m_itemSelectionModel->clearSelection();

  if (!matchData.isValid())
  {
    d->m_view->hide();
    return;
  }

  QString matchString = matchData.toString();
  if (matchString.size() < d->m_minumumLength)
  {
    d->m_view->hide();
    return;
  }


  QModelIndex idx = d->m_model->index(0, 0);

  if (!idx.isValid())
    return;

  QModelIndexList matchingIndexes = d->m_model->match(idx, d->m_matchingRole, matchData, -1); // StartsWith

kDebug() << matchingIndexes.size();

  if (matchingIndexes.size() > 0)
    d->m_view->show();
  else
    d->m_view->hide();

  foreach(const QModelIndex &matchingIndex, matchingIndexes)
    d->m_itemSelectionModel->select(matchingIndex, QItemSelectionModel::Select); // Put this in a queued connection?
}

void AmazingCompleter::sourceRowsInserted(const QModelIndex &parent, int start, int end)
{
  Q_UNUSED( parent );
  Q_UNUSED( start );
  Q_UNUSED( end );
  Q_D(AmazingCompleter);
  if (d->m_matchData.isValid())
    setCompletionPrefix(d->m_matchData);
}

void AmazingCompleter::setModel(QAbstractItemModel* model)
{
  Q_D(AmazingCompleter);
  d->m_model = model;
  d->m_itemSelectionModel = new QItemSelectionModel(model, this);
  d->m_selectionProxyModel = new KSelectionProxyModel(d->m_itemSelectionModel, this);
  d->m_selectionProxyModel->setSourceModel(d->m_model);


  connect(d->m_model, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
          SLOT(sourceRowsInserted(const QModelIndex &, int, int)));


}

void AmazingCompleter::setView(QAbstractItemView* view, ViewHandler handler)
{
  Q_D(AmazingCompleter);

  d->m_view = view;

  QSize size = d->m_widget->size();

  size.setHeight(size.height()*10);
  size.setWidth(size.width() * 2.5);

  view->move(50, 50);
  view->resize(size);

  view->hide();


  connectModelToView(d->m_selectionProxyModel, view);
}

void AmazingCompleter::connectModelToView(QAbstractItemModel *model, QAbstractItemView *view)
{
  view->setModel(model);
}

void AmazingCompleter::setWidget(QWidget* widget)
{
  Q_D(AmazingCompleter);
  d->m_widget = widget;
}

void AmazingCompleter::setCompletionRole(int role)
{
  Q_D(AmazingCompleter);
  d->m_completionRole = role;
}

void AmazingCompleter::setMatchingRole(int role)
{
  Q_D(AmazingCompleter);
  d->m_matchingRole = role;
}
