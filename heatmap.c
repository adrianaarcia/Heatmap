#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#include "track.h"
#include "trackpoint.h"
#include "location.h"

void heatmap_destroy(int*** map, int rows);

int main(int argc, char* argv[])
{
    
    double cell_width, cell_height;
    char* chars;
    int chars_len, range;
    
    //Step 1: check to make sure arguments are valid
    if(argc != 5 || atof(argv[1]) <= 0 || atof(argv[2]) <= 0 || strlen(argv[3]) <= 0 || atoi(argv[4]) <= 0)
    {
        fprintf(stderr, "ERROR: invalid command line arguments\n");
        return 1;
    }
    else
    {
        cell_width = atof(argv[1]);
        cell_height = atof(argv[2]);
        chars = argv[3];
        chars_len = strlen(chars);
        range = atoi(argv[4]);
    }
    

    //Step 2: Create track
    track* trk = track_create();
    
    if (trk == NULL)
    {
        fprintf(stderr, "ERROR: could not create track\n");
        track_destroy(trk);
        return 2;
    }

    //Step 3: read in from standard input and add to track, account for whitespace/newlines
    double lat, lon;
    long timestamp;
    trackpoint* point = NULL;

    while (scanf("%lf %lf %li", &lat, &lon, &timestamp) != EOF)
    {
        
        point = trackpoint_create(lat, lon, timestamp);
        if(point != NULL)
        {
            if (!(track_add_point(trk, point)))
            {
                fprintf(stderr, "ERROR: could not add trackpoint\n");
                trackpoint_destroy(point);
                track_destroy(trk);
                return 3;
            }
        }
        else
        {
            fprintf(stderr, "ERROR: could not create trackpoint\n");
            trackpoint_destroy(point);
            track_destroy(trk);
            return 4;
        }    
        trackpoint_destroy(point);
    }

    //Step 4: create heatmap
    int** map = NULL;
    int map_rows;
    int map_cols;
    
    track_heatmap(trk, cell_width, cell_height, &map, &map_rows, &map_cols);

    if(map == NULL)
    {
        fprintf(stderr, "ERROR: could not create heatmap\n");
        track_destroy(trk);
        return 5;
    }

    //Step 5: print out heatmap 
    int index;
    for(int i = 0; i < map_rows; i++)
    {
        for(int j = 0; j < map_cols; j++)
        {
            index = ceil((map[i][j])/range);
            if (index < chars_len)
            {
                printf("%c",chars[index]);
            }
            else if (index >= chars_len)
            {
                printf("%c", chars[chars_len - 1]);
            }
        }
        printf("\n");  
    }
    //Step 6: free memory
    heatmap_destroy(&map, map_rows);	
    track_destroy(trk); 
}

void heatmap_destroy(int*** map, int rows)
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
