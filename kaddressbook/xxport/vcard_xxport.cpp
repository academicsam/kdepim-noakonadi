/*
    This file is part of KAddressbook.
    Copyright (c) 2003 Tobias Koenig <tokoe@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

    As a special exception, permission is given to link this program
    with any edition of Qt, and distribute the resulting executable,
    without including the source code for Qt in the source distribution.
*/

#include "vcard_xxport.h"

#include <QtCore/QFile>
#include <QtGui/QCheckBox>
#include <QtGui/QFont>
#include <QtGui/QFrame>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include <kabc/vcardconverter.h>
#include <kapplication.h>
#include <kdialog.h>
#include <kfiledialog.h>
#include <kio/netaccess.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <ktemporaryfile.h>
#include <kurl.h>
#include <libkdepim/addresseeview.h>

#include "gpgme++/context.h"
#include "gpgme++/data.h"
#include "gpgme++/key.h"
#include "qgpgme/dataprovider.h"

#include "xxportmanager.h"

K_EXPORT_KADDRESSBOOK_XXFILTER( kaddrbk_vcard_xxport, VCardXXPort )

class VCardViewerDialog : public KDialog
{
  public:
    VCardViewerDialog( const KABC::Addressee::List &list,
                       QWidget *parent );

    KABC::Addressee::List contacts() const;

  protected:
    void slotUser1();
    void slotUser2();
    void slotApply();
    void slotCancel();

  private:
    void updateView();

    KPIM::AddresseeView *mView;

    KABC::Addressee::List mContacts;
    KABC::Addressee::List::Iterator mIt;
};

class VCardExportSelectionDialog : public KDialog
{
  public:
    VCardExportSelectionDialog( QWidget *parent );
    ~VCardExportSelectionDialog();

    bool exportPrivateFields() const;
    bool exportBusinessFields() const;
    bool exportOtherFields() const;
    bool exportEncryptionKeys() const;

  private:
    QCheckBox *mPrivateBox;
    QCheckBox *mBusinessBox;
    QCheckBox *mOtherBox;
    QCheckBox *mEncryptionKeys;
};

VCardXXPort::VCardXXPort( KABC::AddressBook *ab, QWidget *parent, const char *name )
  : KAB::XXPort( ab, parent, name )
{
  createImportAction( i18n( "Import vCard..." ) );
  createExportAction( i18n( "Export vCard 2.1..." ), "v21" );
  createExportAction( i18n( "Export vCard 3.0..." ), "v30" );
}

bool VCardXXPort::exportContacts( const KABC::AddresseeList &addrList, const QString &identifier )
{
  KABC::VCardConverter converter;
  KUrl url;
  KABC::AddresseeList list;

  list = filterContacts( addrList );

  bool ok = true;
  if ( list.isEmpty() ) {
    return ok;
  } else if ( list.count() == 1 ) {
    url = KFileDialog::getSaveUrl( list[ 0 ].givenName() + '_' + list[ 0 ].familyName() + ".vcf" );
    if ( url.isEmpty() )
      return true;

    if ( identifier == "v21" )
      ok = doExport( url, converter.createVCards( list, KABC::VCardConverter::v2_1 ) );
    else
      ok = doExport( url, converter.createVCards( list, KABC::VCardConverter::v3_0 ) );
  } else {
    QString msg = i18n( "You have selected a list of contacts, shall they be "
                        "exported to several files?" );

    switch ( KMessageBox::questionYesNo( parentWidget(), msg, QString(), KGuiItem(i18n("Export to Several Files")), KGuiItem(i18n("Export to One File")) ) ) {
      case KMessageBox::Yes: {
        KUrl baseUrl = KFileDialog::getExistingDirectoryUrl();
        if ( baseUrl.isEmpty() )
          return true;

        KABC::AddresseeList::ConstIterator it;
        for ( it = list.constBegin(); it != list.constEnd(); ++it ) {
          url = baseUrl.url() + '/' + (*it).givenName() + '_' + (*it).familyName() + ".vcf";

          bool tmpOk;
          KABC::AddresseeList tmpList;
          tmpList.append( *it );

          if ( identifier == "v21" )
            tmpOk = doExport( url, converter.createVCards( tmpList, KABC::VCardConverter::v2_1 ) );
          else
            tmpOk = doExport( url, converter.createVCards( tmpList, KABC::VCardConverter::v3_0 ) );

          ok = ok && tmpOk;
        }
        break;
      }
      case KMessageBox::No:
      default: {
        url = KFileDialog::getSaveUrl( KUrl("addressbook.vcf") );
        if ( url.isEmpty() )
          return true;

        if ( identifier == "v21" )
          ok = doExport( url, converter.createVCards( list, KABC::VCardConverter::v2_1 ) );
        else
          ok = doExport( url, converter.createVCards( list, KABC::VCardConverter::v3_0 ) );
      }
    }
  }

  return ok;
}

