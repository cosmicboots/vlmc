/*****************************************************************************
 * StringWidget: Handle text settings.
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
 *
 * Authors: Hugo Beauzée-Luyssen <hugo@vlmc.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifndef STRINGWIDGET_H
#define STRINGWIDGET_H

#include "ISettingsCategoryWidget.h"
#include <stddef.h>

class   SettingValue;

class   QLineEdit;

class   StringWidget : public ISettingsCategoryWidget
{
    Q_OBJECT

    public:
        StringWidget( SettingValue *s, QWidget *parent = NULL );
        void                    save();

    private slots:
        virtual void            changed( const QVariant& );

    private:
        QLineEdit               *m_lineEdit;
};

#endif // STRINGWIDGET_H
