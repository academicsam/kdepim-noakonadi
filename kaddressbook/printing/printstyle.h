/* -*- C++ -*-
   This file declares the abstract print style base class.

   the KDE addressbook

   $ Author: Mirko Boehm $
   $ Copyright: (C) 1996-2002, Mirko Boehm $
   $ Contact: mirko@kde.org
         http://www.kde.org $
   $ License: LGPL with the following explicit clarification:
         This code may be linked against any version of the Qt toolkit
         from Troll Tech, Norway. $

   $Revision$
*/

#ifndef PRINTSTYLE_H
#define PRINTSTYLE_H

#include <qwidget.h>
#include <qstringlist.h>
#include <qpixmap.h>

#include <kabc/field.h>

namespace KABPrinting {

class PrintingWizard;
class PrintProgress;

/**
 The class PrintStyle implements the abstract interface to the
 PrintingWizards style objects.
 To implement a print style, derive from this class and read
 the information in printingwizard.h to see how this two pieces
 work together. Basically, the print style gets the contacts it
 is supposed to print from the PrintingWizard is will not
 change this set - neither its content nor its order.
 To register your new style in the printing wizard, you need to
 define a PrintStyleFactory that handles how your objects are
 created and deleted. See the existing print styles for
 examples.
 A print style should have a preview image that gives the user
 a basic impression on how it will look. Add this image to the
 printing folder (right here :-), and edit Makefile.am to have
 it installed along with kaddressbook. Load it using
 setPreview(QString).
 Your print style is supposed to add its options as pages to
 the printing wizard. The method wizard() gives you a pointer
 to the wizard object.
 */

class PrintStyle : public QObject
{
  Q_OBJECT

  public:
    PrintStyle( PrintingWizard* parent, const char* name = 0 );
    virtual ~PrintStyle();

    /**
     Reimplement this method to actually print.
     */
    virtual void print( KABC::Addressee::List &contacts, PrintProgress* ) = 0;

    /**
     Reimplement this method to provide a preview of what will
     be printed. It returns an invalid QPixmap by default,
     resulting in a message that no preview is available.
     */
    const QPixmap& preview();

    /**
     Hide all style specific pages in the wizard.
     */
    void hidePages();

    /**
     Show all style specific pages in the wizard.
     */
    void showPages();

    /**
      Returns the preferred sort criterion field.
     */
    KABC::Field* preferredSortField();

    /**
      Returns the preferred sort type.
      
      true = ascending
      false = descending
     */
    bool preferredSortType();

  protected:
    /**
     Load the preview image from the kaddressbook data
     directory. The image should be located in the subdirectory
     "printing". Give only the file name without any prefix as
     the parameter.
     */
    bool setPreview( const QString& fileName );

    /**
     Set the preview image.
     */
    void setPreview( const QPixmap& image );

    /**
      Set preferred sort options for this printing style.
     */
    void setPreferredSortOptions( KABC::Field *field, bool ascending = true );

    /**
     Return the wizard object.
     */
    PrintingWizard *wizard();

    /**
     Add additional page to the wizard e.g. a configuration page for
     the style.
     */
    void addPage( QWidget *page, const QString &title );

  private:
    PrintingWizard *mWizard;
    QPixmap mPreview;
    QPtrList<QWidget> mPageList;
    QStringList mPageTitles;

    KABC::Field *mSortField;
    bool mSortType;
};


/**
 The factories are used to have all object of the respective
 print style created in one place.
 This will maybe be changed to a template because of its simple
 nature :-)
*/
class PrintStyleFactory
{
  public:
    PrintStyleFactory( PrintingWizard* parent, const char* name = 0 );
    virtual ~PrintStyleFactory();
    virtual PrintStyle *create() = 0;

    /**
     Overload this method to provide a one-liner description
     for your print style.
     */
    virtual QString description() = 0;

  protected:
    PrintingWizard* mParent;
    const char* mName;
};

}

#endif