KABC::Addressee::List VCardXXPort::importContacts( const QString& ) const
{
  QString fileName;
  KABC::Addressee::List addrList;
  KUrl::List urls;

  if ( !XXPortManager::importData.isEmpty() )
    addrList = parseVCard( XXPortManager::importData.toAscii() );
  else {
    if ( XXPortManager::importURL.isEmpty() )
      urls = KFileDialog::getOpenUrls( KUrl(), "*.vcf|vCards", parentWidget(),
                                       i18n( "Select vCard to Import" ) );
    else
      urls.append( XXPortManager::importURL );

    if ( urls.count() == 0 )
      return addrList;

    QString caption( i18n( "vCard Import Failed" ) );
    bool anyFailures = false;
    KUrl::List::Iterator it;
    for ( it = urls.begin(); it != urls.end(); ++it ) {
      if ( KIO::NetAccess::download( *it, fileName, parentWidget() ) ) {

        QFile file( fileName );

        if ( file.open( QIODevice::ReadOnly ) ) {
          const QByteArray data = file.readAll();
          file.close();
          if ( data.size() > 0 ) {
            addrList += parseVCard( data );
          }

          KIO::NetAccess::removeTempFile( fileName );
        } else {
          QString text = i18nc( "@info",
                                "<para>When trying to read the vCard, there was an error opening the file <filename>%1</filename>:</para>"
                                "<para>%2</para>",
                                (*it).pathOrUrl(),
                                i18nc( "QFile", file.errorString().toLatin1() ) );
          KMessageBox::error( parentWidget(), text, caption );
          anyFailures = true;
        }
      } else {
        QString text = i18nc( "@info", "<para>Unable to access vCard:</para><para>%1</para>", KIO::NetAccess::lastErrorString() );
        KMessageBox::error( parentWidget(), text, caption );
        anyFailures = true;
      }
    }

    if ( !XXPortManager::importURL.isEmpty() ) { // a vcard was passed via cmd
      if ( addrList.isEmpty() ) {
        if ( anyFailures && urls.count() > 1 )
          KMessageBox::information( parentWidget(),
                                    i18n( "No contacts were imported, due to errors with the vCards." ) );
        else if ( !anyFailures )
          KMessageBox::information( parentWidget(), i18n( "The vCard does not contain any contacts." ) );
      } else {
        VCardViewerDialog dlg( addrList, parentWidget() );
        dlg.exec();
        addrList = dlg.contacts();
      }
    }
  }

  return addrList;
}

KABC::Addressee::List VCardXXPort::parseVCard( const QByteArray &data ) const
{
  KABC::VCardConverter converter;

  return converter.parseVCards( data );
}

bool VCardXXPort::doExport( const KUrl &url, const QByteArray &data )
{
  if( QFileInfo(url.isLocalFile() ? url.toLocalFile() : url.path()).exists() ) {
      if(KMessageBox::questionYesNo( parentWidget(), i18n("Do you want to overwrite file \"%1\"", url.isLocalFile() ? url.toLocalFile() : url.path()) ) == KMessageBox::No)
        return false;
  }

  KTemporaryFile tmpFile;
  tmpFile.open();

  tmpFile.write( data );
  tmpFile.flush();

  return KIO::NetAccess::upload( tmpFile.fileName(), url, parentWidget() );
}

