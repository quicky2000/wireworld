#ifndef _WIREWORLD_H_
#define _WIREWORLD_H_

#include "wireworld_configuration.h"
#include "wireworld_gui.h"
#include <stdint.h>
#include <vector>

class cell;

class wireworld
{
public:
  wireworld(const std::vector<std::pair<uint32_t,uint32_t> > & p_copper_cells,
	    const std::vector<std::pair<uint32_t,uint32_t> > & p_electron_cells,
	    const std::vector<std::pair<uint32_t,uint32_t> > & p_queue_cells,
	    const wireworld_configuration & p_conf
	    );

  ~wireworld(void);

  void set_queue(cell *p_cell);
  void signal_electron(cell *p_cell);
  void run(void);

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
  wireworld_gui m_gui;
  wireworld_configuration m_conf;
};

#endif /* _WIREWORLD_H_ */