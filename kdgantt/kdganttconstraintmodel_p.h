/****************************************************************************
 ** Copyright (C) 2001-2006 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Gantt library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KD Gantt licenses may use this file in
 ** accordance with the KD Gantt Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.kdab.net/kdgantt for
 **   information about KD Gantt Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/
#ifndef KDGANTTCONSTRAINTMODEL_P_H
#define KDGANTTCONSTRAINTMODEL_P_H

#include "kdganttconstraintmodel.h"

#include <QSet>
#include <QMultiHash>
#include <QPersistentModelIndex>

namespace KDGantt {
    class ConstraintModel::Private {
    public:
        Private();

        void addConstraintToIndex( const QPersistentModelIndex& idx, const Constraint& c );
        void removeConstraintFromIndex( const QPersistentModelIndex& idx,  const Constraint& c );

        QSet<Constraint> constraints;
        QMultiHash<QPersistentModelIndex,Constraint> indexMap;
    };
}

#endif /* KDGANTTCONSTRAINTMODEL_P_H */

