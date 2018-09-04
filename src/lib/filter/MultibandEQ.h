/**********************************************************************
 * @file MultibandEQ.h
 * @author TomatoPi
 * @version 1.0
 *
 * File Providing Signal Filtering stuff.
 * MultiPole Filter provided by this file is analog to a MultiBand
 * Equalizer
 * 
 * https://en.wikipedia.org/wiki/Equalization_(audio)
 **********************************************************************/
#ifndef DEF_MULTIPOLE_FILTER_H
#define DEF_MULTIPOLE_FILTER_H

#include <algorithm>
#include <iostream>

#include "lib/filter/FilterBase.h"

/**
 * Basic Equalizer
 * @warning an EQ with N bands will Have N-1 Poles
 **/
class MultibandEQ : public AFilterBase{

    public :

        MultibandEQ(sfx::usize_t poleCount, float* poles, float samplerate
                    ,sfx::usize_t order=4);
        virtual ~MultibandEQ();

        /**
         * Compute given input and output mix of all bands
         * Use getBand function for get a specific band value
         **/
        float compute(float in, sfx::usize_t bandCount, float* gains);
        float getBand(sfx::usize_t idx);

        /**
         * Change EQ Configuration
         **/
        void setFrequency(sfx::usize_t idx, float f, float sr);
        void setFrequency(sfx::usize_t poleCount, float* poles, float sr);
        float getFrequency(sfx::usize_t idx) const;

        sfx::usize_t getBandCount() const;

    protected :
    
        /** store non-scalled band outputs **/
        float*          m_band;
        sfx::usize_t   m_bandCount;

        /** Store EQ Poles **/
        FilterPole**    m_pole;
        sfx::usize_t   m_poleCount;
};

#endif
