#ifndef _CELL_H_
#define _CELL_H_

#include <stdint.h>
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

#endif /* _CELL_H_H */
