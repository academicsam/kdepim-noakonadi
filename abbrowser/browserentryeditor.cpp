#include <Entity.h>
#include "browserentryeditor.h"

////////////////////////////
// PabContactDialog Methods

PabContactDialog::PabContactDialog( QWidget *parent, 
				    const char *name, 
				    QString entryKey,
            Entity *entry,
				    bool modal )
  : ContactDialog( parent, name, new Entity( *entry ) , modal ),
    entryKey_( entryKey )
{
}

PabContactDialog::~PabContactDialog()
{
  delete ce;
}

void PabContactDialog::accept()
{
  emit change( entryKey_, ce );
  ContactDialog::accept();
}

////////////////////////////
// PabNewContactDialog Methods

PabNewContactDialog::PabNewContactDialog( QWidget *parent, 
					  const char *name, 
					  bool modal )
  : ContactDialog( parent, name, new Entity, modal )
{}

PabNewContactDialog::~PabNewContactDialog()
{
  delete ce;
}

void PabNewContactDialog::accept()
{
  emit add( ce );
  ContactDialog::accept();
}
