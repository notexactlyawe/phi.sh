#include "FishClass.h"
#include <iostream>
#include <vector>

FishClass::FishClass(double width_in, double length_in, double x_pos_in, double y_pos_in)
{
 	width.push_back(width_in);
	length.push_back(length_in);
	x_pos.push_back(x_pos_in);
	y_pos.push_back(y_pos_in);
}

bool FishClass::IsFish()
{
  return False;
}
