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
	if (q != NULL){
		if (q->children[0] != NULL){
			for (int i = 0; i < 4; ++i) {
				saveQuad(im, q->children[i]);
			}
		} else {
			for (int i = q->ty; i < q->ty + q->width; i++){
				for (int j = q->tx; j < q->tx + q->width; j++){
					im->data[(i * im->sx) + j] = q->grayscale;
				}
			}
		}
	}
}

/* Calculates the sum, max, and min of the grayscales 
 * of the corresponding sub-matrices
 */
void findGrayscaleValues(Image *im, int w, int tx, int ty, int *grayscaleDifference, double *newGrayscale){
	int max = 0;
	int min = 255;
	
	for (int i = ty; i < ty + w; i++){
		for (int j = tx; j < tx + w; j++){
			if (im->data[(i * im->sx) + j] >= max){
				max = im->data[(i * im->sx) + j];
			}  else if (im->data[(i * im->sx) + j] <= min){
				min = im->data[(i * im->sx) + j];
			}
			*newGrayscale += im->data[(i * im->sx) + j];
		}
	}
	
	*grayscaleDifference = max - min;
	*newGrayscale = lrint(*newGrayscale / (w * w));
}

/* Build quadtree from image.  Splitting stops when maxDepth hits 0 or maximum
 * difference in grayscale is <= threshold.  Do this to only the square region
 * at top-left corner (tx, ty) of width w.  Use heap allocation for all Quad
 * nodes you produce.
 */
Quad *formQuadtree(Image *im, int maxDepth, int threshold, int tx, int ty, int w)
{
	int grayscaleDifference;
	double newGrayscale = 0.0;
	
	// Allocates memory for a quad node and sets its tx, ty, and width values
	Quad *root = (Quad *)calloc(1, sizeof(Quad));
	
	if (root != NULL){
		root->tx = tx;
		root->ty = ty;
		root->width = w;
		
		findGrayscaleValues(im, w, tx, ty, &grayscaleDifference, &newGrayscale);
		
		// Splits the image if the node has children, which is determined by 
		// maxDepth and (max - min)
		if (maxDepth != 0 && grayscaleDifference > threshold){
			root->children[0] = formQuadtree(im, maxDepth - 1, threshold, tx, ty, w/2);
			root->children[1] = formQuadtree(im, maxDepth - 1, threshold, tx + (w/2), ty, w/2);
			root->children[2] = formQuadtree(im, maxDepth - 1, threshold, tx, ty + (w/2), w/2);
			root->children[3] = formQuadtree(im, maxDepth - 1, threshold, tx + (w/2), ty + (w/2), w/2);
		} else {
			root->grayscale = newGrayscale;
		}
	}
	
	return root;
}

/* Deallocate all Quad nodes of the given quadtree. Assume that they all came
 * from the heap.
 */
void freeQuadtree(Quad *q)
{
	if (q != NULL){
		for (int i = 0; i < 4; ++i) {
			freeQuadtree(q->children[i]);
		}
		
		free(q);
	}
}
