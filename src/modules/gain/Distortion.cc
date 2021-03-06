/*
 * Copyright (C) 2018 Space-Computer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <math.h>

#include "noyau/midi.h"
#include "noyau/utils.h"
#include "noyau/modules/ModuleBase.h"

//#include "lib/filter/GraphicEQ.h"
//#include "lib/filter/GraphicEQ_macros.h"

#define UNIQUE_NAME "simple_distortion"
#define NAME "Distortion"
#define VERSION "1.1"

struct DistortionDatas
{
    DistortionDatas():
    
        shape(0.37), softness(8.4),
    
        gain(748), volume(0.01f),
        seuil(0.8), shoot(2), knee(0.01), pout(0)
    {
//        
//        // Setup Equalizer, Param used here are not important
//        float f[] = {100, 1000};
//        tone_in  = std::unique_ptr<GraphicEQ>(new GraphicEQ( 2, f, samplerate));
//        tone_out = std::unique_ptr<GraphicEQ>(new GraphicEQ( 2, f, samplerate));
//
//        tone_in_params.low  = 220;
//        tone_in_params.high = 830;
//
//        tone_in_params.gains[0] = 1;//1.81f;
//        tone_in_params.gains[1] = 1;//2.26f;
//        tone_in_params.gains[2] = 1;//1.17f;
//
//        tone_out_params.low  = 246;
//        tone_out_params.high = 622;
//
//        tone_out_params.gains[0] = 1;//1.31f;
//        tone_out_params.gains[1] = 1;//2.02f;
//        tone_out_params.gains[2] = 1;//1.63f;
//        
    }
    
    float shape, softness;
    
    float gain, volume;
    
    float seuil, shoot, knee, pout;
};

extern "C"
Module::ShortInfo function_register_module_info(void)
{
    return Module::ShortInfo(UNIQUE_NAME, NAME, VERSION);
}

#ifdef __SFX_PI__
extern "C"
void* function_create_jack_module(EffectUnit* effect)
{
    effect->registerAudioInput("Input");
    effect->registerAudioOutput("Output");
            
    return new DistortionDatas();
}
#else
extern "C"
void* function_create_non_jack_module(EffectUnit* effect)
{
    effect->registerAudioInput("Input");
    effect->registerAudioOutput("Output");
            
    return new DistortionDatas();
}
#endif

extern "C"
void function_destroy_module(void* datas)
{
    delete (DistortionDatas*)datas;
}

extern "C"
Module::SlotTable function_register_module_slots(void)
{
    Module::SlotTable table;
    /*
     * Register all slots with :
     *
    table["NOM"] = Module::Slot{"NOM", [](sfx::hex_t val, Module* mod)
        {
            if (val < 128)
            {
                // Code du Slot ici
     
                // Valeur variant lineairement
                //((MODULE_DATAS*)mod->getDatas())->PARAM = sfx::mapfm_lin(val, MIN, MAX); 
     
                // Valeur de gain en db : renvois facteur gain
                //((MODULE_DATAS*)mod->getDatas())->PARAM = sfx::mapfm_db(val, MIN, MAX);  
     
                // Valeur representant une frequence [~20Hz;~20kHz]
                //((MODULE_DATAS*)mod->getDatas())->PARAM = sfx::mapfm_freq(val, MIN, MAX);
            }
            return ((MODULE_DATAS*)mod->getDatas())->PARAM; // Retourner la valeur du paramètre modulé
        }};
     */
    
    Module::register_slot(table, 46, "shape", "Shape", [](sfx::hex_t val, EffectUnit* effect)->float
        {
            if (val < 128)
                ((DistortionDatas*)effect->getDatas())->shape = sfx::mapfm_lin(val, 0, 1);
            return ((DistortionDatas*)effect->getDatas())->shape;
        });
    Module::register_slot(table, 15, "softness", "Softness", [](sfx::hex_t val, EffectUnit* effect)->float
        {
            if (val < 128)
                ((DistortionDatas*)effect->getDatas())->softness = sfx::mapfm_lin(val, 1, 60);
            return ((DistortionDatas*)effect->getDatas())->softness;
        });
        
        
    Module::register_slot(table, 46, "seuil", "Seuil", [](sfx::hex_t val, EffectUnit* effect)->float
        {
            if (val < 128)
                ((DistortionDatas*)effect->getDatas())->seuil = sfx::mapfm_lin(val, 0, 1);
            return ((DistortionDatas*)effect->getDatas())->seuil;
        });
    Module::register_slot(table, 15, "shoot", "Shoot", [](sfx::hex_t val, EffectUnit* effect)->float
        {
            if (val < 128)
                ((DistortionDatas*)effect->getDatas())->shoot = sfx::mapfm_lin(val, 0.01, 60);
            return ((DistortionDatas*)effect->getDatas())->shoot;
        });
    Module::register_slot(table, 46, "knee", "Knee", [](sfx::hex_t val, EffectUnit* effect)->float
        {
            if (val < 128)
                ((DistortionDatas*)effect->getDatas())->knee = sfx::mapfm_lin(val, 0.000001, 0.1);
            return ((DistortionDatas*)effect->getDatas())->knee;
        });
        
    Module::register_slot(table, 81, "gain", "Gain", [](sfx::hex_t val, EffectUnit* effect)->float
        {
            if (val < 128)
                ((DistortionDatas*)effect->getDatas())->gain = sfx::mapfm_db(val, -10, +50);
            return ((DistortionDatas*)effect->getDatas())->gain;
        });
    Module::register_slot(table, 42, "volume", "Volume", [](sfx::hex_t val, EffectUnit* effect)->float
        {
            if (val < 128)
                ((DistortionDatas*)effect->getDatas())->volume = sfx::mapfm_db(val, -50, +10);
            return ((DistortionDatas*)effect->getDatas())->volume;
        });
        
        /////////////////////////////////////////////////////////////
        // Input Filter
        /////////////////////////////////////////////////////////////
        
