#ifndef DEF_MODULES_H
#define DEF_MODULES_H

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <vector>

#include <math.h>
#include <jack/jack.h>

#include "../consts.h"

/**
*   Enum of avaiables ports types
*/
enum MODULE_PORT_TYPE{

    AUDIO_I,
    AUDIO_O,
    MIDI_I,
    MIDI_O,
    MOD
};

/**
*   Function for register class member callback functions
*   @see Module::process_callback(jack_nframes_t, void*)
*/
int mod_Process_Callback(jack_nframes_t nframes, void *u);

/**
*   Alias for vector of param array used by modules.
*   Banks are used to save different sets of param's values for a module
*/
typedef std::vector<float*> ModBank;

/*
*   Params shared by all modules
*/
#define MOD_COUNT 2

#define MOD_BYPASS 0
#define MOD_VOLUME 1

static const std::string MOD_PARAMS[MOD_COUNT] = {"Bypass", "Volume"};

/*
*   Basic class for all modules
*/
class Module{

    public:

        /**
        *   Module basic constructor.
        *   It create jack client for module, alloc and register it's ports
        *   It also alloc params array and make module waiting for activation
        *   Module specific subconstructor have to set it ready for compute
        *
        *   @param server JACK's server name
        *   @param type module's type
        *   @param pc number of parameters
        *   @param ai number of audio inputs
        *   @param ao number of audio outputs
        *   @param mi number of midi inputs
        *   @param mo unmber of midi outputs
        *   @param ... names of ports in order
        */
        Module( std::string name, int type, int id, int pc, int ai, int ao, int mi, int mo, ... );
        virtual ~Module();

        /**
        *   Process callback called by jack when module is active
        *
        *   @param nframes number of frames to compute
        *   @param arg pointer to this module, unused
        *   @return 0
        */
        int process(jack_nframes_t nframes, void *arg);

        /**
        *   Set module status
        *   @param state true for bypass module
        */
        void    set_bypass(bool state);
        /**
        *   Get module status
        *   @return true if module is bypassed
        */
        bool    get_bypass() const;

        /**
        *   Get module JACK's client
        *   @return module's client
        */
        jack_client_t*  get_client() const;

        /**
        *   Get module's type
        *   @return module's type
        */
        int get_type();

        /**
        *   Get module's name
        *   @return module's name
        */
        std::string get_name();

        /**
        *   Change a single module's param.
        *   Check if given idx corrspond to a param, do nothing if not
        *   @param idx idx of param to change_param
        *   @param value new value for param
        */
        void set_param(int idx, float value);
        /**
        *   Change all params in a single row.
        *   Check if given param count is equal to module's param count
        *   do nothing if not
        *   @param count number of params in given array
        *   @param values params array
        */
        void set_param(int count, const float *values);

        /**
        *   Get number of parameters
        *   @return number of params
        */
        //int get_param_count() const;
        /**
        *   Get given param's value
        *   @param idx param's index
        *   @return given param value or 0 if not found
        */
        float get_param(int idx) const;

        /**
        *   Get formated name of given param.
        *   Used in user interface for navigate between params
        *   return empty string if idx is not valid
        *   @param idx param's index
        *   @return param's name or empty string if index is not valid
        */
        //std::string get_param_name(int idx);

        /**
        *   Get parame name followed by it value.
        *   Returned string is maximum 11 caracters long for screen
        *   @param idx param's index
        *   @return formated string or NONE if given param not found
        */
        //std::string get_formated_param(int idx);

        /**
        *   Get module's port.
        *   @param type port's type
        *   @param idx port's index
        *   @return port of given type at given index or NULL if not found
        *   @see MODULE_PORT_TYPE
        */
        jack_port_t* get_port(MODULE_PORT_TYPE type, int idx);

        /**
        *   Get number of given port type
        *   @param type port's type
        *   @return number of prots of given type
        *   @see MODULE_PORT_TYPE
        */
        int get_port_count(MODULE_PORT_TYPE type);

        /**
        *   Add a bank to the module.
        *   @see ModBank
        *   @param size number of params in given bank
        *   @param bank bank to add
        */
        void add_bank(int size, const float *bank);

        /**
        *   Add a bank with default params values to the module.
        */
        void add_bank();

