#ifndef _ABBROWSER_CONDUIT_H
#define _ABBROWSER_CONDUIT_H
// abbrowser-conduit.cc
//
// Copyright (C) 2000,2001 by Dan Pilone
// Copyright (C) 2000 Gregory Stern
// Copyright (C) 2002 by Reinhold Kainhofer
//
// This file is distributed under the Gnu General Public Licence (GPL).
// The GPL should have been included with this file in a file called
// COPYING. 
//
// $Revision$
//

/*
** Bug reports and questions can be sent to kde-pim@kde.org
*/



#include <plugin.h>

#include <qmap.h>
#include <qptrlist.h>

#include <kapplication.h>
#include <qdict.h>

#include <pilotAddress.h>
#include <kabc/addressbook.h>
#include <kabc/addressee.h>

#define SYNCNONE 0
#define SYNCMOD 1
#define SYNCDEL 3


#define SYNC_FULL 1
#define SYNC_FAST 0
#define SYNC_FIRST 2

#define CHANGED_NONE 0
#define CHANGED_PC 1
#define CHANGED_PALM 2
#define CHANGED_BOTH CHANGED_PC|CHANGED_PALM
#define CHANGED_ADD  0x100
#define CHANGED_NORES 0x200
#define CHANGED_DUPLICATE CHANGED_ADD|CHANGED_NORES|CHANGED_BOTH

//#define ADD_BOTH CHANGED_BOTH|CHANGED_ADD

using namespace KABC;

//class DCOPClient;
class PilotLocalDatabase;
class PilotSerialDatabase;

class AbbrowserConduit : public ConduitAction
{
Q_OBJECT
public:
	AbbrowserConduit(
		KPilotDeviceLink *o,
		const char *n = 0L,
		const QStringList &a = QStringList() );
	virtual ~AbbrowserConduit();
	virtual bool exec();

protected slots:
	void syncDeletedRecord();
	void syncPCRecToPalm();
	void syncPalmRecToPC();
	void cleanup();

private:
	enum EConflictResolution {
		eUserChoose=0,
		eDoNotResolve,
		ePilotOverides,
		eAbbrowserOverides,
		eRevertToBackup,
		eKeepBothInAbbrowser
	};
	EConflictResolution getResolveConflictOption() const { return fConflictResolution; }
	bool doSmartMerge() const { return fSmartMerge; }
	
	EConflictResolution getEntryResolution(const KABC::Addressee & abEntry, const PilotAddress &backupAddress, const PilotAddress & pilotAddress);
	EConflictResolution getFieldResolution(const QString &entry, const QString &field, const QString &palm, const QString &backup, const QString &pc);
	EConflictResolution ResolutionDialog(QString Title, QString Text, QStringList &lst, QString remember="", bool*rem=0L) const;

	int _conflict(const QString &entry, const QString &field, const QString &pc, const QString &backup, 
			const QString &palm, bool & mergeNeeded, QString & mergedStr);
	int _compare(const QString &str1, const QString &str2) const;

	/**
	*  @return the Abbrowser Contact field to map the pilot "other" phone
	*  field to (such as BusinessFax, etc)
	*/
	static bool isPilotStreetHome()  { return fPilotStreetHome; }
	static bool isPilotFaxHome()  { return fPilotFaxHome; }



	/**
	*  Do the preperations before doSync or doBackup.
	*  Load contacts, set the pilot 
	*/
	bool _prepare();

	/**
	* Read the global KPilot config file for settings
	* particular to the AbbrowserConduit conduit.
	*/
	void readConfig();

	/**
	*  Load the contacts from the addressbook.
	*  @return true if successful, false if not
	*/
	bool _loadAddressBook();
	/**
	*  Save the contacts back to the addressbook.
	*  @return true if successful, false if not
	*/
	bool _saveAddressBook();
	
	static QString getOtherField(const KABC::Addressee&abEntry);
	static void setOtherField(KABC::Addressee&abEntry, QString nr);
	static KABC::PhoneNumber getFax(const KABC::Addressee &abEntry);
	static KABC::Address getAddress(const KABC::Addressee &abEntry);
	