//     Module::register_slot(table, 57, "in_lowcut", "In Lowcut", [](sfx::hex_t val, EffectUnit* effect)
//        {
//            if (val < 128)
//            {
//                float res = sfx::mapfm_freq(val);
//                ((DistortionDatas*)effect->getDatas())->tone_in_params.low = res;
//                ((DistortionDatas*)effect->getDatas())->tone_in->setFrequency(0, res, jack_get_sample_rate(effect->getClient()));
//            }
//            return ((DistortionDatas*)effect->getDatas())->tone_in_params.low;
//        });
//     Module::register_slot(table, 79, "in_highcut", "In Highcut", [](sfx::hex_t val, EffectUnit* effect)
//        {
//            if (val < 128)
//            {
//                float res = sfx::mapfm_freq(val);
//                ((DistortionDatas*)effect->getDatas())->tone_in_params.high = res;
//                ((DistortionDatas*)effect->getDatas())->tone_in->setFrequency(1, res, jack_get_sample_rate(effect->getClient()));
//            }
//            return ((DistortionDatas*)effect->getDatas())->tone_in_params.high;
//        });
//    Module::register_slot(table, 75, "in_lowgain", "In Lowgain", [](sfx::hex_t val, EffectUnit* effect)
//        {
//            if (val < 128)
//                ((DistortionDatas*)effect->getDatas())->tone_in_params.gains[0] = sfx::mapfm_db(val, -30, 30);
//            return ((DistortionDatas*)effect->getDatas())->tone_in_params.gains[0];
//        });
//    Module::register_slot(table, 80, "in_midgain", "In Midgain", [](sfx::hex_t val, EffectUnit* effect)
//        {
//            if (val < 128)
//                ((DistortionDatas*)effect->getDatas())->tone_in_params.gains[1] = sfx::mapfm_db(val, -30, 30);
//            return ((DistortionDatas*)effect->getDatas())->tone_in_params.gains[1];
//        });
//    Module::register_slot(table, 87, "in_highgain", "In Highgain", [](sfx::hex_t val, EffectUnit* effect)
//        {
//            if (val < 128)
//                ((DistortionDatas*)effect->getDatas())->tone_in_params.gains[2] = sfx::mapfm_db(val, -30, 30);
//            return ((DistortionDatas*)effect->getDatas())->tone_in_params.gains[2];
//        });
        
        /////////////////////////////////////////////////////////////
        // Output Filter
        /////////////////////////////////////////////////////////////
             
