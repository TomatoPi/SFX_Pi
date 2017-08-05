#include "Presset.h"
/*
*	---------------------------------------------------------------------------
*	Presset stuff
*	---------------------------------------------------------------------------
*/
int save_preset(string name, string version, Module_Node_List list){
	
	string filename = "./Files/" + name + ".txt";
	ofstream flux(filename.c_str());
	
	if(flux){
		
		cout << "Save presset file : " << filename;
		int i = 0;
		flux << NODE_ROOT_PRESET << " " << version << " " << name << " " << list.list_.size() << endl;
		
		for(Module_List::iterator itr = list.list_.begin(); itr != list.list_.end(); itr++){
			
			Module_Node* node = *itr;
			int param_count = node->get_module()->get_param_count();
			
			flux << NODE_MODULE << " " << node->get_module()->get_type() << " " << param_count << endl;
			
            if(node->get_module()->set_bank(0)){
                
                do{
                    
                    flux << "    " << NODE_PARAM << " " << param_count ;
                    for(int n = 0; n < param_count; n++) flux << " " << node->get_module()->get_param(n);
                    flux << endl;
                    
                }while(!node->get_module()->next_bank());
            }
			
			for(IO_Accessor_List::iterator atr = node->accessor_get_list()->begin(); atr != node->accessor_get_list()->end(); atr++){
				
				IO_Accessor *a = *atr;
				flux << "    " << NODE_ACCESSOR << " " << i << " " << a->get_target_param() << " " << a->get_potar();
				flux << " " << a->get_min() << " " << a->get_max() << " " << a->get_curve() << " " << a->get_db() << " " << a->get_inv() << endl;
			}
			
			i++;
		}
		
		for(Module_List::iterator itr = list.list_.begin(); itr != list.list_.end(); itr++){
			
			Module_Node* node = *itr;
			for(Connection_List::iterator ctr = node->connection_get_list()->begin(); ctr != node->connection_get_list()->end(); ctr++){
				
				Connection c = (*ctr);
				
				flux << NODE_CONNECTION << " " << c.s << " " << c.sp << " " << c.t << " " << c.tp << endl;
			}
		}
	}else{
		
		flux.close();
		cout << "Error can't open file " << filename << endl;
		return 1;
	}
	flux.close();
	cout << " -- Presset successfully saved" << endl;
	return 0;
}

int load_preset(string name, string version, Module_Node_List *list){
	
	string filename = "./Files/" + name + ".txt";
	ifstream flux(filename.c_str());
    
    cout << "Load presset : " << filename << endl;
	
	if(flux){
		
		Module_Node_List newlist;
		
		string current;
		flux >> current;
		//cout << current << " ";
		
		if(!!current.compare(NODE_ROOT_PRESET)){
			cout << "File format not mach, First ndoe must be a NODE_ROOT_PRESET -- " << current << endl;
			flux.close();
			return 1;
		}
		current.clear();
		
		flux >> current;
		//cout << current << " ";
		if(!!current.compare(version)){
			cout << "Bad version -- File : " << current << " -- Current : " << version << endl;
			flux.close();
			return 1;
		}
		current.clear();
		
		flux >> current;
		//cout << current;
		current.clear();
		
		int module_count = 0;
		int current_module = -1;
		flux >> module_count;
		//cout << " " << module_count << endl;
		
		//cout << "Document start" << endl;
		current.clear();
		while(flux >> current){
			
			if(!current.compare(NODE_MODULE)){
				
				
				int type, param_count;
				flux >> type;
				flux >> param_count;
				
				//cout << current << " " << type << " " << param_count << endl;
				
				if(newlist.add_module(static_cast<MODULE_TYPE>(type))){
					cout << "Failed load Module -- Type : " << type << endl;
					flux.close();
					return 1;
				}
				++current_module;
				
			}else if(!current.compare(NODE_PARAM) && current_module != -1){
				
				int param_count;
				flux >> param_count;
				
				//cout << current << " " << param_count ;
				
				float *params = new float[param_count];
				for(int i = 0; i < param_count; i++){
					flux >> *(params+i);
					//cout << " " << params[i];
				}
				//cout << endl;
				
				newlist.list_[current_module]->get_module()->add_bank(param_count, params);
				
			}else if(!current.compare(NODE_ACCESSOR) && current_module != -1){
				
				int ti, pi, p, c, id, iv;
				float min, max;
				flux >> ti;
				flux >> pi;
				flux >> p;
				flux >> min;
				flux >> max;
				flux >> c;
				flux >> id;
				flux >> iv;
				
				//cout << current << " " << ti << " " << pi << " " << p << " " << c << " " << id << " " << iv << endl;
				
				newlist.add_accessor(ti, pi, p, min, max, static_cast<IO_CURVE>(c), id, iv);
				
			}else if(!current.compare(NODE_CONNECTION) && current_module != -1){
				
				short si, sp, ti, tp;
				flux >> si;
				flux >> sp;
				flux >> ti;
				flux >> tp;
				
				//cout << current << " " << si << " " << sp << " " << ti << " " << tp <<endl;
				
				newlist.add_connection(si, sp, ti, tp);
			}
			current.clear();
		}
		
		//cout << "document end" << endl;
		
		for(Module_List::iterator itr = list->list_.begin(); itr != list->list_.end(); itr++) delete *itr;
		list->list_.clear();
		
		*list = newlist;
		
	}else{
		
		cout << "Error can't open file " << filename << endl;
		flux.close();
		return 1;
	}
	flux.close();
	cout << "Presset loaded successfull" << endl;
	return 0;
}