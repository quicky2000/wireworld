#include "parameter_manager.h"

#include "SDL/SDL.h"
#include <stdint.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <assert.h>

class wireworld_configuration
{
public:
  wireworld_configuration(void);

  void set_input_file_name(const std::string & p_name);
  const std::string & get_input_file_name(void)const;
  void set_nb_max_cycle(uint32_t p_nb_max_cycle);
  uint32_t get_nb_max_cycle(void)const;
  void set_start_cycle(uint32_t p_start_cycle);
  uint32_t get_start_cycle(void)const;
  void set_refresh_interval(uint32_t p_refresh_interval);
  uint32_t get_refresh_interval(void)const;
  void set_display_duration(uint32_t p_display_duration);
  uint32_t get_display_duration(void)const;
private:
  std::string m_input_file_name;
  uint32_t m_nb_max_cycle;
  uint32_t m_start_cycle;
  uint32_t m_refresh_interval;
  uint32_t m_display_duration;
};

wireworld_configuration::wireworld_configuration(void):
  m_input_file_name("wireworld.txt"),
  m_nb_max_cycle(1000),
  m_start_cycle(0),
  m_refresh_interval(1),
  m_display_duration(500)
{
}

void wireworld_configuration::set_input_file_name(const std::string & p_name)
{
  m_input_file_name = p_name;
}

const std::string & wireworld_configuration::get_input_file_name(void)const
{
  return m_input_file_name;
}

void wireworld_configuration::set_nb_max_cycle(uint32_t p_nb_max_cycle)
{
  m_nb_max_cycle = p_nb_max_cycle;
}

uint32_t wireworld_configuration::get_nb_max_cycle(void)const
{
  return m_nb_max_cycle;
}

void wireworld_configuration::set_start_cycle(uint32_t p_start_cycle)
{
  m_start_cycle = p_start_cycle;
}

uint32_t wireworld_configuration::get_start_cycle(void)const
{
  return m_start_cycle;
}

void wireworld_configuration::set_refresh_interval(uint32_t p_refresh_interval)
{
  m_refresh_interval = p_refresh_interval;
}

uint32_t wireworld_configuration::get_refresh_interval(void)const
{
  return m_refresh_interval;
}

void wireworld_configuration::set_display_duration(uint32_t p_display_duration)
{
  m_display_duration = p_display_duration;
}

uint32_t wireworld_configuration::get_display_duration(void)const
{
  return m_display_duration;
}


class my_gui
{
public:
  my_gui(void):
    m_screen(NULL),
    m_coef(20),
    m_copper_color(0),
    m_queue_color(0),
    m_electron_color(0)
  {
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
      {
	std::cout << "Unable to initialize SDL: " << SDL_GetError() << std::endl ;
      }
  }

  void createWindow(uint32_t p_width,uint32_t p_height)
  {
    const SDL_VideoInfo *l_video_info = SDL_GetVideoInfo();

    assert(l_video_info);
#if SDL_COMPILEDVERSION >= SDL_VERSIONNUM(1, 2, 14)
    std::cout << "Current resolution : " << l_video_info->current_w << "x" << l_video_info->current_h << " with Pixel Format " << ((uint32_t)l_video_info->vfmt->BitsPerPixel) << " bits per pixel" << std::endl ;
    m_coef = (l_video_info->current_w / p_width < l_video_info->current_h / p_height ? l_video_info->current_w / p_width : l_video_info->current_h / p_height );
#else
    m_coef = (1900 / p_width < 1130 / p_height ? 1900 / p_width : 1130 / p_height );
#endif
    if(!m_coef) m_coef =1 ;
    std::cout << "coef = " << m_coef << std::endl ;


    p_width = p_width * m_coef;
    p_height = p_height * m_coef;
    m_screen = SDL_SetVideoMode(p_width,p_height,32,SDL_SWSURFACE);
    if(m_screen == NULL)
      {
	std::cout << "Unable to set video mode to " << p_width << "*"<< p_height << "*32" << std::endl ;
	SDL_Quit();
      }
    m_copper_color = getColorCode(255,160,0);
    m_queue_color = getColorCode(0,160,255);
    m_electron_color = getColorCode(255,255,255);
  }

