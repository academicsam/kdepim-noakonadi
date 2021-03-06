/** -*- c++ -*-
 * networkaccount.cpp
 *
 * Copyright (c) 2000-2002 Michael Haeckel <haeckel@kde.org>
 * Copyright (c) 2002 Marc Mutz <mutz@kde.org>
 *
 * This file is based on work on pop3 and imap account implementations
 * by Don Sanders <sanders@kde.org> and Michael Haeckel <haeckel@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */




#include "networkaccount.h"
#include "accountmanager.h"
#include "kmkernel.h"
#include "globalsettings.h"

#include <kconfiggroup.h>
#include <kio/global.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstringhandler.h>
#include <kdebug.h>
#include <kwallet.h>
using KIO::MetaData;
using KWallet::Wallet;

#include <climits>

namespace KMail {


 // for restricting number of concurrent connections to the same server
  typedef QHash<QString, int> ServerConnectionsHash;
  K_GLOBAL_STATIC( ServerConnectionsHash, s_serverConnections )

  NetworkAccount::NetworkAccount( AccountManager * parent, const QString & name, uint id )
    : KMAccount( parent, name, id ),
      mAuth( "*" ),
      mPort( 0 ),
      mStorePasswd( false ),
      mUseSSL( false ),
      mUseTLS( false ),
      mAskAgain( false ),
      mPasswdDirty( false ),
      mStorePasswdInConfig( false )
  {

  }

  NetworkAccount::~NetworkAccount() {

  }

  void NetworkAccount::init() {
    KMAccount::init();

    mLogin.clear();
    mPasswd.clear();
    mAuth = '*';
    mHost.clear();
    mPort = defaultPort();
    mStorePasswd = false;
    mUseSSL = false;
    mUseTLS = false;
    mAskAgain = false;
  }

  //
  //
  // Getters and Setters
  //
  //

  void NetworkAccount::setLogin( const QString & login ) {
    mLogin = login;
  }

  QString NetworkAccount::passwd() const {
    if ( storePasswd() && mPasswd.isEmpty() )
      mOwner->readPasswords();
    return KStringHandler::obscure( mPasswd );
  }

  void NetworkAccount::setPasswd( const QString & passwd, bool storeInConfig ) {
    if ( mPasswd != KStringHandler::obscure( passwd ) ) {
      mPasswd = KStringHandler::obscure( passwd );
      mPasswdDirty = true;
    }
    setStorePasswd( storeInConfig );
  }

  void NetworkAccount::clearPasswd() {
    setPasswd( "", false );
  }

  void NetworkAccount::setAuth( const QString & auth ) {
    mAuth = auth;
  }

  void NetworkAccount::setStorePasswd( bool store ) {
    if( mStorePasswd != store && store )
      mPasswdDirty = true;
    mStorePasswd = store;
    if ( !store )
      mStorePasswdInConfig = false;
  }

  void NetworkAccount::setHost( const QString & host ) {
    mHost = host;
  }

  void NetworkAccount::setPort( unsigned short int port ) {
    mPort = port;
  }

  void NetworkAccount::setUseSSL( bool use ) {
    mUseSSL = use;
  }

  void NetworkAccount::setUseTLS( bool use ) {
    mUseTLS = use;
  }

  void NetworkAccount::setSieveConfig( const SieveConfig & config ) {
    mSieveConfig = config;
  }

  //
  //
  // read/write config
  //
  //