KABC::AddresseeList VCardXXPort::filterContacts( const KABC::AddresseeList &addrList )
{
  KABC::AddresseeList list;

  if ( addrList.isEmpty() )
    return addrList;

  VCardExportSelectionDialog dlg( parentWidget() );
  if ( !dlg.exec() )
    return list;

  KABC::AddresseeList::ConstIterator it;
  for ( it = addrList.begin(); it != addrList.end(); ++it ) {
    KABC::Addressee addr;

    addr.setUid( (*it).uid() );
    addr.setFormattedName( (*it).formattedName() );
    addr.setPrefix( (*it).prefix() );
    addr.setGivenName( (*it).givenName() );
    addr.setAdditionalName( (*it).additionalName() );
    addr.setFamilyName( (*it).familyName() );
    addr.setSuffix( (*it).suffix() );
    addr.setNickName( (*it).nickName() );
    addr.setMailer( (*it).mailer() );
    addr.setTimeZone( (*it).timeZone() );
    addr.setGeo( (*it).geo() );
    addr.setProductId( (*it).productId() );
    addr.setSortString( (*it).sortString() );
    addr.setUrl( (*it).url() );
    addr.setSecrecy( (*it).secrecy() );
    addr.setSound( (*it).sound() );
    addr.setEmails( (*it).emails() );
    addr.setCategories( (*it).categories() );

    if ( dlg.exportPrivateFields() ) {
      addr.setBirthday( (*it).birthday() );
      addr.setNote( (*it).note() );
      addr.setPhoto( (*it).photo() );
    }

    if ( dlg.exportBusinessFields() ) {
      addr.setTitle( (*it).title() );
      addr.setRole( (*it).role() );
      addr.setOrganization( (*it).organization() );
      addr.setDepartment( (*it).department() );

      addr.setLogo( (*it).logo() );

      KABC::PhoneNumber::List phones = (*it).phoneNumbers( KABC::PhoneNumber::Work );
      KABC::PhoneNumber::List::Iterator phoneIt;
      for ( phoneIt = phones.begin(); phoneIt != phones.end(); ++phoneIt )
        addr.insertPhoneNumber( *phoneIt );

      KABC::Address::List addresses = (*it).addresses( KABC::Address::Work );
      KABC::Address::List::Iterator addrIt;
      for ( addrIt = addresses.begin(); addrIt != addresses.end(); ++addrIt )
        addr.insertAddress( *addrIt );
    }

    KABC::PhoneNumber::List phones = (*it).phoneNumbers();
    KABC::PhoneNumber::List::Iterator phoneIt;
    for ( phoneIt = phones.begin(); phoneIt != phones.end(); ++phoneIt ) {
      int type = (*phoneIt).type();

      if ( type & KABC::PhoneNumber::Home && dlg.exportPrivateFields() )
        addr.insertPhoneNumber( *phoneIt );
      else if ( type & KABC::PhoneNumber::Work && dlg.exportBusinessFields() )
        addr.insertPhoneNumber( *phoneIt );
      else if ( dlg.exportOtherFields() )
        addr.insertPhoneNumber( *phoneIt );
    }

    KABC::Address::List addresses = (*it).addresses();
    KABC::Address::List::Iterator addrIt;
    for ( addrIt = addresses.begin(); addrIt != addresses.end(); ++addrIt ) {
      int type = (*addrIt).type();

      if ( type & KABC::Address::Home && dlg.exportPrivateFields() )
        addr.insertAddress( *addrIt );
      else if ( type & KABC::Address::Work && dlg.exportBusinessFields() )
        addr.insertAddress( *addrIt );
      else if ( dlg.exportOtherFields() )
        addr.insertAddress( *addrIt );
    }

    if ( dlg.exportOtherFields() )
      addr.setCustoms( (*it).customs() );

    if ( dlg.exportEncryptionKeys() ) {
      addKey( addr, KABC::Key::PGP );
      addKey( addr, KABC::Key::X509 );
    }

    list.append( addr );
  }

  return list;
}

void VCardXXPort::addKey( KABC::Addressee &addr, KABC::Key::Type type )
{
  QString fingerprint = addr.custom( "KADDRESSBOOK",
                                     (type == KABC::Key::PGP ? "OPENPGPFP" : "SMIMEFP") );
  if ( fingerprint.isEmpty() )
    return;

  GpgME::Context * context = GpgME::Context::createForProtocol( GpgME::OpenPGP );
  if ( !context ) {
    kError() <<"No context available";
    return;
  }

  context->setArmor( false );
  context->setTextMode( false );

  QGpgME::QByteArrayDataProvider dataProvider;
  GpgME::Data dataObj( &dataProvider );
  GpgME::Error error = context->exportPublicKeys( fingerprint.toLatin1(), dataObj );
  delete context;

  if ( error ) {
    kError() << error.asString();
    return;
  }

  KABC::Key key;
  key.setType( type );
  key.setBinaryData( dataProvider.data() );

  addr.insertKey( key );
}

// ---------- VCardViewer Dialog ---------------- //

