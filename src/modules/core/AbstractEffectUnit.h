/**********************************************************************
 * @file AbstractEffectUnit.h
 * @author TomatoPi
 * @version 1.0
 *
 * Core File for all effects
 **********************************************************************/
#ifndef DEF_ABSTRACT_EFFECT_UNIT_H
#define DEF_ABSTRACT_EFFECT_UNIT_H

#include <iostream>
#include <string>
#include <map>
#include <cstring>

#include "JACKUnit.h"

#include "../../Const.h"

/**
 * Core class of Effects
 **/
class AbstractEffectUnit{

        typedef std::map<uint8_t, float*> ModBankArray;

    public :

        /**
         * EffectUnit Constructor.
         * Subclasses Constructor must build the JACKUnit
         *
         * @throw string if a construction step has failed
         * @see JACKUnit.h
         **/
        AbstractEffectUnit(uint8_t id, uint8_t type, uint8_t bankSize, uint16_t paramSize);
        virtual ~AbstractEffectUnit();

        /**
         * Change Effects Parameters.
         * Changed effect will affect current Bank, or create a new one
         * if effect bank array is empty
         * The two functions provided change a single param or all
         * params in a single row
         * Do Nothing if given index is too big or length differs from
         * bank size
         **/
        void setParam(uint8_t idx, float value);
        void setParam(uint8_t size, const float *values);
        float getParam(uint8_t idx)  const;

        /**
         * Add or Delete Given Bank.
         * For add a new Bank, given ID must not be already present
         * Given float array must be the same size as bank Size
         *
         * @warning Some ID values are reserved, @see EUCST
         * 
         * TODO Decider du comportement si ID deja present dans bankArray
         **/
        int addBank(uint8_t id);
        int addBank(uint8_t id, uint8_t size, const float *values);
        void removeBank(uint8_t id);
        uint8_t getCurrentBank() const;
        ModBankArray getAllBanks() const;
        uint8_t getBankSize() const;

        /**
         * Change Current Bank.
         * Prev and NextBank functions returns true if incrementation
         * has looped to the array beginning or end
         * Change Bank function take target bank id as parameter
         **/
        bool nextBank();
        bool prevBank();
        int changeBank(uint8_t id);

        /**
         * Change Effect's Bypassed status
         * A bypassed effect will ouput 0 if it's a generator
         * or Copy it's inputs to outputs if it's a processor
         **/
        void setBypass(bool state);
        bool getBypass() const;

        /**
         * Get Effect's Type Code
         **/
        uint8_t getType() const;

        /**
         * Get Effect's ID
         **/
        uint8_t getID() const;
        
        /**
         * Return JACK part of the EffectUnit
         * given number is index of an unit's port
         * return JACKUnit that own given port
         **/
        virtual JACKUnit* getJACKUnit(uint8_t pidx);

        /**
         * Print Current Effect Unit Content
         **/
        void printEffect() const;
        void printPool() const;

        /**
         * Clear content of effect unit
         **/
        void clearEffect();

    /* *** Effect Unit Stuff *** */
    protected :

        /**
         * Function Called When a parameter is modified or bank changed
         * It will update the Param Array passed to callback function
         * Pure Virtual function, must be redefined by every effect type
         **/
        virtual void update() = 0;

        uint8_t m_id;
        uint8_t m_type;

    /* *** JACK Stuff *** */
    protected :

        JACKUnit *m_jackU;

    /* *** Bank Related Stuff *** */
    protected :
        
        ModBankArray            m_bankArray;
        ModBankArray::iterator  m_currentBank;
        uint8_t                 m_bankSize;

        float*  m_paramArray;
        uint16_t m_paramSize;
};

/**
 * Effect Unit Constants
 **/
namespace EUCST{

    /* ****** COMMON PARAMS FOR EFFECTS ****** */
    const uint8_t C_PARAM_COUNT = 2;
    
    const uint8_t IDXBYP  = 0;
    const uint8_t IDXVOL  = 1;

    const std::string C_NAME[C_PARAM_COUNT] = {"Bypass", "Volume"};

    const std::string JACK_SERVER = "Space_Fx";
    
    /** Error Code if Unit Contains no Banks **/
    const uint8_t BANK_ID_ERR = 0xff;
};

#endif
