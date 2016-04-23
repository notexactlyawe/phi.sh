#include "FishClass.h"
#include <iostream>
#include <vector>

FishClass::FishClass(double width_in, double length_in, double x_pos_in, double y_pos_in)
{ is_fish = true;
 	
        if (IsFish()){
	    	width.push_back(width_in);
		length.push_back(length_in);
		x_pos.push_back(x_pos_in);
		y_pos.push_back(y_pos_in);
	 	};
}

bool FishClass::IsFish()

{
  if (width.back()*length.back() <50*50){ 
     // Capsize is 45*45, including some margin of error we arrive at 60
     is_fish = false;
     }
     return is_fish;    
}
