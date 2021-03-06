/*   -*- c++ -*-
 *   accountdialog.h
 *
 *   kmail: KDE mail client
 *   Copyright (C) 2000 Espen Sand, espen@kde.org
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef _ACCOUNT_DIALOG_H_
#define _ACCOUNT_DIALOG_H_

#include <config-kmail.h>
#include "imapaccountbase.h"
#include "ui_imapsettings.h"
#include "ui_localsettings.h"
#include "ui_maildirsettings.h"
#include "ui_popsettings.h"

#include <kdialog.h>
#include <klineedit.h>

#include <QPointer>
#include <QList>

class QRegExpValidator;
class KMAccount;
class KMFolder;
class QButtonGroup;

namespace MailTransport {
class ServerTest;
}

namespace KPIMIdentities {
class IdentityCombo;
}

namespace KMail {

class SieveConfigEditor;
class FolderRequester;

class AccountDialog : public KDialog
{
  Q_OBJECT

  public:
    AccountDialog( const QString & caption, KMAccount *account,
                   QWidget *parent=0 );
    virtual ~AccountDialog();
  private:
    struct LocalWidgets
    {
      Ui::LocalPage ui;
    };

    struct MaildirWidgets
    {
      Ui::MaildirPage ui;
    };

    struct PopWidgets
    {
      Ui::PopPage ui;
      QButtonGroup *encryptionButtonGroup;
      QButtonGroup *authButtonGroup;
    };

    struct ImapWidgets
    {
      Ui::ImapPage ui;
      QButtonGroup *encryptionButtonGroup;
      QButtonGroup *authButtonGroup;
      FolderRequester *trashCombo;
      ImapAccountBase::nsDelimMap nsMap;
      KPIMIdentities::IdentityCombo    *identityCombo;
    };

  private slots:
    virtual void slotOk();
    void slotLocationChooser();
    void slotMaildirChooser();
    void slotEnablePopInterval( bool state );
    void slotEnableImapInterval( bool state );
    void slotEnableLocalInterval( bool state );
    void slotEnableMaildirInterval( bool state );
    void slotFontChanged();
    void slotLeaveOnServerClicked();
    void slotEnableLeaveOnServerDays( bool state );
    void slotEnableLeaveOnServerCount( bool state );
    void slotEnableLeaveOnServerSize( bool state );
    void slotFilterOnServerClicked();
    void slotPipeliningClicked();
    void slotPopEncryptionChanged(int);
    void slotPopPasswordChanged( const QString& text );
    void slotImapPasswordChanged( const QString& text );
    void slotImapEncryptionChanged(int);
    void slotCheckPopCapabilities();
    void slotCheckImapCapabilities();
    void slotPopCapabilities( QList<int> );
    void slotImapCapabilities( QList<int> );
    void slotReloadNamespaces();
    void slotSetupNamespaces( const ImapAccountBase::nsDelimMap& map );
    void slotEditPersonalNamespace();
    void slotEditOtherUsersNamespace();
    void slotEditSharedNamespace();
    void slotConnectionResult( int errorCode, const QString& );
    void slotLeaveOnServerDaysChanged( int value );
    void slotLeaveOnServerCountChanged( int value );
    void slotFilterOnServerSizeChanged( int value );
    void slotIdentityCheckboxChanged();

  private:
    void makeLocalAccountPage();
    void makeMaildirAccountPage();
    void makePopAccountPage();
    void makeImapAccountPage( bool disconnected = false );
    void setupSettings();
    void saveSettings();
    void checkHighest( QButtonGroup * );
    void enablePopFeatures();
    void enableImapAuthMethods();
    void initAccountForConnect();
    const QString namespaceListToString( const QStringList& list );

  private:
    LocalWidgets mLocal;
    MaildirWidgets mMaildir;
    PopWidgets   mPop;
    ImapWidgets  mImap;
    KMAccount    *mAccount;
    QList<QPointer<KMFolder> > mFolderList;
    QStringList  mFolderNames;
    MailTransport::ServerTest *mServerTest;
    KMail::SieveConfigEditor *mSieveConfigEditor;
    QRegExpValidator *mValidator;
    bool mServerTestFailed;
};

class NamespaceLineEdit: public KLineEdit
{
  Q_OBJECT

  public:
    NamespaceLineEdit( QWidget* parent );

    const QString& lastText() { return mLastText; }

  public slots:
    virtual void setText ( const QString & );

  private:
    QString mLastText;
};

class NamespaceEditDialog: public KDialog
{
  Q_OBJECT

  public:
    NamespaceEditDialog( QWidget* parent, ImapAccountBase::imapNamespace type,
        ImapAccountBase::nsDelimMap* map );

  protected slots:
    void slotOk();
    void slotRemoveEntry( int );

  private:
    ImapAccountBase::imapNamespace mType;
    ImapAccountBase::nsDelimMap* mNamespaceMap;
    ImapAccountBase::namespaceDelim mDelimMap;
    QMap<int, NamespaceLineEdit*> mLineEditMap;
    QButtonGroup* mBg;
};

} // namespace KMail

#endif
