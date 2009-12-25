/*****************************************************************************
 * VideoClipWorkflow.cpp : Clip workflow. Will extract a single frame from a VLCMedia
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Hugo Beauzee-Luyssen <hugo@vlmc.org>
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

#include "VideoClipWorkflow.h"
#include "MainWorkflow.h"
#include "StackedBuffer.hpp"

VideoClipWorkflow::VideoClipWorkflow( Clip* clip ) : ClipWorkflow( clip )
{
    for ( unsigned int i = 0; i < VideoClipWorkflow::nbBuffers; ++i )
    {
        //Actually we don't release, we extend pool, but the release method do exactly what we want.
        m_availableBuffers.push_back( new LightVideoFrame( MainWorkflow::getInstance()->getWidth()
                                    * MainWorkflow::getInstance()->getHeight()
                                    * Pixel::NbComposantes ) );
    }
//    m_availableBuffersLock = new QReadWriteLock;
//    m_computedBuffersLock = new QReadWriteLock;
}

VideoClipWorkflow::~VideoClipWorkflow()
{
    while ( m_availableBuffers.isEmpty() == false )
        delete m_availableBuffers.pop();
    while ( m_computedBuffers.isEmpty() == false )
        delete m_computedBuffers.pop();
//    delete m_availableBuffersLock;
//    delete m_computedBuffersLock;
}

void            VideoClipWorkflow::initVlcOutput()
{
    char        buffer[32];

    m_vlcMedia->addOption( ":no-audio" );
    m_vlcMedia->addOption( ":no-sout-audio" );
    m_vlcMedia->addOption( ":sout=#transcode{}:smem" );
    m_vlcMedia->setVideoDataCtx( this );
    m_vlcMedia->setVideoLockCallback( reinterpret_cast<void*>( getLockCallback() ) );
    m_vlcMedia->setVideoUnlockCallback( reinterpret_cast<void*>( getUnlockCallback() ) );
    m_vlcMedia->addOption( ":sout-transcode-vcodec=RV24" );
    m_vlcMedia->addOption( ":sout-transcode-acodec=s16l" );
//    m_vlcMedia->addOption( ":no-sout-keep" );

    m_vlcMedia->addOption( ":no-sout-smem-time-sync" );

    sprintf( buffer, ":sout-transcode-width=%i", MainWorkflow::getInstance()->getWidth() );
    m_vlcMedia->addOption( buffer );

    sprintf( buffer, ":sout-transcode-height=%i", MainWorkflow::getInstance()->getHeight() );
    m_vlcMedia->addOption( buffer );

    //Forced output fps
    sprintf( buffer, ":sout-transcode-fps=%f", (float)Clip::DefaultFPS );
    m_vlcMedia->addOption( buffer );
}

void*       VideoClipWorkflow::getLockCallback()
{
    return reinterpret_cast<void*>(&VideoClipWorkflow::lock);
}

void*       VideoClipWorkflow::getUnlockCallback()
{
    return reinterpret_cast<void*>( &VideoClipWorkflow::unlock );
}

void*       VideoClipWorkflow::getOutput( ClipWorkflow::GetMode mode )
{
    preGetOutput();
    //Don't lock before preGetOutput() : it can eventually waits for the clipworkflow to render a frame
    //which requires the renderLock to be unheld.
    QMutexLocker    lock( m_renderLock );

    qWarning() << "poping buffer";
    if ( isEndReached() == true )
        return NULL;
    StackedBuffer<LightVideoFrame*>* buff;
    if ( mode == ClipWorkflow::Pop )
        buff = new StackedBuffer<LightVideoFrame*>( m_computedBuffers.pop(), &m_availableBuffers, true );
    else if ( mode == ClipWorkflow::Get )
        buff = new StackedBuffer<LightVideoFrame*>( m_computedBuffers.head(), NULL, false );
    ClipWorkflow::postGetOutput();
    return buff;
}

void    VideoClipWorkflow::lock( VideoClipWorkflow* cw, void** pp_ret, int size )
{
    Q_UNUSED( size );
    cw->m_renderLock->lock();
    LightVideoFrame*    lvf = cw->m_availableBuffers.pop();
    cw->m_computedBuffers.push_back( lvf );
    qWarning() << "feeding video buffer";
    *pp_ret = (*(lvf))->frame.octets;
}

void    VideoClipWorkflow::unlock( VideoClipWorkflow* cw, void* buffer, int width, int height, int bpp, int size, qint64 pts )
{
    Q_UNUSED( buffer );
    Q_UNUSED( width );
    Q_UNUSED( height );
    Q_UNUSED( bpp );
    Q_UNUSED( size );

    cw->computePtsDiff( pts );
    LightVideoFrame*    lvf = cw->m_computedBuffers.head();
    (*(lvf))->ptsDiff = cw->m_currentPts - cw->m_previousPts;
    cw->m_renderLock->unlock();
    //If this is the first buffer that has been rendered, there may be a waiting TrackWorkflow.
    cw->commonUnlock();
}

uint32_t    VideoClipWorkflow::getAvailableBuffers() const
{
    return m_availableBuffers.count();
}

uint32_t    VideoClipWorkflow::getComputedBuffers() const
{
    return m_computedBuffers.count();
}