  uint32_t getColorCode(uint8_t r,uint8_t g,uint8_t b)
  {
    return SDL_MapRGB(m_screen->format,r,g,b);
  }

  void refresh(void)
  {
    SDL_UpdateRect(m_screen,0,0,0,0);
  }

  void displayCopper(uint32_t p_x,uint32_t p_y)
  {
    setPixel(p_x,p_y,m_copper_color);
  }

  void displayQueue(uint32_t p_x,uint32_t p_y)
  {
    setPixel(p_x,p_y,m_queue_color);
  }

  void displayElectron(uint32_t p_x,uint32_t p_y)
  {
    setPixel(p_x,p_y,m_electron_color);
  }

  ~my_gui(void)
  {
    SDL_Quit();
  }
private:
  void setPixel(uint32_t p_x,uint32_t p_y,uint32_t p_color)
  {
    if ( SDL_MUSTLOCK(m_screen) )
      {
	if ( SDL_LockSurface(m_screen) < 0 )
	  {
	    exit(-1);
	  }
      }
    for(uint32_t l_x = p_x * m_coef;l_x < m_coef *(p_x + 1);++l_x)
      {
	for(uint32_t l_y = p_y * m_coef;l_y < m_coef *(p_y + 1);++l_y)
	  {
	    uint32_t *l_bufp = (uint32_t *)m_screen->pixels + l_y * m_screen->pitch/4 + l_x;
	    *l_bufp = p_color;
	  }
      }
	  
    if ( SDL_MUSTLOCK(m_screen) )
      {
	SDL_UnlockSurface(m_screen);
      }

  }

  SDL_Surface *m_screen;
  uint32_t m_coef;
  uint32_t m_copper_color;
  uint32_t m_queue_color;
  uint32_t m_electron_color;
};

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
#ifdef DEBUG
    std::cout << "Create cell(" << m_x << "," << m_y << ")" << std::endl ;
#endif
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
#ifdef DEBUG
    std::cout << "Cell(" << m_x << "," << m_y << ") set as electron" << std::endl;
#endif
    assert(m_current_state == COPPER);
    m_current_state = ELECTRON;
  }

  inline void become_queue(void)
  {
#ifdef DEBUG
    std::cout << "Cell(" << m_x << "," << m_y << ") set as queue" << std::endl;
#endif
    m_current_state = QUEUE;
  }

  inline void checked(void)
  {
    m_to_check = false;
    m_nb_electrons_around = 0;
  }

  inline void become_copper(void)
  {
#ifdef DEBUG
    std::cout << "Cell(" << m_x << "," << m_y << ") set as copper" << std::endl;
#endif
    assert(m_current_state = QUEUE);
    m_current_state = COPPER;
  }

public:
  inline uint32_t getX(void)const
  {
    return m_x;
  }

  inline uint32_t getY(void)const
  {
    return m_y;
  }

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
#ifdef DEBUG
      std::cout << "Cell(" << m_x << "," << m_y << ") has " << m_nb_electrons_around << " electrons around "<< std::endl;