//    Module::register_slot(table, 59, "out_lowcut", "Out Lowcut", [](sfx::hex_t val, EffectUnit* effect)->float
//        {
//            if (val < 128)
//            {
//                float res = sfx::mapfm_freq(val);
//                ((DistortionDatas*)effect->getDatas())->tone_out_params.low = res;
//                ((DistortionDatas*)effect->getDatas())->tone_out->setFrequency(0, res, jack_get_sample_rate(effect->getClient()));
//            }
//            return ((DistortionDatas*)effect->getDatas())->tone_out_params.low;
//        });
//    Module::register_slot(table, 75, "out_highcut", "Out Highcut", [](sfx::hex_t val, EffectUnit* effect)->float
//        {
//            if (val < 128)
//            {
//                float res = sfx::mapfm_freq(val);
//                ((DistortionDatas*)effect->getDatas())->tone_out_params.high = res;
//                ((DistortionDatas*)effect->getDatas())->tone_out->setFrequency(1, res, jack_get_sample_rate(effect->getClient()));
//            }
//            return ((DistortionDatas*)effect->getDatas())->tone_out_params.high;
//        });
//    Module::register_slot(table, 69, "out_lowgain", "Out Lowgain", [](sfx::hex_t val, EffectUnit* effect)->float
//        {
//            if (val < 128)
//                ((DistortionDatas*)effect->getDatas())->tone_out_params.gains[0] = sfx::mapfm_db(val, -30, 30);
//            return ((DistortionDatas*)effect->getDatas())->tone_out_params.gains[0];
//        });
//    Module::register_slot(table, 78, "out_midgain", "Out Midgain", [](sfx::hex_t val, EffectUnit* effect)->float
//        {
//            if (val < 128)
//                ((DistortionDatas*)effect->getDatas())->tone_out_params.gains[1] = sfx::mapfm_db(val, -30, 30);
//            return ((DistortionDatas*)effect->getDatas())->tone_out_params.gains[1];
//        });
//    Module::register_slot(table, 73, "out_highgain", "Out Highgain", [](sfx::hex_t val, EffectUnit* effect)->float
//        {
//            if (val < 128)
//                ((DistortionDatas*)effect->getDatas())->tone_out_params.gains[2] = sfx::mapfm_db(val, -30, 30);
//            return ((DistortionDatas*)effect->getDatas())->tone_out_params.gains[2];
//        });
    
    return table;
}

/*
 * fonction donnant le facteur de reduction de la dérivée en fonction de la distance au max
 * il faut une fonction tendant vers 0+ plus on se raproche de 1-
 * et 0- plus on se raproche de 1+
 * et qui vaut 1 quand on est au seuil
 */
float gradiant_v2(float valeur, float seuil, float shoot, float knee)
{
    if (valeur > seuil)
    {
        return knee*(exp((shoot*(valeur-seuil))/(seuil-0.95))-exp(-shoot));
    }
    else return 1;
}

float gradiant_v1(float valeur, float seuil)
{
    if (valeur > seuil)
    {
        return (valeur-0.9)/(seuil - 0.9);
    }
    else return 1;
}

