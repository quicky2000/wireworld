#include <iostream>
#include <stdint.h>
#include <stdlib.h>
using namespace std;

int main(int argc, char ** argv)
{
  if(argc != 2)
    {
      cout << "Usage is binary2bcd <number>" << endl ;
      exit(-1);
    }
  uint64_t l_number = strtoll(argv[1],NULL,0);
  cout << "Input number is " << l_number << endl ;

  uint32_t l_bottom_loop[] = {
    0xFFFF63C0,
    0x4E20,
    0x2710,
    0x7D0,
    0xFA0,
    0x7D0,
    0x3E8,
    0xC8,
    0x190,
    0xC8,
    0x64,
    0x14,
    0x28,
    0x14,
    0xA,
    0x2,
    0x4,
    0x2,
    0x1,
    0x1
  };

  uint32_t l_top_loop[] = {
    0xFFFFFF80,
    0x0,
    0x0,
    0x1890,
    0x0,
    0x0,
    0x0,
    0x3E8,
    0x0,
    0x0,
    0x0,
    0x44,
    0x0,
    0x0,
    0x0,
    0xA,
    0x0,
    0x0,
    0x0,
    0x1,
  };

  uint64_t l_adder_content = l_number;
  uint64_t l_adder_full = 0xFFFFFFFF;
  uint32_t l_display[5] = {0,0,0,0,0};
  uint32_t l_display_index = 0;
  uint32_t l_power_index = 2;
  uint32_t l_carry = 0;
  for(uint32_t l_index = 0; l_index < 20 ; ++l_index)
    {
      cout << "carry 0x" << hex << l_carry << dec << endl ;
      cout << "l_top_loop[" << l_index << "] 0x" << hex << l_top_loop[l_index] << dec << endl ; 
	  l_carry = l_carry & ( ~ (l_top_loop[l_index]));
	  cout << "Adjusted carry 0x" << hex << l_carry << dec << endl ;
      cout << "l_bottom_loop[" << l_index << "] 0x" << hex << l_bottom_loop[l_index] << dec << endl ; 
	  uint32_t l_to_add = l_carry ^ l_bottom_loop[l_index];
	  cout << "[" << l_index << "]: Adder content : " << hex << l_adder_content << dec << endl ;
	  cout << "To add 0x" << hex << l_to_add <<dec << endl ;
      l_adder_content +=  l_to_add;
      cout << "==> [" << l_index << "]: Adder content : " << hex << l_adder_content << dec ;
      if(l_adder_content > l_adder_full)
	{
	  cout << "\tOverflow !" ;
	  l_adder_content = (l_adder_content & 0xFFFFFFFF) + 1;
	  l_carry = 0xFFFFFFFF;

	  l_display[l_display_index]+= 1 << l_power_index;
	}
      else
	{
	  l_carry = 0;
	}
      l_power_index = (l_power_index > 0 ? l_power_index - 1 : 3);
      if(l_power_index == 3)
	{
	  ++l_display_index;
	}
      cout << endl;
    }

  for(uint32_t l_index = 0 ; l_index < 5; ++l_index)
    {
      cout << "|" << l_display[l_index] ;
    }
  cout << "|" << endl ;
}
