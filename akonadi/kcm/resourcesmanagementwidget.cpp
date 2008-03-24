/*
    Copyright (c) 2006 - 2007 Volker Krause <vkrause@kde.org>
    Copyright (c) 2008 Omat Holding B.V. <tomalbers@kde.nl>

    Based on KMail code by:
    Copyright (C) 2001-2003 Marc Mutz <mutz@kde.org>

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

#include "resourcesmanagementwidget.h"
#include "ui_resourcesmanagementwidget.h"

#include <akonadi/agentmanager.h>
#include <akonadi/agentinstancecreatejob.h>
#include <akonadi/agentfilterproxymodel.h>

#include <KDebug>
#include <KMenu>
#include <KMessageBox>
#include <KMimeType>

static bool isWantedResource( const QStringList &wantedMimeTypes, const QStringList &resourceMimeTypes )
{
  foreach ( const QString resourceMimeType, resourceMimeTypes ) {
    KMimeType::Ptr mimeType = KMimeType::mimeType( resourceMimeType, KMimeType::ResolveAliases );
    if ( mimeType.isNull() )
      continue;

    // check if the resource MIME type is or inherits from the
    // wanted one, e.g.
    // if the resource offers message/news and wanted types includes
    // message/rfc822 this will be true, because message/news is a
    // "subclass" of message/rfc822
    foreach ( const QString wantedMimeType, wantedMimeTypes ) {
      if ( mimeType->is( wantedMimeType ) )
        return true;
    }
  }

  return false;
}

class ResourcesManagementWidget::Private
{
public:
    Ui::ResourcesManagementWidget   ui;
    Akonadi::AgentManager           *manager;
    QHash<QAction*, QString>        menuOptions;
    QStringList                     wantedMimeTypes;
};

ResourcesManagementWidget::ResourcesManagementWidget( QWidget *parent,  const QStringList &args ) :
        QWidget( parent ),
        d( new Private )
{
    d->wantedMimeTypes = args;
    d->ui.setupUi( this );
    d->manager = new Akonadi::AgentManager( this );

    KMenu *addMenu = new KMenu( this );
    bool atleastone = false;
    QStringList types = d->manager->agentTypes();
    foreach( const QString& type, types ) {
        if ( !d->manager->agentCapabilities( type ).contains( "Resource" ) )
            continue;

        QStringList mimeTypes = d->manager->agentMimeTypes( type );
        bool wanted = isWantedResource( d->wantedMimeTypes, mimeTypes );

        if ( !wanted && !d->wantedMimeTypes.isEmpty() )
            continue;

        QAction *action = addMenu->addAction( d->manager->agentName( type ) );
        action->setIcon( d->manager->agentIcon( type ) );
        d->menuOptions.insert( action, type );
        atleastone = true;
    }

    if ( !atleastone ) {
        QAction *action = addMenu->addAction( i18n( "Akonadi server is not running or "
                                              "the server could not find any resources." ) );
        action->setEnabled( false );
        KMessageBox::information( this, i18n( "Akonadi server is not running or "
                                              "the server could not find any resources." ), i18n( "No resources found" ) );
    }

    d->ui.addButton->setMenu( addMenu );
    connect( addMenu, SIGNAL( triggered( QAction* ) ), SLOT( addClicked( QAction* ) ) );

    foreach( const QString& type, d->wantedMimeTypes )
        d->ui.resourcesList->agentFilterProxyModel()->addMimeType( type );
    connect( d->ui.resourcesList, SIGNAL( currentChanged( const QString&, const QString& ) ),
             SLOT( updateButtonState( const QString& ) ) );
    connect( d->ui.resourcesList, SIGNAL( doubleClicked( const QString& ) ),
             SLOT( editClicked() ) );

    connect( d->ui.editButton, SIGNAL( clicked() ), SLOT( editClicked() ) );
    connect( d->ui.removeButton, SIGNAL( clicked() ), SLOT( removeClicked() ) );

    connect( d->manager, SIGNAL( agentInstanceAdded( const QString& ) ),
             SLOT( fillResourcesList() ) );
    connect( d->manager, SIGNAL( agentInstanceRemoved( const QString& ) ),
             SLOT( fillResourcesList() ) );
    connect( d->manager, SIGNAL( agentInstanceNameChanged( const QString&, const QString& ) ),
             SLOT( fillResourcesList() ) );
    updateButtonState();
}

ResourcesManagementWidget::~ResourcesManagementWidget()
{
    delete d;
}

void ResourcesManagementWidget::updateButtonState( const QString& current)
{
    if ( current.isEmpty() ) {
        d->ui.editButton->setEnabled( false );
        d->ui.removeButton->setEnabled( false );
    } else {
        d->ui.editButton->setEnabled( true );
        d->ui.removeButton->setEnabled( true );
    }
}

void ResourcesManagementWidget::addClicked( QAction *action )
{
    Q_ASSERT( d->menuOptions.contains( action ) );
    Akonadi::AgentInstanceCreateJob *job = new Akonadi::AgentInstanceCreateJob( d->menuOptions.value( action ) , this );
    job->configure( this );
    job->start();
}

void ResourcesManagementWidget::editClicked()
{
    QString current = d->ui.resourcesList->currentAgentInstance();
    if ( !current.isEmpty() )
        d->manager->agentInstanceConfigure( current, this );
}

void ResourcesManagementWidget::removeClicked()
{
    QString current = d->ui.resourcesList->currentAgentInstance();
    if ( !current.isEmpty() )
        d->manager->removeAgentInstance( current );
    updateButtonState();
}

#include "resourcesmanagementwidget.moc"