/*
 * ici on va avoir besoin de la valeur courante de l'entrée, et la valeur précédante : in, prev
 * la sortie sera toujours notée out
 * La fonction du gradiant ne garantissant plus de rattraper correctement la courbe
 * on rajoute une condition disant que si le signal repasse sous le signal saturé, on arrete
 * de modifier le signal
 */
 float clip_gradiant_v2(float in, float seuil, float gain
 , float shoot, float knee
 , float& prev_out)
 {
    float out = prev_out;
    in *= gain;
    
    if (in > 0)
    {
        if (in < seuil) out = in;
        else
        {
            float alpha = gradiant_v2(prev_out, seuil, shoot, knee);
            out += alpha;
            
            // On verifie si il faut rattraper la courbe normale
            if (out > in) out = in;
            if (out > 1) out = 1;
        }
    }
    else 
    {
        if (in > -seuil) out = in;
        else
        {
            float alpha = gradiant_v2(-prev_out, seuil, shoot, knee);
            out -= alpha;
            
            // On verifie si il faut rattraper la courbe normale
            if (out < in) out = in;
            if (out < -1) out = -1;
        }
    }
    
    prev_out = out;
    
    return out;
 }
 float clip_gradiant_v1(float valeur, float seuil, float gain, float& prev_in, float& prev_out)
 {
    float out = prev_out;
    valeur *= gain;
    float dt = valeur - prev_in; // on calcule la variation
    
    if (valeur > 0)
    {
        if (valeur < seuil) out = valeur;
        else
        {
            float alpha = gradiant_v1(prev_out, seuil);
            out += alpha*dt;
            
            // On verifie si il faut rattraper la courbe normale
            if (out > valeur) out = valeur;
        }
    }
    else 
    {
        if (valeur > -seuil) out = valeur;
        else
        {
            float alpha = gradiant_v1(-prev_out, seuil);
            out += alpha*dt;
            
            // On verifie si il faut rattraper la courbe normale
            if (out < valeur) out = valeur;
        }
    }
    
    prev_in = valeur;
    prev_out = out;
    
    return out;
 }

#ifdef __SFX_PI__
extern "C"
int function_process_callback(jack_nframes_t nframes, void* arg)
{
    EffectUnit* effect = (EffectUnit*) arg;
    DistortionDatas* disto = (DistortionDatas*) effect->getDatas();
    
    sfx::sample_t *in, *out;
    in  = (sfx::sample_t*)jack_port_get_buffer( effect->getAudioInputs(0).port , nframes );
    out = (sfx::sample_t*)jack_port_get_buffer( effect->getAudioOutputs(0).port, nframes );

    void* midi_port = jack_port_get_buffer(effect->getMidiInputs(0).port, nframes);
    jack_midi_event_t in_event;
    jack_nframes_t event_index = 0;
    jack_nframes_t event_count = jack_midi_get_event_count(midi_port);
    
    /*
    for (jack_nframes_t i = 0; i < event_count; ++i)
    {
        jack_midi_event_get(&in_event, midi_port, i);
        sfx::debug(NAME, "Event %d time is %d. 1st byte is 0x%x\n"
            , i, in_event.time, *(in_event.buffer));
    }
    //*/
    
    void* midi_throught = jack_port_get_buffer(effect->getMidiOutputs(0).port, nframes);
    jack_midi_clear_buffer(midi_throught);
    
    jack_midi_event_get(&in_event, midi_port, 0);
    for ( jack_nframes_t i = 0; i < nframes; i++ )
    {
        if (in_event.time == i && event_index < event_count)
        {
            sfx::Midi_reserve_MidiThroughMessage(midi_throught, in_event);
            
            if (sfx::Midi_verify_ChanneledMessage(in_event.buffer, sfx::Midi_ControlChange))
            {
                effect->veryfyAndComputeCCMessage(in_event.buffer);
            }
            event_index++;
            if (event_index < event_count)
            {
                jack_midi_event_get(&in_event, midi_port, event_index);
            }
        }

        //out[i] = disto->tone_in->compute(in[i], 3, disto->tone_in_params.gains);
        
        out[i] = in[i] * disto->gain;
        out[i] = (1 - disto->shape)*tanh( out[i] ) + disto->shape*tanh( out[i] / disto->softness );
        //out[i] = tanh(clip_gradiant_v1(out[i], disto->seuil, disto->gain, disto->shoot, disto->knee));//, disto->pout);
        
        out[i] *= disto->volume;//* disto->tone_out->compute(out[i], 3, disto->tone_out_params.gains);
    }
    
    return 0;
}
#endif