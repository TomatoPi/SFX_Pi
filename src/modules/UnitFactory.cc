/**********************************************************************
 * @file UnitFactory.cpp
 * @author TomatoPi
 * @version 1.0
 *
 * File Providing Common stuff about effect units
 **********************************************************************/
#include "UnitFactory.h"

std::map<uint8_t, UnitFactory::UnitFactoryReg> UnitFactory::m_reg = std::map<uint8_t, UnitFactory::UnitFactoryReg>();

/* ****************************************************************** */
/* ******************* Registration and Creation ******************** */
/* ****************************************************************** */
void UnitFactory::registerEffect(uint8_t type,
                    std::string name, builder_f builder,
                    const std::string* paramNames, uint8_t paramCount,
                    const std::string* portNames, 
                    uint8_t ai, uint8_t ao, uint8_t mi, uint8_t mo){

    if ( m_reg.find(type) == m_reg.end() ){

        m_reg[type] = UnitFactory::UnitFactoryReg(name, builder,
                                        paramNames, paramCount,
                                        portNames, ai, ao, mi, mo );

        printf("UnitFactory : Registered : TC( %3u ) : Name( %-18s ) : ParamCount( %2lu ) : PortCount( %u %u %u %u )\n",
            type,
            name.c_str(),
            m_reg[type].m_paramNames.size(),
            ai, ao, mi, mo
            );
    }
    else{

        printf("UnitFactory : Error : TC( %3u ) is already used\n", type);
    }
}

AbstractEffectUnit* UnitFactory::createEffect(uint8_t type, uint8_t id){

    if ( m_reg.find(type) != m_reg.end() ){

        return (*m_reg[type].m_builder)(id, type);
    }
    return NULL;
}

/* ****************************************************************** */
/* ************************ Getters and Setters ********************* */
/* ****************************************************************** */
std::string UnitFactory::getName(uint8_t type){

    if ( m_reg.find(type) != m_reg.end() ){

        return m_reg[type].m_name;
    }
    return std::string();
}

UnitFactory::NameArray UnitFactory::getParamNames(uint8_t type){

    if ( m_reg.find(type) != m_reg.end() ){

        return m_reg[type].m_paramNames;
    }
    return UnitFactory::NameArray();
}
uint8_t UnitFactory::getParamCount(uint8_t type){

    if ( m_reg.find(type) != m_reg.end() ){

        return m_reg[type].m_paramNames.size();
    }
    return 0;
}

UnitFactory::NameArray UnitFactory::getPortNames(uint8_t type){

    if ( m_reg.find(type) != m_reg.end() ){

        return m_reg[type].m_portNames;
    }
    return UnitFactory::NameArray();
}
uint8_t UnitFactory::getPortCount(uint8_t type, uint8_t ptype){

    if ( m_reg.find(type) != m_reg.end() && ptype < 4 ){

        return m_reg[type].m_portCount[ptype];
    }
    return 0;
}

std::string UnitFactory::buildPortName(AbstractEffectUnit* source,
                                    uint8_t ptype, uint8_t idx){

    uint8_t type = source->getType();
    
    if ( m_reg.find(type) != m_reg.end() ){

        uint8_t pidx = 0;
        for( uint8_t i = 0; i < ptype; i++){

             pidx += m_reg[type].m_portCount[i];
        }
        
        std::string ename = source->getJACKUnit(pidx+idx)->getName();
        
        return ename + ":" + m_reg[type].m_portNames[pidx+idx];
    }
    return std::string();
}

/* ****************************************************************** */
/* ******************* UnitFactoryRegister Entry ******************** */
/* ****************************************************************** */
UnitFactory::UnitFactoryReg::UnitFactoryReg():
    m_name(""),
    m_builder(NULL),
    m_paramNames(NameArray()),
    m_portNames(NameArray())
{
    m_portCount[0] = 0;
    m_portCount[1] = 0;
    m_portCount[2] = 0;
    m_portCount[3] = 0;
}
UnitFactory::UnitFactoryReg::UnitFactoryReg(std::string name, builder_f builder,
                    const std::string* paramNames, uint8_t paramCount,
                    const std::string* portNames, 
                    uint8_t ai, uint8_t ao, uint8_t mi, uint8_t mo):
    m_name(name),
    m_builder(builder),
    m_paramNames(NameArray()),
    m_portNames(NameArray())
{
    m_portCount[0] = ai;
    m_portCount[1] = ao;
    m_portCount[2] = mi;
    m_portCount[3] = mo;
    
    // Copy Common param names and effect's param names
    const uint8_t cpc = EUCST::C_PARAM_COUNT;
    
    for ( uint8_t i = 0; i < cpc; i++ ){

        m_paramNames.push_back(EUCST::C_NAME[i]);
    }
    for ( uint8_t i = cpc; i < paramCount + cpc ; i++ ){

        m_paramNames.push_back(paramNames[i-cpc]);
    }

    // Copy Ports Names
    for ( uint8_t i = 0; i < ai + ao + mi + mo; i++ ){

        m_portNames.push_back(portNames[i]);
    }
}

void UnitFactory::printRegister(){

    printf("Available Effects : \n");
    for ( auto& cur : m_reg ){

        printf("   - %3u : %-13s\n", cur.first, cur.second.m_name.c_str());
    }
    printf("End\n");
}