#include "export.h"
#include "DisplayWin32.h"
#include "Game.h"
#include "SolarSystemComponent.h"

// Added preprocessor property in project properties -> C/C++ -> Preprocessor -> Preprocessor Definitions NOMINMAX

int main()
{
    SolarSystemComponent solarSystemInstance;
    solarSystemInstance.Run();
}