#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

#include "track.h"

typedef struct segment
{
	trackpoint** trkpts;
	int size;
	double length;
	int trkpt_cap;
} segment;

struct track
{
	segment* segments;
	int seg_count;
	int seg_cap;
	int total_trkpts;
};

void map_destroy(int*** map, int rows);

track *track_create()
{
	track *trk = malloc(sizeof(track));

	if(trk != NULL)
	{
		trk->segments = malloc(sizeof(segment));

		if(trk->segments != NULL) //initialize values
		{
			trk->seg_cap = 1;
			trk->seg_count = 1;
			trk->total_trkpts = 0;

			trk->segments->length = 0;
			trk->segments->size = 0;
			
			//don't allocate memory for trkpts yet -- will handle this in track_add_point
			trk->segments->trkpts = NULL;
			trk->segments->trkpt_cap = 0;

		}
		
		else
		{
			free(trk);
			return NULL;
		}
		
	}	
	
	return trk;
}

void track_destroy(track *trk)
{
	if(trk != NULL)
	{
		for (int i = 0; i < trk->seg_count; i++)
		{
			if (trk->segments[i].trkpts != NULL) 
			{
				for(int j = 0; j < trk->segments[i].size; j++) //free each trackpoint pointer
				{
					if(trk->segments[i].trkpts[j] != NULL)
					{
						free(trk->segments[i].trkpts[j]);
					}
				}

				free(trk->segments[i].trkpts); //free pointer to array of trackpoint pointers
			}
		}
		free(trk->segments); //free pointer to array of segments
	}
	free(trk); //free pointer to track
}


int track_count_segments(const track *trk)
{
	if(trk != NULL)
	{
		return trk->seg_count;
	}
	else
	{
		return 0; //return 0 if track is NULL
	}
}
 
int track_count_points(const track *trk, int i)
{
	if(trk != NULL && i >= 0 && i < trk->seg_count)
	{
		return trk->segments[i].size;
	}
	else
	{
		return 0; //return 0 if track is NULL or if index is invalid
	}
}

trackpoint *track_get_point(const track *trk, int i, int j) //removed error checking and allocation because i think trackpoint_copy takes care of it?
{
	if(trk != NULL && i >= 0 && j >= 0 && i < trk->seg_count && j < trk->segments[i].size)
	{
		return trackpoint_copy(trk->segments[i].trkpts[j]);
	}
	else
	{
		return NULL;
	}
}

double *track_get_lengths(const track *trk) //revisit the warning
{
	if (trk != NULL)
	{
		double* lengths = malloc(sizeof(double) * trk->seg_count);

		for(int i = 0; i < trk->seg_count; i++)
		{
			lengths[i] = trk->segments[i].length;
		}
		return lengths;
	}

	else return NULL;
}

bool track_add_point(track *trk, const trackpoint *pt)
{	
	if(trk != NULL && pt != NULL)
	{
		segment* curr_seg = &trk->segments[trk->seg_count - 1]; //pointer to current segment
		
		long compare = LONG_MAX; //timestamp to compare pt's time to
		
		if (curr_seg->size > 0) //already a trackpoint in segment
		{
			compare = trackpoint_time(curr_seg->trkpts[curr_seg->size - 1]);
		}
		else //empty segment, have to allocate memory for trkpts
		{
			curr_seg->trkpts = malloc(sizeof(trackpoint*));

			if(curr_seg->trkpts != NULL)
			{
				curr_seg->trkpt_cap = 1; //++?

				if (trk->seg_count > 1)
				{
					segment* prev_seg = &trk->segments[trk->seg_count - 2]; //temporary pointer to previous segment
					
					compare = trackpoint_time(prev_seg->trkpts[prev_seg->size - 1]); //compare to last trkpt in prev_seg
				}
				else compare = LONG_MIN; //case: very first trackpoint being added
			}
			else return false;
		}
		
		//add point if timestamp is valid
		if(trackpoint_time(pt) > compare || compare == LONG_MIN) //need to account for the case that first trkpt added is equal to LONG_MIN
		{
			
			if(!(curr_seg->trkpt_cap > curr_seg->size)) //reallocate memory if there won't be enough space to add a pt
			{
				curr_seg->trkpts = realloc(curr_seg->trkpts, (sizeof(trackpoint*) * (curr_seg->trkpt_cap * 2)));//reallocate by factor of two
				
				if (curr_seg->trkpts != NULL)
				{
					curr_seg->trkpt_cap = curr_seg->trkpt_cap * 2;
				} 
				else return false;
			}		
			
			trackpoint* copy = trackpoint_copy(pt);

			if (copy != NULL)//copy pt to end of trkpts
			{
				curr_seg->trkpts[curr_seg->size] = copy;
				curr_seg->size++;
				trk->total_trkpts++;

				if(curr_seg->size > 1)//increment length of segment
				{
					location loc_1 = trackpoint_location(curr_seg->trkpts[curr_seg->size - 2]);
					location loc_2 = trackpoint_location(curr_seg->trkpts[curr_seg->size - 1]);

					curr_seg->length += location_distance(&loc_1, &loc_2);

				}				
				return true;
			}
		}
	
	}

	return false;
}

