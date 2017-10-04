/**
 * File Containing Add Module submenu's functions and stuff
 */

#ifndef DEF_SUB_ADD_MODULE_H
#define DEF_SUB_ADD_MODULE_H

#include "../../core/Tree/Tree.h"
#include "../../core/Modules.h"
#include "Declarations.h"

/**
 * Function called once at Menu initialisation for add this Submenu
 * It add new menu branch next after given Iterator
 * and return iterator on Added highest node
 */
MenuIterator menu_init_add_module( MenuTree *menu, MenuIterator pos );

/**********************************************************************
 *  Nodes
 **********************************************************************/

/**
 * Add Module to the graph
 */
void main_add_module_enter( Module_Node_List* );
MenuIterator main_add_module_do( Move_flag, MenuIterator, Module_Node_List* );

/**
 * Choose module type
 */
void addmod_choose_type_enter( Module_Node_List* );
MenuIterator addmod_choose_type_do( Move_flag, MenuIterator, Module_Node_List* );

#endif