  void NetworkAccount::readConfig( KConfigGroup & config ) {
    KMAccount::readConfig( config );

    setLogin( config.readEntry( "login" ) );

    if ( config.readEntry( "store-passwd", false ) ) {
      mStorePasswd = true;
      QString encpasswd = config.readEntry( "pass" );
      if ( encpasswd.isEmpty() ) {
        encpasswd = config.readEntry( "passwd" );
        if ( !encpasswd.isEmpty() ) encpasswd = importPassword( encpasswd );
      }

      if ( !encpasswd.isEmpty() ) {
        setPasswd( KStringHandler::obscure( encpasswd ), true );
        mOldPassKey = encpasswd;
        mPasswdDirty = false; // set by setPasswd() on first read
        mStorePasswdInConfig = true;
      } else {
        // read password if wallet is already open, otherwise defer to on-demand loading
        if ( Wallet::isOpen( Wallet::NetworkWallet() ) )
          readPassword();
      }

    } else {
      setPasswd( "", false );
    }

    setHost( config.readEntry( "host" ) );

    unsigned int port = config.readEntry( "port", (uint)defaultPort() );
    if ( port > USHRT_MAX ) port = defaultPort();
    setPort( port );

    setAuth( config.readEntry( "auth", "*" ) );
    setUseSSL( config.readEntry( "use-ssl", false ) );
    setUseTLS( config.readEntry( "use-tls", false ) );

    mSieveConfig.readConfig( config );
  }

  void NetworkAccount::writeConfig( KConfigGroup & config ) {
    KMAccount::writeConfig( config );

    if ( storePasswd() ) {
      // write password to the wallet if possible and necessary
      bool passwdStored = false;

      //If the password is different from the one stored in the config,
      //try to store the new password in the wallet again.
      //This ensures a malicious user can't just write a dummy pass key in the
      //config, which would get overwritten by the real password and therefore
      //leak out of the more secure wallet.
      if ( mStorePasswdInConfig &&
           KStringHandler::obscure( mOldPassKey ) != passwd() )
        mStorePasswdInConfig = false;

      //If the password should be written to the wallet, do that
      if ( !mStorePasswdInConfig ) {

        //If the password is dirty, try to store it in the wallet
        if ( mPasswdDirty ) {
          Wallet *wallet = kmkernel->wallet();
          if ( wallet && wallet->writePassword( "account-" + QString::number(mId), passwd() ) == 0 )
            passwdStored = true;
        }

        //If the password isn't dirty, it is already stored in the wallet.
        else
          passwdStored = true;

        //If the password is stored in the wallet, it isn't dirty or stored in the config
        if ( passwdStored ) {
          mPasswdDirty = false;
          mStorePasswdInConfig = false;
        }
      }
      else
        passwdStored = config.hasKey("pass");

      // if wallet is not available, write to config file, since the account
      // manager deletes this group, we need to write it always
      bool writeInConfigNow = !passwdStored && mStorePasswdInConfig;
      if ( !passwdStored && !mStorePasswdInConfig ) {
        int answer = KMessageBox::warningYesNo( 0,
            i18n("KWallet is not available. It is strongly recommended to use "
                 "KWallet for managing your passwords.\n"
                 "However, KMail can store the password in its configuration "
                 "file instead. The password is stored in an obfuscated format, "
                 "but should not be considered secure from decryption efforts "
                 "if access to the configuration file is obtained.\n"
                 "Do you want to store the password for account '%1' in the "
                 "configuration file?", name() ),
            i18n("KWallet Not Available"),
            KGuiItem( i18n("Store Password") ),
            KGuiItem( i18n("Do Not Store Password") ) );
        if (answer == KMessageBox::Yes)
          writeInConfigNow = true;
        if (answer == KMessageBox::No)
          mStorePasswd = false;
      }

      if ( writeInConfigNow ) {
        config.writeEntry( "pass", KStringHandler::obscure( passwd() ) );
        mOldPassKey = KStringHandler::obscure( passwd() );
        mStorePasswdInConfig = true;
      }
    }

    // delete password from the wallet if password storage is disabled
    if (!storePasswd() && !Wallet::keyDoesNotExist(
        Wallet::NetworkWallet(), "kmail", "account-" + QString::number(mId))) {
      Wallet *wallet = kmkernel->wallet();
      if (wallet)
        wallet->removeEntry( "account-" + QString::number(mId) );
    }

    // delete password from config file if it is stored in the wallet or
    // not stored at all
    if ( !mStorePasswdInConfig )
      config.deleteEntry( "pass" );

    config.writeEntry( "store-passwd", storePasswd() );
    config.writeEntry( "login", login() );
    config.writeEntry( "host", host() );
    config.writeEntry( "port", static_cast<unsigned int>( port() ) );
    config.writeEntry( "auth", auth() );
    config.writeEntry( "use-ssl", useSSL() );
    config.writeEntry( "use-tls", useTLS() );

    mSieveConfig.writeConfig( config );
  }

