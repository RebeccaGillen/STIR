//
// $Id$
//
/*!
  \file 
  \ingroup utilities

  \brief Implementation of class TimeFrameDefinitions
 
  \author Kris Thielemans
  
  $Date$
  $Revision$
*/
/*
    Copyright (C) 2003- $Date$, Hammersmith Imanet Ltd
    See STIR/LICENSE.txt for details
*/


#include "stir/TimeFrameDefinitions.h"
#include <iostream>
#include <fstream>

#ifndef STIR_NO_NAMESPACES
using std::make_pair;
using std::cerr;
using std::endl;
#endif

START_NAMESPACE_STIR


double
TimeFrameDefinitions::
get_start_time(unsigned int frame_num) const
{
  assert(frame_num>=1);
  assert(frame_num<=get_num_frames());
  return frame_times[frame_num-1].first;
}

double
TimeFrameDefinitions::
get_end_time(unsigned int frame_num) const
{
  assert(frame_num>=1);
  assert(frame_num<=get_num_frames());
  return frame_times[frame_num-1].second;
}

double
TimeFrameDefinitions::
get_duration(unsigned int frame_num) const
{
  return get_end_time(frame_num) - get_start_time(frame_num);
}

double
TimeFrameDefinitions::
get_start_time() const
{
  return get_start_time(1);
}

double
TimeFrameDefinitions::
get_end_time() const
{
  return get_end_time(get_num_frames());
}

unsigned int
TimeFrameDefinitions::
get_num_frames() const
{
  return frame_times.size();
}

TimeFrameDefinitions::
TimeFrameDefinitions()
{}

TimeFrameDefinitions::
TimeFrameDefinitions(const string& fdef_filename)
{
  ifstream in(fdef_filename.c_str());
  if (!in)
    error("Error reading %s\n", fdef_filename.c_str());

  
  double previous_end_time = 0;
  while (true)
  {
    int num;
    double duration;
    in >> num >> duration;
    if (!in)
      break;
    if (num<0 || duration<=0)
        error("Reading frame_def file %s: encountered negative numbers (%d, %g)\n",
	      fdef_filename.c_str(), num, duration);

    if (num==0)
      {
	// special case to allow us to skip a time period without storing it
	previous_end_time+=duration;
      }
    while (num--)
    {
      frame_times.push_back(make_pair(previous_end_time, previous_end_time+duration));
      previous_end_time+=duration;
    }
  }
#if 0
  cerr << "Frame definitions:\n{";
  for (unsigned frame_num=1; frame_num<=get_num_frames(); ++frame_num)
  {
    cerr << '{' << get_start_time(frame_num) 
         << ',' << get_end_time(frame_num) 
         << '}';
    if (frame_num<get_num_frames())
      cerr << ',';
  }
  cerr << '}' << endl;
#endif
}

TimeFrameDefinitions::
TimeFrameDefinitions(const vector<pair<double, double> >& frame_times)
  : frame_times(frame_times)
{
  if (get_num_frames()==0)
    return;

  // check times are in sequence
  double current_time = get_start_time(1);
  for (unsigned int current_frame = 1; current_frame != get_num_frames(); ++ current_frame)
    {
      if (current_time > get_start_time(current_frame))
	error("TimeFrameDefinitions: frame number %d start_time (%g) is smaller than "
	      "previous end_time (%g)\n",
	      current_frame, get_start_time(current_frame), current_time);
      if (get_start_time(current_frame) > get_end_time(current_frame))
	error("TimeFrameDefinitions: frame number %d start_time (%g) is larger than "
	      "end_time (%g)\n",
	      current_frame, get_start_time(current_frame), get_end_time(current_frame));
      current_time = get_end_time(current_frame);
    }
}

END_NAMESPACE_STIR
