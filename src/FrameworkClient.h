#pragma once

namespace bridge
{
    enum class OpenResult
    {
        Opened,
        FrameworkMissing,
        UnsupportedFramework,
        NotInitialized
    };

    // Uses the framework's public DLL ABI; it does not simulate input or patch memory.
    OpenResult OpenFrameworkMainWindow();
    const char* Describe(OpenResult result);
}

