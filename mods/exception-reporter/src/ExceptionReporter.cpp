#include "ExceptionReporter.h"


ExceptionReporter::ExceptionReporter(HMODULE module)
    :
    Mod(module)
{
}

void ExceptionReporter::OnProcessAttach()
{
}

void ExceptionReporter::OnProcessDetach()
{
}

void ExceptionReporter::OnThreadAttach(HANDLE thread)
{
}

void ExceptionReporter::OnThreadDetach(HANDLE thread)
{
}