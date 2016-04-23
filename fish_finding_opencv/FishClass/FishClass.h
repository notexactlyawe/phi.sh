#ifndef FISHCLASS_H
#define FISHCLASS_H

#include <iostream>
#include <vector>


class FishClass
{
/*! 
Information of individual fish recognised in the picture
*/

public:
	// Sorry, everything will be public this weekend...
	std::vector<double> width;
	std::vector<double> length;
	std::vector<double> x_pos;
	std::vector<double> y_pos;

	// Range of expected next location 
	// (xmin, xmax)
	std::vector<double> x_expected_range;
	// (ymin, ymax)
	std::vector<double> y_expected_range;
	bool is_fish;

public:
// our constructor
/*! Null constructor */
  FishClass(){};
/*! Constructor 
Instantiate a fish with the known data from the image 
*/
  FishClass(double width_in, double length_in, double x_pos_in, double y_pos_in);

/*! Give a possible location for the next frame to help the ID process... */
  void FindExpectedRange();

/*! Check if is fish or noise (if size is large enough) */
  bool IsFish();

  /*!Destructor */
  ~FishClass(){};

};

#endif
