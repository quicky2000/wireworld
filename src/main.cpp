#include <stdint.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <iostream>
#include <assert.h>

class cell
{
public:
  cell(uint32_t p_x,uint32_t p_y):
    m_x(p_x),
    m_y(p_y),
    m_current_state(COPPER),
    m_to_check(false),
    m_nb_neighbours(0),
    m_nb_electrons_around(0)
  {
    std::cout << "Create cell(" << m_x << "," << m_y << ")" << std::endl ;
  }

  void add_neighbour(cell * p_cell)
  {
    assert(m_nb_neighbours<8);
    m_neighbours[m_nb_neighbours] = p_cell;
    ++m_nb_neighbours;
  }

  inline uint32_t get_nb_neighbour(void)const
  {
    return m_nb_neighbours;
  }

  inline cell* get_neighbour(uint32_t p_index)
  {
    assert(p_index<8);
    return m_neighbours[p_index];
  }

  inline void become_electron(void)
  {
    std::cout << "Cell(" << m_x << "," << m_y << ") set as electron" << std::endl;
    assert(m_current_state == COPPER);
    m_current_state = ELECTRON;
  }

  inline void become_queue(void)
  {
    std::cout << "Cell(" << m_x << "," << m_y << ") set as queue" << std::endl;
    m_current_state = QUEUE;
  }

  inline void become_copper(void)
  {
    std::cout << "Cell(" << m_x << "," << m_y << ") set as copper" << std::endl;
    assert(m_current_state = QUEUE);
    m_to_check = false;
    m_nb_electrons_around = 0;
    m_current_state = COPPER;
  }

public:
  inline bool is_copper(void)const
  {
    return m_current_state==COPPER;
  }

  inline void signal_electron(void)
  {
    m_to_check = true;
    ++m_nb_electrons_around;
  }
  
  inline uint32_t get_nb_electron_around(void)const
    {
      return m_nb_electrons_around;
    }

  inline bool to_be_checked(void)const
  {
    return m_to_check ;
  }

private:
  typedef enum cell_state {COPPER,QUEUE,ELECTRON} t_cell_state;
  uint32_t m_x;
  uint32_t m_y;
  t_cell_state m_current_state;
  bool m_to_check;
  uint32_t m_nb_neighbours;
  cell* m_neighbours[8];
  uint32_t m_nb_electrons_around;
};

class wireworld
{
public:
  wireworld(const std::vector<std::pair<uint32_t,uint32_t> > & p_copper_cells,
	    const std::vector<std::pair<uint32_t,uint32_t> > & p_electron_cells,
	    const std::vector<std::pair<uint32_t,uint32_t> > & p_queue_cells
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
    m_queue_current_index(0)
  {
    std::map<std::pair<uint32_t,uint32_t>,cell*> l_bidimensionnal_world;

    //Creating copper cells
    std::vector<std::pair<uint32_t,uint32_t> >::const_iterator l_iter = p_copper_cells.begin();
    std::vector<std::pair<uint32_t,uint32_t> >::const_iterator l_iter_end = p_copper_cells.end();
    uint32_t l_cell_index = 0;
    while(l_iter != l_iter_end)
      {
	std::map<std::pair<uint32_t,uint32_t>,cell*>::iterator l_bi_iter = l_bidimensionnal_world.find(*l_iter);
	if(l_bi_iter == l_bidimensionnal_world.end())
	  {
	    cell * l_new_cell = new cell(l_iter->first,l_iter->second);
	    // Storing cell for later deletion
	    m_copper_cells[l_cell_index] = l_new_cell;
	    ++l_cell_index;

	    // Storing cell in virtual bidimensionnal world
	    l_bidimensionnal_world.insert(std::map<std::pair<uint32_t,uint32_t>,cell*>::value_type(*l_iter,l_new_cell));
	  }
	else
	  {
	    std::cout << "WARNING : a cell still exists at (" << l_iter->first << "," << l_iter->second << ") : skipping this one" << std::endl ;
	  }
	
	++l_iter;
      }

    //Determining neighbours
    std::map<std::pair<uint32_t,uint32_t>,cell*>::const_iterator l_iter_cell = l_bidimensionnal_world.begin();
    std::map<std::pair<uint32_t,uint32_t>,cell*>::const_iterator l_iter_cell_end = l_bidimensionnal_world.end();
    while(l_iter_cell != l_iter_cell_end)
      {
	uint32_t l_x = l_iter_cell->first.first;
	uint32_t l_y = l_iter_cell->first.second;
	std::cout << "Determining neighbour for cell(" << l_x << "," << l_y << ")" << std::endl ;
	for(int32_t l_index_x = -1; l_index_x < 2; ++l_index_x)
	  {
	    for(int32_t l_index_y = -1; l_index_y < 2; ++l_index_y)
	      {
		if(l_index_x || l_index_y)
		  {
		    std::map<std::pair<uint32_t,uint32_t>,cell*>::const_iterator l_iter_neighbour = l_bidimensionnal_world.find(std::pair<uint32_t,uint32_t>(l_x + l_index_x , l_y + l_index_y));
		    if(l_iter_neighbour != l_bidimensionnal_world.end())
		      {
			l_iter_cell->second->add_neighbour(l_iter_neighbour->second);
		      }
		  }
	      }
	  }
	++l_iter_cell;
      }

    //Instantiating queues
    // It need to be done before electron instaciation because only non queue cellules will need to be checked for the next step
    l_iter = p_queue_cells.begin();
    l_iter_end = p_queue_cells.end();
    while(l_iter != l_iter_end)
      {
	std::map<std::pair<uint32_t,uint32_t>,cell*>::iterator l_bi_iter = l_bidimensionnal_world.find(*l_iter);
	if(l_bi_iter != l_bidimensionnal_world.end())
	  {
	    set_queue(l_bi_iter->second);
	  }
	else
	  {
	    std::cout << "ERROR : you try to put a queue on coordinate(" << l_iter->first << "," << l_iter->second << ") which is not copper" << std::endl ;
	    exit(-1);
	  }
	++l_iter;
      }


    // Instantiating electrons
    // It need to be done after queue instanciation because non queue cells will be add to list of cells to be checked next step
    l_iter = p_electron_cells.begin();
    l_iter_end = p_electron_cells.end();
    while(l_iter != l_iter_end)
      {
	std::map<std::pair<uint32_t,uint32_t>,cell*>::iterator l_bi_iter = l_bidimensionnal_world.find(*l_iter);
	if(l_bi_iter != l_bidimensionnal_world.end())
	  {
	    set_electron(l_bi_iter->second);
	  }
	else
	  {
	    std::cout << "ERROR : you try to put an electron on coordinate(" << l_iter->first << "," << l_iter->second << ") which is not copper" << std::endl ;
	    exit(-1);
	  }
	++l_iter;
      }





  }

  ~wireworld(void)
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

  void set_queue(cell *p_cell)
  {
    m_queue_cells[m_queue_current_index] = p_cell;
    ++m_queue_current_index;
    p_cell->become_queue();
  }

  void set_electron(cell *p_cell)
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
    p_cell->become_electron();
  }

