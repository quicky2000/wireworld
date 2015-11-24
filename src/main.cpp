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
#include "wireworld_parameters.h"
#include "wireworld_parser.h"
#include "wireworld_types.h"
#include "wireworld.h"

int main(int argc,char ** argv)
{

  wireworld_common::wireworld_configuration l_config;
  wireworld_common::wireworld_parameters::configure(argc,argv,l_config);

  wireworld_common::wireworld_types::t_cell_list l_copper_cells;
  wireworld_common::wireworld_types::t_cell_list l_queue_cells;
  wireworld_common::wireworld_types::t_cell_list l_electron_cells;

  wireworld_common::wireworld_parser::parse(l_config.get_input_file_name(),l_copper_cells,l_queue_cells,l_electron_cells);

  //Creating world  
  wireworld::wireworld l_world(l_copper_cells,l_electron_cells,l_queue_cells,l_config);

  // Launching simulation
  l_world.run();

}
//EOF
