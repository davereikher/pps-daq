#include "Event.h"
#include "Exception.h"
#include "EventConstants.h"
#include "CAENDigitizer.h"

ClassDef(Event)

Event::Event()
{}

Event::Event(CAEN_DGTZ_X742_EVENT_t* a_pEvent)
{
	Parse(a_pEvent);
}

void Event::Parse(CAEN_DGTZ_X742_EVENT_t* a_pEvent)
{

}
