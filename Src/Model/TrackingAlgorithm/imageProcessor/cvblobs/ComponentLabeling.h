#if !defined(_COMPONENT_LABELING_H_INCLUDED)
#define _CLASSE_BLOBRESULT_INCLUDED

#include "vector"
#include "BlobContour.h"
#include "blob.h"


//! definici� de que es un vector de blobs
typedef std::vector<CBlob*>	Blob_vector;



bool ComponentLabeling(cv::Mat inputImage,
						cv::Mat* maskImage,
						unsigned char backgroundColor,
						Blob_vector &blobs );


void contourTracing( cv::Mat image, cv::Mat* mask, cv::Point contourStart, t_labelType *labels, 
					 bool *visitedPoints, t_labelType label,
					 bool internalContour, unsigned char backgroundColor,
					 CBlobContour *currentBlobContour );

cv::Point tracer( cv::Mat image, cv::Mat* mask, cv::Point P, bool *visitedPoints,
				short initialMovement,
				unsigned char backgroundColor, short &movement );
				

#endif	//!_CLASSE_BLOBRESULT_INCLUDED
