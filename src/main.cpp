#include "parameter_manager.h"
#include "wireworld_configuration.h"
#include "wireworld.h"

#include <fstream>
#include <vector>
#include <stdint.h>
#include <iostream>

int main(int argc,char ** argv)
{

  // Defining application command line parameters
  parameter_manager::parameter_manager l_param_manager("wireworld.exe","--",1);
  parameter_manager::parameter_if l_param_file("input_file");
  parameter_manager::parameter_if l_param_nb_max_cycle("nb_max_cycle");
  parameter_manager::parameter_if l_param_start_cycle("start_cycle",true);
  parameter_manager::parameter_if l_param_delay("frame_delay",true);
  parameter_manager::parameter_if l_param_refresh_interval("refresh_interval",true);
  parameter_manager::parameter_if l_param_display_duration("display_duration",true);
  l_param_manager.add(l_param_file);
  l_param_manager.add(l_param_start_cycle);
  l_param_manager.add(l_param_nb_max_cycle);
  l_param_manager.add(l_param_delay);
  l_param_manager.add(l_param_refresh_interval);
  l_param_manager.add(l_param_display_duration);

  // Treating parameters
  l_param_manager.treat_parameters(argc,argv);

  // Defining configuration according to parameters
  wireworld_configuration l_conf;
  if(l_param_file.value_set())
    {
      l_conf.set_input_file_name(l_param_file.get_value<std::string>());
    }

  if(l_param_nb_max_cycle.value_set())
    {
      l_conf.set_nb_max_cycle(l_param_nb_max_cycle.get_value<uint32_t>());
    }

  if(l_param_start_cycle.value_set())
    {
      l_conf.set_start_cycle(l_param_start_cycle.get_value<uint32_t>());
    }

  if(l_param_refresh_interval.value_set())
    {
      l_conf.set_refresh_interval(l_param_refresh_interval.get_value<uint32_t>());
    }

  if(l_param_display_duration.value_set())
    {
      l_conf.set_display_duration(l_param_display_duration.get_value<uint32_t>());
    }

  // Preparing data containers
  std::vector<std::pair<uint32_t,uint32_t> > l_copper_cells;
  std::vector<std::pair<uint32_t,uint32_t> > l_queue_cells;
  std::vector<std::pair<uint32_t,uint32_t> > l_electron_cells;

  // Importing datas
  std::string l_file_name = l_conf.get_input_file_name();

  std::ifstream l_input_file(l_file_name.c_str());
  if(l_input_file==NULL)
    {
      std::cout << "ERROR : Unable to open file \"" << l_file_name << "\"" << std::endl ;
      exit(-1);
    }
  std::string l_line;
  uint32_t l_nb_line = 0;
  while(!l_input_file.eof())
    {
      getline(l_input_file,l_line);
      if(!l_input_file.eof())
	{
	  uint32_t l_size = l_line.length();
	  for(uint32_t l_index = 0 ; l_index < l_size ; ++l_index)
	    {
	      switch(l_line[l_index])
		{
		case '.':
		case ' ':
		  break;
		case 'E':
		  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(l_index,l_nb_line));
		  l_electron_cells.push_back(std::pair<uint32_t,uint32_t>(l_index,l_nb_line));
		  break;
		case 'Q':
		  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(l_index,l_nb_line));
		  l_queue_cells.push_back(std::pair<uint32_t,uint32_t>(l_index,l_nb_line));
		  break;
		case '#':
		  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(l_index,l_nb_line));
		  break;
		}
	    }
	}
      ++l_nb_line;
    }


  l_input_file.close();

#ifdef CLOCK

#include "../image/wireworld_computer.h"
  

  //Cross
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(0,1));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(1,0));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(1,2));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(2,1));

  //  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(0,1));
  //  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(1,1));
  //  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(2,1));

  // Line
  for(uint32_t l_x=3;l_x<20;++l_x)
    {
      l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(l_x,1));
    }
 
  l_queue_cells.push_back(std::pair<uint32_t,uint32_t>(0,1));

  l_electron_cells.push_back(std::pair<uint32_t,uint32_t>(1,0));

#endif

#ifdef TOTO
  //Horloge
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(30,5));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(31,5));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(32,5));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(32,4));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(33,4));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(34,4));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(34,5));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(34,6));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(34,7));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(33,7));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(32,7));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(32,6));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(35,5));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(36,6));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(36,7));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(36,8));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(35,9));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(35,10));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(34,11));


  //End of horloge


  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(0,25));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(1,25));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(2,25));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(3,24));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(4,24));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(3,26));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(4,26));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(3,25));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(5,25));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(6,25));
  
  l_queue_cells.push_back(std::pair<uint32_t,uint32_t>(0,25));

  l_electron_cells.push_back(std::pair<uint32_t,uint32_t>(1,25));
#endif

  
  //Creating world  
  wireworld l_world(l_copper_cells,l_electron_cells,l_queue_cells,l_conf);

  // Launching simulation
  l_world.run();

}