        /**
        *   Remove current bank from module.
        */
        void remove_bank();
        /**
        *   Remove given bank from module.
        *   @param idx index of bank to remove
        */
        void remove_bank(int idx);
        /**
        *   Change current bank and swap param values.
        *   Increment current bank or return to first one if current is the last
        *   @return true if bank have rewind to first bank
        */
        bool next_bank();
        /**
        *   Change current bank and swap param values.
        *   Decrement current bank or go to the last one if current is the first
        *   @return true if bank have rewind to last bank
        */
        bool prev_bank();
        /**
        *   Go to the given bank.
        *   If given index doesn't correspond to a bank, go to bank 0
        *   or default values if there is no bank
        *   @param idx target bank index
        *   @return true if given bank loaded
        */
        bool set_bank(int idx);
        /**
        *   Get curent bank index
        *   @return current bank index
        */
        int get_bank();

        /**
        *   Add modulation port
        */
        void add_mod();
        /**
        *   Remove given modulation port
        *   @param idx port's index to remove
        */
        void del_mod(int idx);

        int get_id() const;

    protected:

        virtual inline int do_process(jack_nframes_t nframes) { return 0; };

        virtual void change_param(int idx, float value) {}; /**< @see set_param(int idx, float value) */
        virtual void change_param(const float *values)  {}; /**< @see set_param(float *values) */

        //virtual std::string return_param_name(int idx)     { return std::string(""); };   /**< @see get_param_name(int idx) */
        //virtual std::string return_formated_param(int idx) { return std::string(""); };   /**< @see get_formated_param(int idx) */

        virtual void new_bank() {};    /**< @see add_bank() */

        jack_client_t   *client_;   /**< JACK's Client */
        char*           name_;      /**< Client unique name */
        int             type_;      /**< Module's type */

        bool            is_bypassed_;   /**< True if Module is bypassed */

        jack_port_t **audio_in_, **audio_out_;  /**< JACK's audio ports */
        int ai_, ao_;                           /**< number of audio ports*/
        jack_port_t **midi_in_, **midi_out_;    /**< JACK's midi ports */
        int mi_, mo_;                           /**< number of midi ports*/

        float *param_;  /**< Pointer to current's bank begin */
        int param_c_;   /**< Number of params */

        ModBank bank_;  /**< Vector of avaiable params sets */
        int bank_idx_;  /**< Curent bank index */

        std::vector<jack_port_t*> mod_port_; /**< vector of modulation ports */

        int id_;
};

typedef enum{

    CURVE_LIN,
    CURVE_SIG,
    CURVE_COS
}IO_CURVE;

float curve_lin(float v);   //  Identity tranfer function
float curve_sig(float v);   //  Tanh transfert function scaled between 0 and 1
float curve_cos(float v);   //  Hanning window function

/**
*   Accessor are structure that store values for modify parameters dynamically
*
*   It contain a source index : mod port witch accessor is connected to
*   The targeted param : param that will be modified
*   The modification plage
*   A trasfert curve between modulation signal and modulated param
*/
class Accessor{

    public :

        /**
        *   Accessor constructor
        *
        *   @param target target module's index
        *   @param target target param's index
        *   @param min minimal param's value or offset
        *   @param max maximum param's value or offset
        *   @param curve transfert curve between modulation signal and param
        *   @param isdb true if min and max are in db
        *   @param isrelative true if modulation is relative to param value
        */
        Accessor(int target=-1, int targetp=-1, float min=-1, float max=-1, IO_CURVE curve=CURVE_LIN, bool isdb=false, bool isrelative=false);

        /**
        *   Function that map input.
        *
        *   @param input value to map ( -1 < input < 1 )
        *   @param ref center value
        *   @return modulated param
        */
        float compute( float input , float ref );

        /**
        *   Get current accessor's curve
        *   @return current accessor curve
        */
        IO_CURVE get_curve() const;
        /**
        *   Change current transfert curve
        *   @param new curve
        */
        void set_curve( IO_CURVE curve );

        int target_;
        int targetp_;

        float min_;
        float max_;

        bool isdb_;
        bool isrlt_;

    private :

        float (*curve_func_)(float value);
        IO_CURVE curve_;
};

#endif