	void _setAppInfo();
	KABC::Addressee _addToAbbrowser(const PilotAddress & address);
	int _mergeEntries(PilotAddress &pilotAddress, PilotAddress &backupAddress, KABC::Addressee &abEntry);
	int _handleConflict(PilotAddress &piAddress, PilotAddress &backup, KABC::Addressee &abEntry);
	int _smartMerge(PilotAddress & outPilotAddress, const PilotAddress & backupAddress, KABC::Addressee & outAbEntry);
	int _smartMergePhone(KABC::Addressee &abEntry, const PilotAddress &backupAddress, 
		PilotAddress &pilotAddress, PilotAddress::EPhoneType PalmFlag, KABC::PhoneNumber phone, QString
		 thisName, QString name);
	int _smartMergeEntry(QString abEntry, const PilotAddress &backupAddress, PilotAddress &pilotAddress, int PalmFlag, QString thisName, QString name, QString &mergedString);
	int _smartMergeCategories(KABC::Addressee &abAddress, const PilotAddress &backupAddress, PilotAddress &pilotAddress, QString thisName, QString name, QString &mergedString);

	void _removePilotAddress(PilotAddress &address);
	void _removeAbEntry(KABC::Addressee addressee);
	KABC::Addressee _saveAbEntry(KABC::Addressee &abEntry);
	void _checkDelete(PilotRecord *r, PilotRecord *s);

   /** 
	*  @return true if the abbEntry's pilot id was changed 
	*/
	bool _savePilotAddress(PilotAddress &address, KABC::Addressee &abEntry);
	bool _saveBackupAddress(PilotAddress & backup);
	
	void _copyPhone(KABC::Addressee &toAbEntry, KABC::PhoneNumber phone, QString palmphone);
	void _copy(PilotAddress &toPilotAddr, KABC::Addressee &fromAbEntry);
	void _copy(KABC::Addressee &toAbEntry, const PilotAddress &fromPilotAddr);
	void _setPilotAddress(PilotAddress &toPilotAddr, const KABC::Address &abAddress);
	bool _equal(const PilotAddress &piAddress, KABC::Addressee &abEntry) const;
	KABC::Addressee _findMatch(const PilotAddress & pilotAddress) const;
	int _getCat(const QStringList cats) const ;
	void _setCategory(KABC::Addressee &abEntry, QString cat);

   /** 
	*  Given a list of contacts, creates the pilot id to contact key map
	*  and a list of new contacts in O(n) time (single pass)
	*/
   void _mapContactsToPilot( QMap < recordid_t, QString> &idContactMap) const;

#ifdef DEBUG
	/** 
	* Output to console, for debugging only 
	*/
	static void showAddressee(const KABC::Addressee &abAddress);
	/** 
	* Output to console, for debugging only 
	*/
	static void showPilotAddress(const PilotAddress &pilotAddress);
#endif

	KABC::Addressee _addToPC(PilotRecord *r);
	KABC::Addressee _changeOnPC(PilotRecord*rec, PilotRecord*backup);
	bool _deleteOnPC(PilotRecord*rec,PilotRecord*backup);

	void _addToPalm(KABC::Addressee &entry);
	void _changeOnPalm(PilotRecord *rec, PilotRecord* backuprec, KABC::Addressee &ad);
	void _deleteFromPalm(PilotRecord*rec);

	struct AddressAppInfo fAddressAppInfo;

	bool fSmartMerge;
	EConflictResolution fConflictResolution, fEntryResolution;
	static bool fPilotStreetHome, fPilotFaxHome;
	bool fFullSync, fFirstTime, fArchive;
   static enum  ePilotOtherEnum
	{
		eOtherPhone,
		eAssistant,
//		eBusiness2,
		eBusinessFax,
		eCarPhone,
		eEmail2,
		eHomeFax,
//		eHomePhone2,
		eTelex,
		eTTYTTDPhone
	} ePilotOther;
	int syncAction; 
	int pilotindex;
	bool abChanged;
	static const QString appString;
	static const QString flagString;
	static const QString idString;
	QMap < recordid_t, QString> addresseeMap;
	QValueList <recordid_t> syncedIds;
	KABC::AddressBook* aBook;
	KABC::AddressBook::Iterator abiter;
	
	
	
	
	
	
	void showAdresses(PilotAddress & pilotAddress, const PilotAddress & backupAddress, KABC::Addressee & abEntry);
} ;




