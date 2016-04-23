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

using namespace cv;
using namespace std;

Mat src; Mat src_gray;
int thresh = 103;
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
  
for(int frame_idx =18; frame_idx < 22; frame_idx++)
  {
  int idx = frame_idx;
  ostringstream s;
  s << idx;
  string str_filename = "stream1fish/videoframe" + s.str() + ".jpg";
  string out_filename = "output" + s.str() + ".txt";
  char filename[] = "stream1fish/videoframe15.jpg";	
  /// Load source image and convert it to gray
  src = imread( str_filename.c_str(), 1 );

  /// Convert image to gray and blur it
  cvtColor( src, src_gray, CV_BGR2GRAY );
  blur( src_gray, src_gray, Size(3,3) );

  /// Create Window
  char* source_window = "Source";
  namedWindow( source_window, CV_WINDOW_AUTOSIZE );
  imshow( source_window, src );

  //createTrackbar( " Threshold:", "Source", &thresh, max_thresh, thresh_callback );
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
		cout << "fish added" << endl;		
					}
	else 
		cout << "NOTHING TO REPORT" << endl;
	
	// cout << Mat(contours[i])  << endl;
	// Print rectangle
	cout << "Dimensions: " << minRect[i].size.width << " x " << minRect[i].size.height << endl;
	// Location
	cout << "Loction: (" << minRect[i].center.x << ", " << minRect[i].center.y << ")" << endl;
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
vector< vector<double> > matrix(my_fish_vector.size(), vector<double>(my_fish_frame.size()));
//std::vector<double> matrix(my_fish_vector.size(), std::vector<double>(my_fish_frame.size(),0.0));
//std::vector<double> vector1;
//std::vector<double> vector2;
for (int i = 0; i< my_fish_vector.size(); i++)
    { 
      for (int j = 0; j < my_fish_frame.size(); j++)
      {
	cout << "In loop..." << my_fish_frame[j].y_pos[0] << "   " <<my_fish_vector[i].y_pos.back()<< endl;
	if (my_fish_frame[j].y_pos[0] <=my_fish_vector[i].y_pos.back()) 
	   {
	    matrix[i][j] = 2.0;
	    cout << i<<" "<<j<<" "<< matrix[i][j];
	}
	}
     } 

}
else{for (int j = 0; j< my_fish_frame.size(); j++) {
	cout << "Here2" << endl;
	my_fish_vector.push_back(my_fish_frame[j]);	
	}

     }
}
