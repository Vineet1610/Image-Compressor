#include <stdlib.h>
#include <math.h>                   /* for lrint */
#include "quad.h"
#include "pgmio.h"

/* Reconstruct image from quadtree.  Write into the Image record that im points
 * to.  Assume that the square region described by the quadtree fits into the
 * Image.
 *
 * Recall that each Quad node already contains position information; respect it,
 * and you will benefit from it too (a secret for successful recursions).
 */
void saveQuad(Image *im, Quad *q)
{
	if(q != NULL)
	{
		if(q->children[0] == NULL && q->children[1] == NULL && q->children[2] == NULL && q->children[3] == NULL)
		{
			if(q->width == 1)
			{
				*(im->data + (q->tx * im->sx) + q->ty) = q->grayscale;
			}
			else
			{
				for(int i = 0; i < q->width; i++)
				{
					for(int j = 0; j < q->width; j++)
					{
						*(im->data + ((q->tx + i) * im->sx) + (q->ty + j)) = q->grayscale;
					}
				}
			}
		}
		else
		{
			for(int i = 0; i < 4; i++)
			{
				saveQuad(im, q->children[i]);
			}
		}
	}
}

/* Build quadtree from image.  Splitting stops when maxDepth hits 0 or maximum
 * difference in grayscale is <= threshold.  Do this to only the square region
 * at top-left corner (tx, ty) of width w.  Use heap allocation for all Quad
 * nodes you produce.
 */

/* The function below returns the difference between the maximum and minimum value
   of that particular w x w matrix. This helps in the comparison of threshold
   in the formQuadtree function
*/
int max_difference(Image *im, int w, int x, int y)
{
	int max = -1;
	int min = 255;
	if(w > 1)
	{
		for(int i = 0; i < w; i++)
		{
			for(int j = 0; j < w; j++)
			{
				int grayscale = *(im->data + ((x + i) * im->sx) + (y + j));
				if(grayscale >= max)
				{
					max = grayscale;
				}
				if(grayscale <= min)
				{
					min = grayscale;
				}
			}
		}
		return (max - min);
	}
	else
	{
		return 0; // Width = 1
	}
}

/* The function below calculates the average using the values inside the w x w
   matrix which helps in setting the value of grayscale in formQuadtree function
*/
int average_grayscale(Image *im, int w, int x, int y)
{
	int sum = 0;
	double avg = 0.0;
	int total_numbers = w * w; // Total number of values inside the matrix w x w
	if(w > 1)
	{
		for(int i = 0; i < w; i++)
		{
			for(int j = 0; j < w; j++)
			{
				sum = sum + *(im->data + ((x + i) * im->sx) + (y + j));
			}
		}

		avg = sum / ((double) total_numbers);
		return lrint(avg);
	}
	else
	{
		return *(im->data + (x * im->sx) + y); // Width = 1
	}
}

Quad *formQuadtree(Image *im, int maxDepth, int threshold, int tx, int ty, int w)
{
	Quad *q = NULL; // New quad tree Node
	int diff_grayscale = max_difference(im, w, tx, ty);
	if(maxDepth > 0 && diff_grayscale > threshold && w > 1)
	{
		q = (Quad *)calloc(1, sizeof(Quad));

		if(q != NULL)
		{
			q->tx = tx;
			q->ty = ty;
			q->width = w;
			q->children[0] = formQuadtree(im, maxDepth - 1, threshold, q->tx, q->ty, w/2);
			q->children[1] = formQuadtree(im, maxDepth - 1, threshold, q->tx + (w/2), q->ty, w/2);
			q->children[2] = formQuadtree(im, maxDepth - 1, threshold, q->tx, q->ty + (w/2), w/2);
			q->children[3] = formQuadtree(im, maxDepth - 1, threshold, q->tx + (w/2), q->ty + (w/2), w/2);
			return q;
		}
	}
	else
	{
		q = (Quad *)calloc(1, sizeof(Quad));

		if(q != NULL)
		{
			q->tx = tx;
			q->ty = ty;
			q->width = w;
			q->grayscale = average_grayscale(im, w, tx, ty);
			q->children[0] = NULL;
			q->children[1] = NULL;
			q->children[2] = NULL;
			q->children[3] = NULL;
			return q;
		}
	}
	//return 0;
}

/* Deallocate all Quad nodes of the given quadtree. Assume that they all came
 * from the heap.
 */
void freeQuadtree(Quad *q)
{
	if(q->children[0] == NULL && q->children[1] == NULL && q->children[2] == NULL && q->children[3] == NULL)
	{
		free(q);
	}
	else
	{
		for(int i = 0; i < 4; i++)
        {
			freeQuadtree(q->children[i]);
        }
		free(q);
	}
}