  void run(uint32_t p_nb_max)
  {
    bool l_continu = false;
    uint32_t l_nb_cycle = 1;
    do
      {
	l_continu = false;
	//    sleep(1);
	//display
	std::cout << "=> step " << l_nb_cycle << std::endl ;

	// all queue become copper
	for(uint32_t l_index = 0 ; l_index < m_queue_current_index; l_index++)
	  {
	    m_queue_cells[l_index]->become_copper();
	  }

	// All electron become queues
	for(uint32_t l_index = 0 ; l_index < m_electron_current_index; l_index++)
	  {
	    m_electron_cells[l_index]->become_queue();
	    l_continu = true;
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
		++l_futur_electron_current_index;
		l_continu = true;
	      }
	    m_to_check_start_index = (1 + m_to_check_start_index) % m_nb_cell;
	  }

	// All futur electron become electron
	for(uint32_t l_index = 0 ; l_index < l_futur_electron_current_index; l_index++)
	  {
	    set_electron(m_futur_electron_cells[l_index]);
	  }
	
	
	++l_nb_cycle;
      }while(l_continu && l_nb_cycle <= p_nb_max);
    std::cout << "Nothing more to simulate"<< std::endl;
    
    
  }

private:
  uint32_t m_nb_cell;
  cell** m_copper_cells;
  cell** m_to_check_cells;
  cell** m_electron_cells;
  cell** m_futur_electron_cells;
  cell** m_queue_cells;
  uint32_t m_to_check_start_index;
  uint32_t m_to_check_current_index;
  uint32_t m_electron_current_index;
  uint32_t m_queue_current_index;
};


int main(void)
{
  std::vector<std::pair<uint32_t,uint32_t> > l_copper_cells;
  std::vector<std::pair<uint32_t,uint32_t> > l_queue_cells;
  std::vector<std::pair<uint32_t,uint32_t> > l_electron_cells;

#ifdef CLOCK
  //Cross
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(0,1));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(1,0));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(1,2));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(2,1));

  //  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(0,1));
  //  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(1,1));
  //  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(2,1));

  // Line
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(3,1));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(4,1));
 
  l_queue_cells.push_back(std::pair<uint32_t,uint32_t>(0,1));

  l_electron_cells.push_back(std::pair<uint32_t,uint32_t>(1,0));

#endif
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(0,5));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(1,5));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(2,5));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(3,4));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(4,4));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(3,6));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(4,6));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(3,5));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(5,5));
  l_copper_cells.push_back(std::pair<uint32_t,uint32_t>(6,5));
  
  l_queue_cells.push_back(std::pair<uint32_t,uint32_t>(0,5));

  l_electron_cells.push_back(std::pair<uint32_t,uint32_t>(1,5));

  
  
  wireworld l_world(l_copper_cells,l_electron_cells,l_queue_cells);
  l_world.run(10);
  
}
