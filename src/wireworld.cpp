/*
  This file is part of wireworld
  Copyright (C) 2011  Julien Thevenon ( julien_thevenon at yahoo.fr )

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include "wireworld.h"
#include "cell.h"
#include <map>

namespace wireworld
{
  //------------------------------------------------------------------------------
  wireworld::wireworld(const wireworld_common::wireworld_types::t_cell_list & p_copper_cells,
		       const wireworld_common::wireworld_types::t_cell_list & p_electron_cells,
		       const wireworld_common::wireworld_types::t_cell_list & p_queue_cells,
		       const wireworld_common::wireworld_configuration & p_conf
		       ):
    m_nb_cell(p_copper_cells.size()),
    m_copper_cells(new cell*[m_nb_cell]),
    m_to_check_cells(new cell*[m_nb_cell]),
    m_electron_cells(new cell*[m_nb_cell]),
    m_futur_electron_cells(new cell*[m_nb_cell]),
    m_queue_cells(new cell*[m_nb_cell]),
    m_to_check_start_index(0),
    m_to_check_current_index(0),
    m_electron_current_index(0),
    m_queue_current_index(0),
    m_conf(p_conf),
    m_stop(false),
    m_signal_handler(*this)
  {
    std::map<wireworld_common::wireworld_types::t_coordinates,cell*> l_bidimensionnal_world;

    uint32_t l_x_max = 0;
    uint32_t l_y_max = 0;

    //Creating copper cells
    uint32_t l_cell_index = 0;
    for(auto l_iter:p_copper_cells)
      {
	std::map<wireworld_common::wireworld_types::t_coordinates,cell*>::iterator l_bi_iter = l_bidimensionnal_world.find(l_iter);
	if(l_bi_iter == l_bidimensionnal_world.end())
	  {
	    uint32_t l_x = l_iter.first;
	    uint32_t l_y = l_iter.second;

	    if(l_x_max < l_x)
	      {
		l_x_max = l_x;
	      }
	    if(l_y_max < l_y)
	      {
		l_y_max = l_y;
	      }

	    cell * l_new_cell = new cell(l_x,l_y);
	    // Storing cell for later deletion
	    m_copper_cells[l_cell_index] = l_new_cell;
	    ++l_cell_index;

	    // Storing cell in virtual bidimensionnal world
	    l_bidimensionnal_world.insert(std::map<wireworld_common::wireworld_types::t_coordinates,cell*>::value_type(l_iter,l_new_cell));
	  }
	else
	  {
	    std::cout << "WARNING : a cell still exists at (" << l_iter.first << "," << l_iter.second << ") : skipping this one" << std::endl ;
	  }
      }

    l_x_max+=2;
    l_y_max+=2;
    m_gui.createWindow(l_x_max,l_y_max);

    //Determining neighbours
    for(auto l_iter_cell: l_bidimensionnal_world)
      {
	uint32_t l_x = l_iter_cell.first.first;
	uint32_t l_y = l_iter_cell.first.second;

	m_gui.displayCopper(l_x,l_y);

#ifdef DEBUG
	std::cout << "Determining neighbour for cell(" << l_x << "," << l_y << ")" << std::endl ;
#endif
	for(int32_t l_index_x = -1; l_index_x < 2; ++l_index_x)
	  {
	    for(int32_t l_index_y = -1; l_index_y < 2; ++l_index_y)
	      {
		if(l_index_x || l_index_y)
		  {
		    std::map<wireworld_common::wireworld_types::t_coordinates,cell*>::const_iterator l_iter_neighbour = l_bidimensionnal_world.find(wireworld_common::wireworld_types::t_coordinates(l_x + l_index_x , l_y + l_index_y));
		    if(l_iter_neighbour != l_bidimensionnal_world.end())
		      {
			l_iter_cell.second->add_neighbour(l_iter_neighbour->second);
		      }
		  }
	      }
	  }
      }

    //Instantiating queues
    // It need to be done before electron instaciation because only non queue cellules will need to be checked for the next step
    for(auto l_iter:p_queue_cells)
      {
	std::map<wireworld_common::wireworld_types::t_coordinates,cell*>::iterator l_bi_iter = l_bidimensionnal_world.find(l_iter);
	if(l_bi_iter != l_bidimensionnal_world.end())
	  {
	    set_queue(l_bi_iter->second);
	  }
	else
	  {
	    std::cout << "ERROR : you try to put a queue on coordinate(" << l_iter.first << "," << l_iter.second << ") which is not copper" << std::endl ;
	    exit(-1);
	  }
      }


    // Instantiating electrons
    for(auto l_iter:p_electron_cells)
      {
	std::map<wireworld_common::wireworld_types::t_coordinates,cell*>::iterator l_bi_iter = l_bidimensionnal_world.find(l_iter);
	if(l_bi_iter != l_bidimensionnal_world.end())
	  {
	    l_bi_iter->second->become_electron();
	  }
	else
	  {
	    std::cout << "ERROR : you try to put an electron on coordinate(" << l_iter.first << "," << l_iter.second << ") which is not copper" << std::endl ;
	    exit(-1);
	  }
      }

    // Signaling electrons
    // It need to be done after electron instanciation because only copper cells will be added to list of cells to be checked next step
    for(auto l_iter: p_electron_cells)
      {
	std::map<wireworld_common::wireworld_types::t_coordinates,cell*>::iterator l_bi_iter = l_bidimensionnal_world.find(l_iter);
	if(l_bi_iter != l_bidimensionnal_world.end())
	  {
	    signal_electron(l_bi_iter->second);
	  }
	else
	  {
	    std::cout << "ERROR : you try to put an electron on coordinate(" << l_iter.first << "," << l_iter.second << ") which is not copper" << std::endl ;
	    exit(-1);
	  }
      }


    m_gui.refresh();


  }

  //------------------------------------------------------------------------------
  wireworld::~wireworld(void)
  {
    for(uint32_t l_index = 0 ; l_index < m_nb_cell ; ++l_index)
      {
	delete m_copper_cells[l_index];
      }
    delete[] m_copper_cells;
    delete[] m_to_check_cells;
    delete[] m_electron_cells;
    delete[] m_futur_electron_cells;
    delete[] m_queue_cells;
  }

  //------------------------------------------------------------------------------
  void wireworld::set_queue(cell *p_cell)
  {
    m_queue_cells[m_queue_current_index] = p_cell;
    ++m_queue_current_index;
    p_cell->become_queue();
    m_gui.displayQueue(p_cell->getX(),p_cell->getY());
  }

  //------------------------------------------------------------------------------
  void wireworld::signal_electron(cell *p_cell)
  {
    uint32_t l_nb_neighbour = p_cell->get_nb_neighbour();
    for(uint32_t l_index =0 ; l_index < l_nb_neighbour; ++l_index)
      {
	cell * l_neighbour = p_cell->get_neighbour(l_index);
	if(l_neighbour->is_copper())
	  {
	    if(!l_neighbour->to_be_checked())
	      {
		m_to_check_cells[m_to_check_current_index] = l_neighbour;
		m_to_check_current_index = (m_to_check_current_index + 1) % m_nb_cell;
	      }
	    l_neighbour->signal_electron();
	  }
      }
    m_electron_cells[m_electron_current_index] = p_cell;
    ++m_electron_current_index;
    m_gui.displayElectron(p_cell->getX(),p_cell->getY());
  }

  //------------------------------------------------------------------------------
  void wireworld::run(void)
  {
    uint32_t l_nb_max = m_conf.get_nb_max_cycle();
    uint32_t l_start_cycle = m_conf.get_start_cycle();
    uint32_t l_refresh_interval = m_conf.get_refresh_interval();
    uint32_t l_display_duration = m_conf.get_display_duration();
    bool l_continu = false;
    uint32_t l_nb_cycle = 1;
    do
      {
	l_continu = false;
	//display
	/* Computer */ //	if(l_nb_cycle > 38800 || (!(l_nb_cycle % 1000)) )
	/* Display */	//	if(! (l_nb_cycle % 100))
	/* Display */
	if(l_nb_cycle >= l_start_cycle && !(l_nb_cycle % l_refresh_interval))
	  {
	    m_gui.refresh();
	    std::cout << "=> step " << l_nb_cycle << std::endl ;
	    SDL_Delay(l_display_duration);
	  }

	// all queue become copper
	for(uint32_t l_index = 0 ; l_index < m_queue_current_index; l_index++)
	  {
	    m_queue_cells[l_index]->become_copper();
	    m_gui.displayCopper( m_queue_cells[l_index]->getX(), m_queue_cells[l_index]->getY());

	  }

	// All electron become queues
	for(uint32_t l_index = 0 ; l_index < m_electron_current_index; l_index++)
	  {
	    m_electron_cells[l_index]->become_queue();
	    l_continu = true;
	    m_gui.displayQueue( m_electron_cells[l_index]->getX(), m_electron_cells[l_index]->getY());
	  }

	// Switching electron and queue arrays
	m_queue_current_index = m_electron_current_index ;
	m_electron_current_index = 0;
	cell ** l_tmp_cells = m_queue_cells;
	m_queue_cells = m_electron_cells;
	m_electron_cells = l_tmp_cells;
    
	// Checking cells to become electrons
	uint32_t l_end_index = m_to_check_current_index;
	uint32_t l_futur_electron_current_index = 0;
	while(m_to_check_start_index != l_end_index)
	  {
	    cell * l_cell = m_to_check_cells[m_to_check_start_index];
	    uint32_t l_nb_electron_around = l_cell->get_nb_electron_around();
	    if(l_nb_electron_around > 0 && l_nb_electron_around < 3)
	      {
		m_futur_electron_cells[l_futur_electron_current_index] = l_cell;
		l_cell->become_electron();
		++l_futur_electron_current_index;
		l_continu = true;
	      }
	    l_cell->checked();
	    m_to_check_start_index = (1 + m_to_check_start_index) % m_nb_cell;
	  }

	// All futur electron become electron
	for(uint32_t l_index = 0 ; l_index < l_futur_electron_current_index; l_index++)
	  {
	    signal_electron(m_futur_electron_cells[l_index]);
	  }
	
	
	++l_nb_cycle;


      }while(l_continu && l_nb_cycle <= l_nb_max && !m_stop);
    std::cout << "Nothing more to simulate"<< std::endl;
    
    
  }
}
//EOF