void track_start_segment(track *trk)
{
	if (trk != NULL && trk->segments[trk->seg_count - 1].size > 0) //make sure that there is not already an empty track and that trk is not null
	{
		if(!(trk->seg_cap > trk->seg_count))
		{
			trk->segments = realloc(trk->segments, (sizeof(segment) * (trk->seg_cap * 2)));
			if (trk->segments != NULL)
			{
				trk->seg_cap = trk->seg_cap * 2;
			}
		}
		
		segment* new_seg = &trk->segments[trk->seg_count]; //temporary pointer to new segment, initialize values

		new_seg->size = 0;
		new_seg->length = 0;
		new_seg->trkpts = NULL;
		new_seg->trkpt_cap = 0;
		
		trk->seg_count++;
	}
}

void track_merge_segments(track *trk, int start, int end)
{
	if(trk!= NULL && start >= 0 && end >= start && start < trk->seg_count && end <= trk->seg_count)
	{
		int shift = end - start - 1;
		int new_count =  trk->seg_count - shift;

		segment* new = malloc(sizeof(segment) * new_count);

		if(new != NULL)
		{
			for(int i = 0; i < start; i++) //pre-merge segments stay the same
			{
				new[i].trkpt_cap = trk->segments[i].trkpt_cap;
				new[i].size = trk->segments[i].size;
				new[i].length = trk->segments[i].length;

				new[i].trkpts = trk->segments[i].trkpts;

				trk->segments[i].trkpts = NULL;
			}
			
			new[start].trkpts = malloc(sizeof(trackpoint*));
			new[start].trkpt_cap = 1;
			new[start].size = 0;
			
			int new_trkpt_index = 0;

			for(int i = start; i < end; i++) //merge range of segments
			{
				segment* orig_seg = &trk->segments[i];
				if(!(new[start].trkpt_cap > (new[start].size + orig_seg->size))) //reallocate memory if needed
				{
					new[start].trkpts = realloc(new[start].trkpts, sizeof(trackpoint*) * 2 * (new[start].size + orig_seg->size));
					if (new[start].trkpts != NULL)
					{
						new[start].trkpt_cap = 2 * (new[start].size + orig_seg->size);
					}
					else return;
				}

				for(int j = 0; j < orig_seg->size; j++) //add trkpts to new[start] 
				{
					new[start].trkpts[new_trkpt_index + j] = trackpoint_copy(orig_seg->trkpts[j]);

					free(orig_seg->trkpts[j]);
				}
				free(orig_seg->trkpts);

				new[start].size += orig_seg->size;	

				if(new_trkpt_index > 0) //calculate lengths
				{
					location loc1 = trackpoint_location(new[start].trkpts[new_trkpt_index - 1]);
					location loc2 = trackpoint_location(new[start].trkpts[new_trkpt_index]);
					new[start].length += location_distance(&loc1, &loc2);
				}
				
				new[start].length += orig_seg->length;
				new_trkpt_index += orig_seg->size;
			}

			for(int i = end; i < trk->seg_count; i++) //post-merge segments stay the same
			{
				new[i - shift].size = trk->segments[i].size;
				new[i - shift].length = trk->segments[i].length;
				new[i - shift].trkpt_cap = trk->segments[i].trkpt_cap;

				new[i - shift].trkpts = trk->segments[i].trkpts;
				
				trk->segments[i].trkpts = NULL; 
			}

			free(trk->segments);
			trk->segments = new;
		}

		trk->seg_cap = new_count;
		trk->seg_count = new_count;
	}
}