  //
  //
  // Network processing
  //
  //

  KUrl NetworkAccount::getUrl() const {
    KUrl url;
    url.setProtocol( protocol() );
    url.setUser( login() );
    url.setPass( passwd() );
    url.setHost( host() );
    url.setPort( port() );
    return url;
  }

  MetaData NetworkAccount::slaveConfig() const {
    MetaData m;
    m.insert( "tls", useTLS() ? "on" : "off" );
    return m;
  }

  void NetworkAccount::pseudoAssign( const KMAccount * a ) {
    KMAccount::pseudoAssign( a );

    const NetworkAccount * n = dynamic_cast<const NetworkAccount*>( a );
    if ( !n ) return;

    setLogin( n->login() );
    setPasswd( n->passwd(), n->storePasswd() );
    setHost( n->host() );
    setPort( n->port() );
    setAuth( n->auth() );
    setUseSSL( n->useSSL() );
    setUseTLS( n->useTLS() );
    setSieveConfig( n->sieveConfig() );
  }

  void NetworkAccount::readPassword() {
    if ( !storePasswd() )
      return;

    // ### workaround for broken Wallet::keyDoesNotExist() which returns wrong
    // results for new entries without closing and reopening the wallet
    if ( Wallet::isOpen( Wallet::NetworkWallet() ) )
    {
       Wallet *wallet = kmkernel->wallet();
       if (!wallet || !wallet->hasEntry( "account-" + QString::number(mId) ) )
         return;
    }
    else
    {
       if (Wallet::keyDoesNotExist( Wallet::NetworkWallet(), "kmail", "account-" + QString::number(mId) ) )
         return;
    }

    if ( kmkernel->wallet() ) {
      QString passwd;
      kmkernel->wallet()->readPassword( "account-" + QString::number(mId), passwd );
      setPasswd( passwd, true );
      mPasswdDirty = false;
    }
  }

  void NetworkAccount::setCheckingMail( bool checking )
  {
      mCheckingMail = checking;
      if ( host().isEmpty() )
          return;
    ServerConnectionsHash::Iterator serverConnectionsIt( s_serverConnections->find( host() ) );
    if ( checking ) {
        if ( serverConnectionsIt != s_serverConnections->end() )
            *serverConnectionsIt += 1;
        else
            s_serverConnections->insert( host(), 1 );
        kDebug() << "check mail started - connections for host"
                << host() << "now is"
                << s_serverConnections->value( host() );
    } else {
            if ( serverConnectionsIt != s_serverConnections->end() &&
                *serverConnectionsIt > 0 ) {
                *serverConnectionsIt -= 1;
                kDebug() << "connections to server" << host()
                        << "now" << *serverConnectionsIt;
            }
    }
}

  bool NetworkAccount::mailCheckCanProceed() const
  {
      bool offlineMode = KMKernel::isOffline();

      ServerConnectionsHash::Iterator serverConnectionsIt( s_serverConnections->find( host() ) );
      kDebug() << "for host" << host()
              << "current connections="
              << (serverConnectionsIt==s_serverConnections->end() ? 0 : *serverConnectionsIt)
              << "and limit is" << GlobalSettings::self()->maxConnectionsPerHost();
      bool connectionLimitForHostReached = !host().isEmpty()
              && GlobalSettings::self()->maxConnectionsPerHost() > 0
              && serverConnectionsIt!=s_serverConnections->end()
              && *serverConnectionsIt >= GlobalSettings::self()->maxConnectionsPerHost();
      kDebug() << "connection limit reached:"
              << connectionLimitForHostReached;

      return ( !connectionLimitForHostReached && !offlineMode );
  }

  void NetworkAccount::resetConnectionList( NetworkAccount* acct )
  {
    s_serverConnections->remove( acct->host() );
  }

} // namespace KMail

#include "networkaccount.moc"
