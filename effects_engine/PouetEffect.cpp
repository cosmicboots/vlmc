#include "PouetEffect.h"

char const * 	PouetEffect::m_videoInputsNames[] = {"in"};
char const *	PouetEffect::m_videoOutputsNames[] = {"out"};
 
PouetEffect::PouetEffect() : GenericEffect(
					   PouetEffect::m_videoInputsNames, PouetEffect::m_nbVideoInputs,
					   PouetEffect::m_videoOutputsNames, PouetEffect::m_nbVideoOutputs
					   )
{
};

PouetEffect::~PouetEffect()
{
}

void	PouetEffect::render( void )
{
  LightVideoFrame	lol;
  VideoFrame		tmp;
  quint32		size;

  (m_videoInputs["in"]) >> lol;
  tmp = lol;
  tmp.truncate( tmp.size() / 2 );
  (m_videoOutputs["out"]) << tmp;
  return ;
}

