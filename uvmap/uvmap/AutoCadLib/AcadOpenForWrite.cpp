#include "pch.h"
#include "AcadOpenForWrite.h"

CAcadOpenForWrite::CAcadOpenForWrite(void)
{
    acDocManager->lockDocument(curDoc(), AcAp::kWrite, NULL, NULL, true);
}

CAcadOpenForWrite::~CAcadOpenForWrite(void)
{
    acDocManager->unlockDocument(curDoc());
}