void track_heatmap(const track *trk, double cell_width, double cell_height, int ***map, int *rows, int *cols)
{
	if(trk != NULL&& cell_width > 0 && cell_width <= 360 && cell_height > 0 && cell_height <= 180)
	{
		//Step 1: Check if track has more than one point
		if(trk->total_trkpts <= 1)
		{
			if((map[0] = malloc(sizeof(int*))) != NULL && (map[0][0] = malloc(sizeof(int))) != NULL)
			{
				*rows = 1;
				*cols = 1;
				if(trk->total_trkpts == 1)
				{
					map[0][0][0] = 1;
					return;
				}
				else
				{
					map[0][0][0] = 0;
					return;
				}
				
			}
			else
			{
				free(map[0][0]);
				free(map[0]);
				map = NULL;
				return;
			}
		}
		
		*rows = 0;
		*cols = 0;

		//Step 2: find boundaries
		double north_lat = -91;
		double south_lat = 91;
		double curr_lat, longtd1, longtd2, long_dist, dist_to_beat, smallest_wedge;
		int shift, westernmost_index, west_index, row_num, col_num;
		trackpoint** points;

		smallest_wedge = 361;

		//Step 3: make an array of all trackpoints
		if((points = malloc(sizeof(trackpoint*) * trk->total_trkpts)) != NULL)
		{
			shift = 0;

			for(int i = 0; i < trk->seg_count; i++)
			{
				for(int j = 0; j < trk->segments[i].size; j++)
				{
					points[j+shift] = trackpoint_copy(trk->segments[i].trkpts[j]);
				}

				shift = trk->segments[i].size;
			}
		}
		else
		{
			map = NULL;
			free(points);
			return;
		}

		//Step 4: find boundary points
		for(int i = 0; i < trk->total_trkpts; i++)
		{
			curr_lat = trackpoint_location(points[i]).lat;
			if (curr_lat > north_lat)
			{
				north_lat = curr_lat;
			}
			else if(curr_lat < south_lat)
			{
				south_lat = curr_lat;
			}

			longtd1 = trackpoint_location(points[i]).lon;
				
			dist_to_beat = 0;
				
			west_index = 0;
			
			for(int j = 0; j < trk->total_trkpts; j++) //find longest longitudal distance bw point i and any point j in points
			{
				longtd2 = trackpoint_location(points[j]).lon;

				long_dist = longtd2 - longtd1;

				if(long_dist > 0 && long_dist > dist_to_beat)
				{
					dist_to_beat = long_dist;

					west_index = i;
				}
				else if (long_dist < 0 && long_dist + 360 > dist_to_beat)
				{
					dist_to_beat = long_dist + 360;

					west_index = i;
				}
			}
				
			if(dist_to_beat < smallest_wedge) //find shortest of calculated wedges
			{
				smallest_wedge = dist_to_beat;
				westernmost_index = west_index;
			}
		}

		//Step 5: Figure out dimensions of heatmap

		row_num = ceil((north_lat - south_lat) /cell_height);
		col_num = ceil(smallest_wedge/cell_width);

		//Step 6: Initialize heatmap
		if((map[0] = malloc(sizeof(int*) * row_num) ) != NULL)
		{
			*rows = row_num;
			for(int i = 0; i < *rows; i++)
			{
				if((map[0][i] = calloc(col_num, sizeof(int))) != NULL)
				{	
					for(int j = 0; j < *cols; j++)
					{
						map[0][i][j] = 0;
					}
				}
				else
				{
					map_destroy(map, i);

					for(int i = 0; i < trk->total_trkpts; i++)
					{
						free(points[i]);
					}
						
					free(points);
					return;
				}
			}
				
			*cols = col_num;
		}
		else
		{
			if(map[0] != NULL)
			{
				free(map[0]);
				map = NULL;
			}
			for(int i = 0; i < trk->total_trkpts; i++)
			{
				free(points[i]);
			}
				
			free(points);
			return;
		}

		//Step 7: Populate heatmap
		double left_lon_bound = trackpoint_location(points[westernmost_index]).lon;
		location p_loc;
		int r, c;

		for(int i = 0; i < trk->total_trkpts; i++)
		{
			p_loc = trackpoint_location(points[i]);

			r = floor((north_lat - p_loc.lat)/cell_height);
			c = floor((p_loc.lon - left_lon_bound)/cell_width);

			if(r >= *rows) //deal with cases when points are on edge
			{
				r = *rows - 1;
			}
			if(c >= *cols)
			{
				c = *cols - 1;
			}

			map[0][r][c]++;
		}

		//Step 8: Free memory we no longer need
		for(int i = 0; i < trk->total_trkpts; i++)
		{
			free(points[i]);
		}
				
		free(points);
		return;
		
	}
	else 
	{
		map = NULL;
	}
}

void map_destroy(int*** map, int rows)
{
	for (int r = 0; r < rows; r++)
    {
		if(map[0][r]!=NULL)
		{
      		free(map[0][r]);
    	}	
	}
  	
	free(map[0]);
	map = NULL;
}