// $Log$
// Revision 1.28  2002/10/10 13:44:41  kainhofe
// This fixes several bugs:
// -) conflict resolution now also works if you chose ignore on the last sync
// -) home/work phone/fax were mixed up
// -) deleting an address in kaddressbook now also deletes the address from the handheld
// -) variable renaming for consistent naming
// -) fix a crash with an iterator being deleted and then incremented
// -) Offering the value from the last sync in the conflict resolution dialog
// -) Using an addressbook for several handhelds should work now
// -) archived records are now synced to the PC, but not back to the handheld
// -) If an addressee has a wrong pilotID, the pilotID is reset
//
// Revision 1.27  2002/10/05 13:59:29  kainhofe
// duplication now works as conflict resolution. Removed the warning in the setup dialog.
//
// Revision 1.26  2002/09/12 13:58:20  kainhofe
// some more fixes, still does not do any sync unless compiled with -NDO_DANGEROUS_ABOOK_SYNC . Most things work, except for several conflict resolution settings
//
// Revision 1.25  2002/08/23 22:59:29  kainhofe
// Implemented Adriaan's change 'signal: void exec()' -> 'bool exec()' for "my" conduits
//
// Revision 1.24  2002/08/16 18:54:06  kainhofe
// Tried to fix the conduit, but KABC has changed so much that the conduit was totaly broken again. I will not touch the conduit any more until KABC is really solid and doesn't change any more!!!!
//
// Revision 1.23  2002/08/15 21:48:59  kainhofe
// removed deprecated files (ContactEntry class and DCOP things)
//
// Revision 1.22  2002/08/15 21:40:14  kainhofe
// some more work in the addressbook conduit. Does not yet work
//
// Revision 1.21  2002/07/23 00:52:02  kainhofe
// Reorder the resolution methods
//
// Revision 1.20  2002/07/20 18:50:45  kainhofe
// added a terrible hack to add new contacts to the addressbook. Need to fix kabc for this...
//
// Revision 1.19  2002/07/09 22:40:18  kainhofe
// backup database fixes, prevent duplicate vcal entries, fixed the empty record that was inserted on the palm on every sync
//
// Revision 1.18  2002/07/01 23:25:46  kainhofe
// implemented categories syncing, many things seem to work, but still every sync creates an empty zombie.
//
// Revision 1.17  2002/06/30 22:17:50  kainhofe
// some cleanup. Changes from the palm are still not applied to the pc, pc->palm still disabled.
//
// Revision 1.16  2002/06/30 16:23:23  kainhofe
// Started rewriting the addressbook conduit to use libkabc instead of direct dcop communication with abbrowser. Palm->PC is enabled (but still creates duplicate addresses), the rest is completely untested and thus disabled for now
//
// Revision 1.15  2002/05/15 17:15:32  gioele
// kapp.h -> kapplication.h
// I have removed KDE_VERSION checks because all that files included "options.h"
// which #includes <kapplication.h> (which is present also in KDE_2).
// BTW you can't have KDE_VERSION defined if you do not include
// - <kapplication.h>: KDE3 + KDE2 compatible
// - <kdeversion.h>: KDE3 only compatible
//
// Revision 1.14  2002/04/16 18:22:12  adridg
// Wishlist fix from David B: handle formatted names when syncing
//
// Revision 1.13  2001/12/10 22:10:17  adridg
// Make the conduit compile, for Danimo, but it may not work
//
// Revision 1.12  2001/10/31 23:54:45  adridg
// CVS_SILENT: Ongoing conduits ports
//
#endif