VCardViewerDialog::VCardViewerDialog( const KABC::Addressee::List &list,
                                      QWidget *parent )
  : KDialog( parent ),
    mContacts( list )
{
  setCaption( i18n( "Import vCard" ) );
  setButtons( Yes | No | Apply | Cancel );
  setDefaultButton( Yes );
  setModal( true );
  showButtonSeparator( true );

  QFrame *page = new QFrame( this );
  setMainWidget( page );
  QVBoxLayout *layout = new QVBoxLayout( page );
  layout->setSpacing( spacingHint() );
  layout->setMargin( marginHint() );

  QLabel *label = new QLabel( i18n( "Do you want to import this contact in your address book?" ), page );
  QFont font = label->font();
  font.setBold( true );
  label->setFont( font );
  layout->addWidget( label );

  mView = new KPIM::AddresseeView( page );
  mView->enableLinks( 0 );
  mView->setVerticalScrollBarPolicy ( Qt::ScrollBarAsNeeded );
  layout->addWidget( mView );

  setButtonText( Apply, i18n( "Import All..." ) );

  mIt = mContacts.begin();
  connect(this,SIGNAL(applyClicked()),this,SLOT(slotApply()));
  connect(this,SIGNAL(cancelClicked()),this,SLOT(slotCancel()));
  updateView();
#ifdef __GNUC__
#warning "kde4: when user1 and user2 is used ? see slotUser1 and slotUser2 defined in this class"
#endif
}

KABC::Addressee::List VCardViewerDialog::contacts() const
{
  return mContacts;
}

void VCardViewerDialog::updateView()
{
  mView->setAddressee( *mIt );

  KABC::Addressee::List::Iterator it = mIt;
  enableButton( Apply, (++it) != mContacts.end() );
}

void VCardViewerDialog::slotUser1()
{

     	mIt = mContacts.erase( mIt );

  if ( mIt == mContacts.end() )
    slotApply();

  updateView();
}

void VCardViewerDialog::slotUser2()
{
  mIt++;

  if ( mIt == mContacts.end() )
    slotApply();

  updateView();
}

void VCardViewerDialog::slotApply()
{
  QDialog::accept();
}

void VCardViewerDialog::slotCancel()
{
  mContacts.clear();
  QDialog::accept();
}

// ---------- VCardExportSelection Dialog ---------------- //

VCardExportSelectionDialog::VCardExportSelectionDialog( QWidget *parent )
  : KDialog( parent )
{
  setCaption( i18n( "Select vCard Fields" ) );
  setButtons( Ok | Cancel );
  setDefaultButton( Ok );
  setModal( true );
  showButtonSeparator( true );

  QFrame *page = new QFrame( this );
  setMainWidget( page );

  QVBoxLayout *layout = new QVBoxLayout( page );
  layout->setSpacing( spacingHint() );
  layout->setMargin( marginHint() );

  QLabel *label = new QLabel( i18n( "Select the fields which shall be exported in the vCard." ), page );
  layout->addWidget( label );

  mPrivateBox = new QCheckBox( i18n( "Private fields" ), page );
  layout->addWidget( mPrivateBox );

  mBusinessBox = new QCheckBox( i18n( "Business fields" ), page );
  layout->addWidget( mBusinessBox );

  mOtherBox = new QCheckBox( i18n( "Other fields" ), page );
  layout->addWidget( mOtherBox );

  mEncryptionKeys = new QCheckBox( i18n( "Encryption keys" ), page );
  layout->addWidget( mEncryptionKeys );

  KConfig _config( "kaddressbookrc" );
  KConfigGroup config(&_config, "XXPortVCard" );

  mPrivateBox->setChecked( config.readEntry( "ExportPrivateFields", true ) );
  mBusinessBox->setChecked( config.readEntry( "ExportBusinessFields", false ) );
  mOtherBox->setChecked( config.readEntry( "ExportOtherFields", false ) );
  mEncryptionKeys->setChecked( config.readEntry( "ExportEncryptionKeys", false ) );
}

VCardExportSelectionDialog::~VCardExportSelectionDialog()
{
  KConfig _config( "kaddressbookrc" );
  KConfigGroup config(&_config, "XXPortVCard" );

  config.writeEntry( "ExportPrivateFields", mPrivateBox->isChecked() );
  config.writeEntry( "ExportBusinessFields", mBusinessBox->isChecked() );
  config.writeEntry( "ExportOtherFields", mOtherBox->isChecked() );
  config.writeEntry( "ExportEncryptionKeys", mEncryptionKeys->isChecked() );
}

bool VCardExportSelectionDialog::exportPrivateFields() const
{
  return mPrivateBox->isChecked();
}

bool VCardExportSelectionDialog::exportBusinessFields() const
{
  return mBusinessBox->isChecked();
}

bool VCardExportSelectionDialog::exportOtherFields() const
{
  return mOtherBox->isChecked();
}

bool VCardExportSelectionDialog::exportEncryptionKeys() const
{
  return mEncryptionKeys->isChecked();
}

#include "vcard_xxport.moc"
