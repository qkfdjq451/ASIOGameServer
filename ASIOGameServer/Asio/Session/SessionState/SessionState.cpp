#include "../../../Global.h"
#include "SessionState.h"

SessionState::SessionState(std::shared_ptr<class Session> _session)
{
	session = _session;
}