#endif
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
	    const std::vector<std::pair<uint32_t,uint32_t> > & p_queue_cells,
	    const wireworld_configuration & p_conf
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
    m_conf(p_conf)
  {
    std::map<std::pair<uint32_t,uint32_t>,cell*> l_bidimensionnal_world;

    uint32_t l_x_max = 0;
    uint32_t l_y_max = 0;

    //Creating copper cells
    std::vector<std::pair<uint32_t,uint32_t> >::const_iterator l_iter = p_copper_cells.begin();
    std::vector<std::pair<uint32_t,uint32_t> >::const_iterator l_iter_end = p_copper_cells.end();
    uint32_t l_cell_index = 0;
    while(l_iter != l_iter_end)
      {
	std::map<std::pair<uint32_t,uint32_t>,cell*>::iterator l_bi_iter = l_bidimensionnal_world.find(*l_iter);
	if(l_bi_iter == l_bidimensionnal_world.end())
	  {
	    uint32_t l_x = l_iter->first;
	    uint32_t l_y = l_iter->second;

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
	    l_bidimensionnal_world.insert(std::map<std::pair<uint32_t,uint32_t>,cell*>::value_type(*l_iter,l_new_cell));
	  }
	else
	  {
	    std::cout << "WARNING : a cell still exists at (" << l_iter->first << "," << l_iter->second << ") : skipping this one" << std::endl ;
	  }
	
	++l_iter;
      }

    l_x_max+=2;
    l_y_max+=2;
    m_gui.createWindow(l_x_max,l_y_max);

    //Determining neighbours
    std::map<std::pair<uint32_t,uint32_t>,cell*>::const_iterator l_iter_cell = l_bidimensionnal_world.begin();
    std::map<std::pair<uint32_t,uint32_t>,cell*>::const_iterator l_iter_cell_end = l_bidimensionnal_world.end();
    while(l_iter_cell != l_iter_cell_end)
      {
	uint32_t l_x = l_iter_cell->first.first;
	uint32_t l_y = l_iter_cell->first.second;

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
    l_iter = p_electron_cells.begin();
    l_iter_end = p_electron_cells.end();
    while(l_iter != l_iter_end)
      {
	std::map<std::pair<uint32_t,uint32_t>,cell*>::iterator l_bi_iter = l_bidimensionnal_world.find(*l_iter);
	if(l_bi_iter != l_bidimensionnal_world.end())
	  {
	    l_bi_iter->second->become_electron();
	  }
	else
	  {
	    std::cout << "ERROR : you try to put an electron on coordinate(" << l_iter->first << "," << l_iter->second << ") which is not copper" << std::endl ;
	    exit(-1);
	  }
	++l_iter;
      }

    // Signaling electrons
    // It need to be done after electron instanciation because only copper cells will be added to list of cells to be checked next step
    l_iter = p_electron_cells.begin();
    l_iter_end = p_electron_cells.end();
    while(l_iter != l_iter_end)
      {
	std::map<std::pair<uint32_t,uint32_t>,cell*>::iterator l_bi_iter = l_bidimensionnal_world.find(*l_iter);
	if(l_bi_iter != l_bidimensionnal_world.end())
	  {
	    signal_electron(l_bi_iter->second);
	  }
	else
	  {
	    std::cout << "ERROR : you try to put an electron on coordinate(" << l_iter->first << "," << l_iter->second << ") which is not copper" << std::endl ;
	    exit(-1);
	  }
	++l_iter;
      }


    m_gui.refresh();


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
    m_gui.displayQueue(p_cell->getX(),p_cell->getY());
  }

  void signal_electron(cell *p_cell)
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

  void run()
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


      }while(l_continu && l_nb_cycle <= l_nb_max);
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
  my_gui m_gui;
  wireworld_configuration m_conf;
};


int main(int argc,char ** argv)
{

  // Defining application command line parameters
  parameter_manager l_param_manager("wireworld.exe","--",1);
  parameter_if l_param_file("input_file");
  parameter_if * l_param_nb_max_cycle = new parameter_if("nb_max_cycle");
  parameter_if * l_param_start_cycle = new parameter_if("start_cycle",true);
  parameter_if * l_param_delay = new parameter_if("frame_delay",true);
  parameter_if * l_param_refresh_interval = new parameter_if("refresh_interval",true);
  parameter_if * l_param_display_duration = new parameter_if("display_duration",true);
  l_param_manager.add(&l_param_file);
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

  if(l_param_nb_max_cycle->value_set())
    {
      l_conf.set_nb_max_cycle(l_param_nb_max_cycle->get_value<uint32_t>());
    }

  if(l_param_start_cycle->value_set())
    {
      l_conf.set_start_cycle(l_param_start_cycle->get_value<uint32_t>());
    }

  if(l_param_refresh_interval->value_set())
    {
      l_conf.set_refresh_interval(l_param_refresh_interval->get_value<uint32_t>());
    }

  if(l_param_display_duration->value_set())
    {
      l_conf.set_display_duration(l_param_display_duration->get_value<uint32_t>());
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
