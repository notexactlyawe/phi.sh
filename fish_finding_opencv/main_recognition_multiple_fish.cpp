#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include "FishClass/FishClass.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
using namespace cv;
using namespace std;

Mat src; Mat src_gray;
int thresh = 95;
int max_thresh = 255;
RNG rng(12345);
std::vector<FishClass> my_fish_vector; // Vector with all the recognized fishes during the movie
std::vector<FishClass> my_fish_frame;  // Features recognized at a given frame

/// Function header
void thresh_callback(int, void*);
void update_fish_list();

/** @function main */
int main( int argc, char** argv )
{
  double calibration_factor = 0.04;
for(int frame_idx =33; frame_idx < 34; frame_idx++)
  {
  int idx = frame_idx;
  ostringstream s;
  s << idx;
  string str_filename = "stream3fish/videoframe"  + s.str() + ".jpg";
  string out_filename = "output" + s.str() + ".txt";
  char filename[] = "stream1fish/videoframe15.jpg";	
  /// Load source image and convert it to gray
  src = imread( str_filename.c_str(), 1 );
cout << "-------------" << str_filename.c_str() << " ------------" <<endl;
  /// Convert image to gray and blur it
  cvtColor( src, src_gray, CV_BGR2GRAY );
  blur( src_gray, src_gray, Size(3,3) );

  /// Create Window
  char* source_window = "Source";
  namedWindow( source_window, CV_WINDOW_AUTOSIZE );
  imshow( source_window, src );

  createTrackbar( " Threshold:", "Source", &thresh, max_thresh, thresh_callback );
  waitKey(0);
  thresh_callback( 0, 0);
  update_fish_list();

	// Example to dump data to file
  ofstream output (out_filename.c_str());
  if (output.is_open()) 
	{
		for(int k =0; k<my_fish_frame.size();k++)
		{
			//output << to_string(my_fish_frame[k].width) << "," << to_string(my_fish_frame[k].length) << endl;
			output << my_fish_frame[k].width[0] << "," << my_fish_frame[k].length[0] << "," << my_fish_frame[k].x_pos[0] <<","<<  my_fish_frame[k].y_pos[0] << endl;		
		}
	}
  
  output.close();

}

  // Toy fish
  //FishClass fishy_fish(0.3,0.2,1.1,2.4);
  //my_fish_vector.push_back(fishy_fish);
vector<double> mean_width_v;
vector<double> mean_length_v;
for (int k=0; k<my_fish_vector.size();k++)
	{
	double mean_width = 0.;
	double mean_length = 0.;
	for(int j=0; j<my_fish_vector[k].x_pos.size();j++)
	{
	cout << my_fish_vector[k].x_pos[j] << "  " << my_fish_vector[k].y_pos[j] <<"  " << my_fish_vector[k].width[j] << "  " << my_fish_vector[k].length[j] <<"  " << endl;
	if(my_fish_vector[k].width[j] < my_fish_vector[k].length[j])
		{
		mean_width+= my_fish_vector[k].width[j];
		mean_length+=my_fish_vector[k].length[j];
		}
	else
		{
		mean_width+= my_fish_vector[k].length[j];
		mean_length+=my_fish_vector[k].width[j];		
		}
}
mean_width_v.push_back(calibration_factor * mean_width/my_fish_vector[k].x_pos.size());
mean_length_v.push_back(calibration_factor * mean_length/my_fish_vector[k].x_pos.size());
	}
// Save fish measurements to file
string out_filename = "fish_output.txt";
ofstream output (out_filename.c_str());
if (output.is_open()) 
{
	output <<  "mean_width (cm)" << ", " << "mean_length (cm)" << ", " << "weight (gram)" << endl;	
	for(int k=0; k<mean_width_v.size();k++)
	{
		//output << to_string(my_fish_frame[k].width) << "," << to_string(my_fish_frame[k].length) << endl;
		output <<  mean_width_v[k] << ", " << mean_length_v[k] << ", "<<pow(0.4*mean_length_v[k],2.5)*(0.1)  << endl;		
	}
}

output.close();
cout << "--------------------- Summary of detected fish ---------------------" <<endl;
for (int k=0; k<mean_width_v.size();k++)
{
	cout << "Fish " << k+1 << ": width " << mean_width_v[k] << "cm   ; length  " << mean_length_v[k] << "cm   ; weight  " << pow(0.4*mean_length_v[k],2.5)*(0.1) << "g"<< endl;
}


  return(0);
}

