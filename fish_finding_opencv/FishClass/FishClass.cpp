#include "FishClass.h"
#include <iostream>
#include <vector>

FishClass::FishClass(double width_in, double length_in, double x_pos_in, double y_pos_in)
{ 
	
	is_fish = true;
    	width.push_back(width_in);
	length.push_back(length_in);
	x_pos.push_back(x_pos_in);
	y_pos.push_back(y_pos_in);
        IsFish();
	std::cout << width.back()*length.back() <<std::endl;
	std::cout << "Is fish?  " << is_fish << std::endl;
}

bool FishClass::IsFish()
{

  if (width.back()*length.back() < 7000. || width.back()*length.back() > 150000){ 
	std::cout << width.back()*length.back() <<std::endl;
      std::cout << width.back()<< "x" <<length.back()<< "  is    "<< width.back()*length.back() <<std::endl;
     // Capsize is 45*45, including some margin of error we arrive at 60
     is_fish = false;
     }
     return is_fish;    
}
