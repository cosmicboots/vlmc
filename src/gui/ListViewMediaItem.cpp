/*****************************************************************************
 * ListViewMediaItem.cpp: VLMC LibraryWidget
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Christophe Courtaut <christophe.courtaut@gmail.com>
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

#include "ListViewMediaItem.h"
#include <QPixmap>
#include <QDebug>

ListViewMediaItem::ListViewMediaItem( QFileInfo* fInfo, ListViewMediaItem::fType fType, QListWidget* parent, int type ) :
        QListWidgetItem( parent, type )
{
    m_fileInfo = fInfo;
    m_fileType = fType;

    setIcon( QIcon( ":/images/images/vlmc.png" ) );
    setText( fInfo->baseName() );

    QWidget* renderWidget = new QWidget();

    m_currentMedia = new InputMedia( "file://" + fInfo->absoluteFilePath() );
    m_currentMedia->setupMedia();
    m_currentMedia->setDrawable( renderWidget->winId() );
    m_currentMedia->play();

    connect( m_currentMedia->mediaPlayer(), SIGNAL( playing() ), this, SLOT( setSnapshot() ) );
}

ListViewMediaItem::~ListViewMediaItem()
{
    delete m_fileInfo;
}

void    ListViewMediaItem::setSnapshot()
{
    m_currentMedia->setTime( 50000 );
    connect( m_currentMedia->mediaPlayer(), SIGNAL( timeChanged() ), this, SLOT( takeSnapshot() ) );
}

void    ListViewMediaItem::takeSnapshot()
{
    QPixmap* snapshot = m_currentMedia->takeSnapshot( 32, 32 );
    setIcon( QIcon( *snapshot ) );
    m_currentMedia->stop();
    disconnect( m_currentMedia->mediaPlayer(), SIGNAL( playing() ), this, SLOT( setSnapshot() ) );
    disconnect( m_currentMedia->mediaPlayer(), SIGNAL( playing() ), this, SLOT( takeSnapshot() ) );
}