/** @function thresh_callback */
void thresh_callback(int, void*)
{
  my_fish_frame.clear(); // Clear before populating the current frame
  Mat threshold_output;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;

  /// Detect edges using Threshold
  threshold( src_gray, threshold_output, thresh, 255, THRESH_BINARY );
  /// Find contours
  findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );


  /// Find the rotated rectangles and ellipses for each contour
  vector<RotatedRect> minRect( contours.size() );
  vector<RotatedRect> minEllipse( contours.size() );

  for( int i = 0; i < contours.size(); i++ )
     { minRect[i] = minAreaRect( Mat(contours[i]) );
       if( contours[i].size() > 5 )
         { minEllipse[i] = fitEllipse( Mat(contours[i]) ); }
     }
  for( int i = 0; i < contours.size(); i++ )
     { 
	// Instantiate a fish
  	FishClass detected_fish(minRect[i].size.width,minRect[i].size.height,minRect[i].center.x,minRect[i].center.y );
	
	if(detected_fish.is_fish) {
		my_fish_frame.push_back(detected_fish);
	
					}
	else 
		cout << "Nothing to report" << endl;
	
	// cout << Mat(contours[i])  << endl;
	// Print rectangle
	//cout << "Dimensions: " << minRect[i].size.width << " x " << minRect[i].size.height << endl;
	// Location
	//cout << "Loction: (" << minRect[i].center.x << ", " << minRect[i].center.y << ")" << endl;
     }

  /// Draw contours + rotated rects + ellipses
  Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
  for( int i = 0; i< contours.size(); i++ )
     {
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       // contour
       drawContours( drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
       // ellipse
       ellipse( drawing, minEllipse[i], color, 2, 8 );
       // rotated rectangle
       Point2f rect_points[4]; 
       minRect[i].points( rect_points );
       for( int j = 0; j < 4; j++ )
          line( drawing, rect_points[j], rect_points[(j+1)%4], color, 1, 8 );
     }

  /// Show in a window
  namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
  imshow( "Contours", drawing );
}

void update_fish_list() {
cout << "Updating fish list ..."<< endl;

if (my_fish_vector.empty()!=true){
cout << "Fish vector of size " << my_fish_vector.size() <<"; in frame currently "<< my_fish_frame.size() << endl;
vector< vector<double> > matrix(my_fish_vector.size(), vector<double>(my_fish_frame.size(),100000.));
//std::vector<double> matrix(my_fish_vector.size(), std::vector<double>(my_fish_frame.size(),100.0));
//std::vector<double> vector1;
//std::vector<double> vector2;
for (int i = 0; i< my_fish_vector.size(); i++)
    { 
      for (int j = 0; j < my_fish_frame.size(); j++)
      {
	cout << "In loop..." << my_fish_frame[j].y_pos[0] << "   " <<my_fish_vector[i].y_pos.back()<< endl;
	if (my_fish_frame[j].y_pos[0] >=my_fish_vector[i].y_pos.back()) 
	   {	
	    matrix[i][j] = pow(my_fish_frame[j].y_pos[0] - my_fish_vector[i].y_pos.back(), 2) + pow(my_fish_frame[j].x_pos[0] - my_fish_vector[i].x_pos.back(), 2);
	    cout << i<<"00000000000000000000000000000000"<<j<<" "<< sqrt(matrix[i][j])<< endl;
	   }
	  }
     } 
for (int i = 0; i< my_fish_vector.size(); i++) { // identify the same fish

	//int min = *min_element(matrix[i].begin(), matrix[i].end());

	int min = distance(matrix[i].begin(), min_element(matrix[i].begin(), matrix[i].end()));

	//min = *matrix[i].begin() - min;

	if (my_fish_frame[min].y_pos[0] >= my_fish_vector[i].y_pos.back()) {

		my_fish_vector[i].y_pos.push_back(my_fish_frame[min].y_pos[0]);
		my_fish_vector[i].x_pos.push_back(my_fish_frame[min].x_pos[0]);
		my_fish_vector[i].length.push_back(my_fish_frame[min].length[0]);
		my_fish_vector[i].width.push_back(my_fish_frame[min].width[0]);

		my_fish_frame.erase(my_fish_frame.begin()+min);

	}

	}
for (int j = 0; j< my_fish_frame.size(); j++) { // push the remaining fish
	  my_fish_vector.push_back(my_fish_frame[j]);	
	  }
}

else{for (int j = 0; j< my_fish_frame.size(); j++) {

	  my_fish_vector.push_back(my_fish_frame[j]);	
	  }

     }
}
