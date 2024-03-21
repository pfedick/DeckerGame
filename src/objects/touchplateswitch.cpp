#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "audiopool.h"
#include "player.h"

namespace Decker::Objects {

Representation TouchPlateSwitch::representation()
{
    return Representation(Spriteset::Switches, 44);
}


}	// EOF namespace Decker::Objects
