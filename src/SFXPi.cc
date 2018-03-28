/**********************************************************************
 * @file SFXPi.cc
 * @author TomatoPi
 * @version 1.0
 *
 * Main
 **********************************************************************/
#include <iostream>
#include <unistd.h>

#include "types.h"
#include "log.h"

#include "effect/EffectFactory.h"
#include "effect/EffectHandler.h"

#include "preset/PresetHandler.h"

#include "commands/CommandHandler.h"

#include "event/EventHandler.h"

#include "logic/LogicHandler.h"

#include "analog/AnalogHandler.h"

#include "gui/GuiHandler.h"

using namespace SFXP;

#ifdef __DEBUG__
bool SFXP::GlobalIsDebugEnabled = true;
#endif
bool SFXP::GlobalNeedToExit     = false;
int  SFXP::GlobalErrState       = 0;

int main(int argc, char* argv[]){

    cout << endl <<
    "******************************************************************"
    << endl <<
    "******************************************************************"
    << endl <<
    "   SFX-Pi : The Embedded System"
    << endl <<
    "   Program Version : ";
    cout.width(5); cout << left << SFXP::VERSION << endl <<
    "   Build : " << SFXP::BUILD << endl <<
    #ifdef __DEBUG__
    "   DEBUG BUILD : Use \'make RELEASE=true\' to build a release"
    << endl <<
    #endif
    "******************************************************************"
    << endl <<
    "******************************************************************"
    << endl << endl;

    //// PARSE START ARGS ////
    bool argNogui   = false; // Doesn't enable GUI Handler if true
    bool argNoio    = false; // Doesn't enable Physical IO if true
    bool argNojack  = false; // Doesn't enable Audio if true
    SFXPlog::log("SFX-Pi") << "Parse Launch Args :";
    for (int i = 0; i < argc; i++){

        if (strcmp(argv[i], "-nogui") == 0) {

            cout << " (Console)";
            argNogui = true;
        }
        else if (strcmp(argv[i], "-noio") == 0){

            cout << " (Headless)";
            argNoio = true;
        }
        else if (strcmp(argv[i], "-nojack") == 0) {

            cout << " (No Audio)";
            argNojack = true;
        }
        else if (strcmp(argv[i], "-editor") == 0) {

            cout << " (Preset Editor Mode)";
            argNoio = true;
            argNojack = true;
        }
    }
    if (!(argNogui || argNoio || argNojack)) {

        cout << " Standard Run";
    }
    cout << '\n';

    //// BUILD PROGRAM ////
    PresetHandler*  PRESET_HANDLER  (nullptr);
    EffectHandler*  EFFECT_HANDLER  (nullptr);
    CommandHandler* COMMAND_HANDLER (nullptr);
    EventHandler*   EVENT_HANDLER   (nullptr);
    LogicHandler*   LOGIC_HANDLER   (nullptr);
    AnalogHandler*  ANALOG_HANDLER  (nullptr);
    GuiHandler*     GUI_HANDLER     (nullptr);
    try {
        
        // REGISTER PLUGIN LIST //
        if (EffectFactory::buildRegistry())
            throw string("Failed Build Pugin Registry");

        // CREATE PRESET HANDLER //
        PRESET_HANDLER = new PresetHandler();
        if (PRESET_HANDLER->errored())
            throw string("Failed Build Preset Handler");

        // CREATE EFFECT HANDLER //
        EFFECT_HANDLER = new EffectHandler(argNojack);
        if (EFFECT_HANDLER->errored())
            throw string("Failed Build Effect Handler");

        // CREATE COMMAND HANDLER //
        COMMAND_HANDLER = new CommandHandler();
        if (COMMAND_HANDLER->errored())
            SFXPlog::err("SFXPi") << "Failed Build Command Handler" << endl;

        // CREATE EVENT HANDLER //
        EVENT_HANDLER = new EventHandler();
        if (EVENT_HANDLER->errored())
            throw string("Failed Build Event Handler");

        // CREATE LOGIC HANDLER //
        LOGIC_HANDLER = new LogicHandler(argNoio);
        if (LOGIC_HANDLER->errored())
            throw string("Failed Build Logic Handler");

        // CREATE ANALOG HANDLER //
        ANALOG_HANDLER = new AnalogHandler(argNoio);
        if (ANALOG_HANDLER->errored())
            throw string("Failed Build Analog Handler");

        // CREATE ANALOG HANDLER //
        GUI_HANDLER = new GuiHandler(argNogui);
        if (GUI_HANDLER->errored())
            throw string("Failed Build Gui Handler");

        // ATTACH HANDLERS TOGETHER //
        PRESET_HANDLER->attachPresetHandler (PRESET_HANDLER);
        PRESET_HANDLER->attachEffectHandler (EFFECT_HANDLER);
        PRESET_HANDLER->attachCommandHandler(COMMAND_HANDLER);
        PRESET_HANDLER->attachEventHandler  (EVENT_HANDLER);
        PRESET_HANDLER->attachLogicHandler  (LOGIC_HANDLER);
        PRESET_HANDLER->attachAnalogHandler (ANALOG_HANDLER);
        PRESET_HANDLER->attachGuiHandler    (GUI_HANDLER);

        EFFECT_HANDLER->attachPresetHandler (PRESET_HANDLER);
        EFFECT_HANDLER->attachEffectHandler (EFFECT_HANDLER);
        EFFECT_HANDLER->attachCommandHandler(COMMAND_HANDLER);
        EFFECT_HANDLER->attachEventHandler  (EVENT_HANDLER);
        EFFECT_HANDLER->attachLogicHandler  (LOGIC_HANDLER);
        EFFECT_HANDLER->attachAnalogHandler (ANALOG_HANDLER);
        EFFECT_HANDLER->attachGuiHandler    (GUI_HANDLER);

        COMMAND_HANDLER->attachPresetHandler(PRESET_HANDLER);
        COMMAND_HANDLER->attachEffectHandler(EFFECT_HANDLER);
        COMMAND_HANDLER->attachCommandHandler(COMMAND_HANDLER);
        COMMAND_HANDLER->attachEventHandler (EVENT_HANDLER);
        COMMAND_HANDLER->attachLogicHandler (LOGIC_HANDLER);
        COMMAND_HANDLER->attachAnalogHandler(ANALOG_HANDLER);
        COMMAND_HANDLER->attachGuiHandler   (GUI_HANDLER);

        EVENT_HANDLER->attachPresetHandler  (PRESET_HANDLER);
        EVENT_HANDLER->attachEffectHandler  (EFFECT_HANDLER);
        EVENT_HANDLER->attachCommandHandler (COMMAND_HANDLER);
        EVENT_HANDLER->attachEventHandler   (EVENT_HANDLER);
        EVENT_HANDLER->attachLogicHandler   (LOGIC_HANDLER);
        EVENT_HANDLER->attachAnalogHandler  (ANALOG_HANDLER);
        EVENT_HANDLER->attachGuiHandler     (GUI_HANDLER);

        LOGIC_HANDLER->attachPresetHandler  (PRESET_HANDLER);
        LOGIC_HANDLER->attachEffectHandler  (EFFECT_HANDLER);
        LOGIC_HANDLER->attachCommandHandler (COMMAND_HANDLER);
        LOGIC_HANDLER->attachEventHandler   (EVENT_HANDLER);
        LOGIC_HANDLER->attachLogicHandler   (LOGIC_HANDLER);
        LOGIC_HANDLER->attachAnalogHandler  (ANALOG_HANDLER);
        LOGIC_HANDLER->attachGuiHandler     (GUI_HANDLER);

        ANALOG_HANDLER->attachPresetHandler (PRESET_HANDLER);
        ANALOG_HANDLER->attachEffectHandler (EFFECT_HANDLER);
        ANALOG_HANDLER->attachCommandHandler(COMMAND_HANDLER);
        ANALOG_HANDLER->attachEventHandler  (EVENT_HANDLER);
        ANALOG_HANDLER->attachLogicHandler  (LOGIC_HANDLER);
        ANALOG_HANDLER->attachAnalogHandler (ANALOG_HANDLER);
        ANALOG_HANDLER->attachGuiHandler    (GUI_HANDLER);
        
        GUI_HANDLER->attachPresetHandler    (PRESET_HANDLER);
        GUI_HANDLER->attachEffectHandler    (EFFECT_HANDLER);
        GUI_HANDLER->attachCommandHandler   (COMMAND_HANDLER);
        GUI_HANDLER->attachEventHandler     (EVENT_HANDLER);
        GUI_HANDLER->attachLogicHandler     (LOGIC_HANDLER);
        GUI_HANDLER->attachAnalogHandler    (ANALOG_HANDLER);
        GUI_HANDLER->attachGuiHandler       (GUI_HANDLER);

        // INIT AND SYNC HANDLERS //
        SFXPEvent INIT_ALL = SFXPEvent(SFXPEvent::Type::Event_InitAll);
        PRESET_HANDLER->pushEvent   (&INIT_ALL);
        EFFECT_HANDLER->pushEvent   (&INIT_ALL);
        COMMAND_HANDLER->pushEvent  (&INIT_ALL);
        EVENT_HANDLER->pushEvent    (&INIT_ALL);
        LOGIC_HANDLER->pushEvent    (&INIT_ALL);
        ANALOG_HANDLER->pushEvent   (&INIT_ALL);
        GUI_HANDLER->pushEvent      (&INIT_ALL);

        // PRET POUR FAIRE DU SALE MAMENE //
        cout << endl <<
        "******************************************************************"
        << endl <<
        "   Progam Setup Is OK : Let's get Started Mudda Fukkazz"
        << endl <<
        "******************************************************************"
        << endl << endl;
    }
    catch (const string & e) {

        SFXPlog::fatal("SFX-Pi") << e << endl;
        SFXPlog::fatal("SFX-Pi") << "Abort Program Launch" << endl;

        SFXP::GlobalNeedToExit = true;
        SFXP::GlobalErrState = 1;
    }

    //// MAIN LOOP ////
    while (!SFXP::GlobalNeedToExit){

        // UPDATE HANDLERS //
        PRESET_HANDLER  ->run();
        EFFECT_HANDLER  ->run();
        COMMAND_HANDLER ->run();
        EVENT_HANDLER   ->run();
        LOGIC_HANDLER   ->run();
        ANALOG_HANDLER  ->run();
        GUI_HANDLER     ->run();
        // Sleep for 10ms ( 100Hz )
        usleep(10000);
    }

    cout << endl <<
    "******************************************************************"
    << endl <<
    "   Progam End : Deconstruct Everything"
    << endl <<
    "******************************************************************"
    << endl << endl;

    // TELL THE PROGRAM TO STOP //
    SFXP::GlobalNeedToExit = true;

    // DESTROY HANDLERS //
    delete GUI_HANDLER;
    delete ANALOG_HANDLER;
    delete LOGIC_HANDLER;
    delete EVENT_HANDLER;
    delete COMMAND_HANDLER;
    delete EFFECT_HANDLER;
    delete PRESET_HANDLER;
    
    cout << endl <<
    "******************************************************************"
    << endl <<
    "   Progam End : GoodBye"
    << endl <<
    "******************************************************************"
    << endl << endl;
    
    return SFXP::GlobalErrState;
}
