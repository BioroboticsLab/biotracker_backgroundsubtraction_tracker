
#include "ComponentLabeling.h"

#include <thread>
#include <mutex>
#include <future>

//! Conversion from freeman code to coordinate increments (counterclockwise)
static const cv::Point freemanCodeIncrement[8] =
    {{1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}};

inline unsigned char GET_IMAGE_PIXEL(cv::Mat image, cv::Point p)
{
    return image.at<unsigned char>(p);
}

inline bool GET_IMAGEMASK_PIXEL(cv::Mat* mask, cv::Point p)
{
    if (mask)
        return mask->at<unsigned char>(p) > 0;
    else
        return true;
}

inline bool GET_BELOW_VISITEDPIXEL(bool* currentPixel, int imageWidth)
{
    return *(currentPixel + imageWidth);
}

/**
- FUNCI�: ASSIGN_LABEL
- FUNCIONALITAT: Assigns label value to label image
- PAR�METRES:
    -
- RESULTAT:
    -
- RESTRICCIONS:
    -
- AUTOR: rborras
- DATA DE CREACI�: 2008/04/29
- MODIFICACI�: Data. Autor. Descripci�.
*/
inline void ASSIGN_LABEL(cv::Point    p,
                         t_labelType* labels,
                         int          imageWidth,
                         int          newLabel)
{
    *(labels + p.y * imageWidth + p.x) = newLabel;
}

inline void ASSIGN_VISITED(cv::Point p, bool* visitedPoints, int imageWidth)
{
    *(visitedPoints + p.y * imageWidth + p.x) = true;
}

/**
- FUNCI�: ComponentLabeling
- FUNCIONALITAT: Calcula els components binaris (blobs) d'una imatge amb
connectivitat a 8
- PAR�METRES:
    - inputImage: image to segment (pixel values different than blobColor are
treated as background)
    - maskImage: if not NULL, all the pixels equal to 0 in mask are skipped in
input image
    - backgroundColor: color of background (ignored pixels)
    - blobs: blob vector destination
- RESULTAT:
    -
- RESTRICCIONS:
    -
- AUTOR: rborras
- DATA DE CREACI�: 2008/04/21
- MODIFICACI�: Data. Autor. Descripci�.
- NOTA: Algorithm based on "A linear-time component labeling algorithm using
contour tracing technique", F.Chang et al
*/
bool ComponentLabeling(cv::Mat       inputImage,
                       cv::Mat*      maskImage,
                       unsigned char backgroundColor,
                       Blob_vector&  blobs)
{
    // row major vector with visited points
    bool internalContour, externalContour;
    int  imageWidth, imageHeight, currentLabel, contourLabel;
    //! current blob pointer
    CBlob*    currentBlob;
    cv::Size  imageSizes;
    cv::Point currentPoint;

    // verify input image
    if (!inputImage.empty())
        return false;

    // verify that input image and mask image has same size
    if (maskImage) {
        if (inputImage.size() != maskImage->size())
            return false;
    }

    imageSizes = inputImage.size();

    imageWidth  = imageSizes.width;
    imageHeight = imageSizes.height;

    // create auxiliary buffers that are initialized to 0
    t_labelType* labelledImage = (t_labelType*) calloc(imageWidth *
                                                           imageHeight,
                                                       sizeof(t_labelType));
    bool*        visitedPoints = (bool*) calloc(imageWidth * imageHeight,
                                         sizeof(bool));

    // Execute the POI detection parallel.
    // This basically just filters out the background (which is the majority of
    // points). The result will be an ordered list of non-background points for
    // the blob-detector to work on.
    std::vector<cv::Point2d> foregroundPoints;

    const int totalRegionsX = 4;
    const int totalRegionsY = 4;
    const int totalRegions  = totalRegionsX * totalRegionsY;
    const int regionWidth   = imageWidth / totalRegionsX;
    const int regionHeight  = imageHeight / totalRegionsY;

    // get the pixel data from an image at a certain position
    auto getImageData = [](cv::Mat image, const int row, const int column) {
        return image.data + row * image.step + column;
    };

    // to allow each thread to acces the main list directly (saves us some
    // merging later)
    std::mutex foregroundPointAccessMutex;

    auto workOnRegion = [&](int x, int y) {
        const int startingX = x * regionWidth;
        const int startingY = y * regionHeight;

        for (int j = 0; j < regionHeight; ++j)
            for (int i = 0; i < regionWidth; ++i) {
                const int pointX = startingX + i;
                const int pointY = startingY + j;
                // ignore background pixels or 0 pixels in mask
                const unsigned char* pInputImage = getImageData(inputImage,
                                                                pointY,
                                                                pointX);
                if ((*pInputImage == backgroundColor) ||
                    (maskImage &&
                     *getImageData(*maskImage, pointY, pointX) == 0)) {
                    continue;
                } else // remember all non-background pixels
                {
                    std::lock_guard<std::mutex> lock(
                        foregroundPointAccessMutex);
                    foregroundPoints.emplace_back(pointX, pointY);
                }
            }
    };

    // start one parallel thread for each region
    std::vector<std::future<void>> merger;
    merger.reserve(totalRegions);

    for (int y = 0; y < totalRegionsY; ++y) {
        for (int x = 0; x < totalRegionsX; ++x) {
            merger.push_back(std::async([&, x, y] { workOnRegion(x, y); }));
        }
    }

    // and wait until all have completed; the results are alrady in the main
    // list
    for (const auto& asyncResult : merger) {
        asyncResult.wait();
    }

    // need to sort for coordinate as the loop below is written in a way that
    // depends on the order
    std::sort(foregroundPoints.begin(),
              foregroundPoints.end(),
              [](const cv::Point2d& a, const cv::Point2d& b) -> bool {
                  return (a.y < b.y) || (a.y == b.y && a.x < b.x);
              });

    // initialize pointers and label counter
    currentLabel = 1;

    auto getImageDataVisited = [&](const int& row, const int& column) {
        return visitedPoints + row * inputImage.step + column;
    };

    auto getImageDataLabelled = [&](const int& row, const int& column) {
        return labelledImage + row * inputImage.step + column;
    };

    for (const cv::Point2d& point : foregroundPoints) {
        const int i = point.x;
        const int j = point.y;

        t_labelType* pLabels        = getImageDataLabelled(j, i);
        bool*        pVisitedPoints = getImageDataVisited(j, i);

        // new external contour: current label == 0 and above pixel is
        // background
        if (j > 0) {
            const unsigned char* pAboveInputImage = getImageData(inputImage,
                                                                 j - 1,
                                                                 i);

            externalContour = ((*pAboveInputImage == backgroundColor) ||
                               (maskImage &&
                                *getImageData(*maskImage, j - 1, i) == 0)) &&
                              (*pLabels == 0);
        } else
            externalContour = (*pLabels == 0);

        // new internal contour: below pixel is background and not visited
        if (!externalContour && j < imageHeight - 1) {
            const unsigned char* pBelowInputImage = getImageData(inputImage,
                                                                 j + 1,
                                                                 i);
            internalContour = *pBelowInputImage == backgroundColor &&
                              !GET_BELOW_VISITEDPIXEL(pVisitedPoints,
                                                      imageWidth);
        } else {
            internalContour = false;
        }

        if (externalContour) {
            currentPoint = cv::Point(i, j);
            // assign label to labelled image
            *pLabels = currentLabel;

            // create new blob
            currentBlob = new CBlob(currentLabel, currentPoint, imageSizes);

            // contour tracing with currentLabel
            contourTracing(inputImage,
                           maskImage,
                           currentPoint,
                           labelledImage,
                           visitedPoints,
                           currentLabel,
                           false,
                           backgroundColor,
                           currentBlob->GetExternalContour());

            // add new created blob
            blobs.push_back(currentBlob);

            currentLabel++;
        } else {
            if (internalContour) {
                currentPoint = cv::Point(i, j);

                if (*pLabels == 0) {
                    // take left neightbour value as current
                    if (i > 0)
                        contourLabel = *(pLabels - 1);
                } else {
                    contourLabel = *pLabels;
                }

                if (contourLabel > 0) {
                    currentBlob = blobs[contourLabel - 1];
                    CBlobContour newContour(currentPoint,
                                            currentBlob->GetStorage());

                    // contour tracing with contourLabel
                    contourTracing(inputImage,
                                   maskImage,
                                   currentPoint,
                                   labelledImage,
                                   visitedPoints,
                                   contourLabel,
                                   true,
                                   backgroundColor,
                                   &newContour);

                    currentBlob->AddInternalContour(newContour);
                }
            }
            // neither internal nor external contour
            else {
                // take left neightbour value as current if it is not labelled
                if (i > 0 && *pLabels == 0)
                    *pLabels = *(pLabels - 1);
            }
        }
    }

    // free auxiliary buffers
    free(labelledImage);
    free(visitedPoints);

    return true;
}

/**
- FUNCI�:
- FUNCIONALITAT:
- PAR�METRES:
    -
- RESULTAT:
    -
- RESTRICCIONS:
    -
- AUTOR: rborras
- DATA DE CREACI�: 2008/04/29
- MODIFICACI�: Data. Autor. Descripci�.
*/
void contourTracing(cv::Mat       image,
                    cv::Mat*      maskImage,
                    cv::Point     contourStart,
                    t_labelType*  labels,
                    bool*         visitedPoints,
                    t_labelType   label,
                    bool          internalContour,
                    unsigned char backgroundColor,
                    CBlobContour* currentBlobcontour)
{
    cv::Point t, tnext, tsecond;
    short     initialMovement, movement;

    if (internalContour) {
        initialMovement = 7; // 3;
    } else {
        initialMovement = 3; // 7;
    }

    tsecond = tracer(image,
                     maskImage,
                     contourStart,
                     visitedPoints,
                     initialMovement,
                     backgroundColor,
                     movement);

    // assign current label to tnext
    ASSIGN_LABEL(contourStart, labels, image.size().width, label);

    // contour corresponds to isolated pixel?
    if (tsecond.x == contourStart.x && tsecond.y == contourStart.y) {
        // we are finished with the contour
        return;
    }

    // add chain code to current contour
    currentBlobcontour->AddChainCode(movement);

    // assign label to next point
    ASSIGN_LABEL(tsecond, labels, image.size().width, label);

    tnext.x = tsecond.x;
    tnext.y = tsecond.y;
    t.x     = tnext.x;
    t.y     = tnext.y;

    // while T is different than contourStart and Tnext is different than T
    // follow contour until start point is reached again
    while (!(t.x == contourStart.x && t.y == contourStart.y) ||
           !(tsecond.x == tnext.x && tsecond.y == tnext.y)) {

        t.x             = tnext.x;
        t.y             = tnext.y;
        initialMovement = (movement + 5) % 8;

        // search for next contour point
        tnext = tracer(image,
                       maskImage,
                       t,
                       visitedPoints,
                       initialMovement,
                       backgroundColor,
                       movement);

        // assign label to contour point
        ASSIGN_LABEL(tnext, labels, image.size().width, label);

        // add chain code to current contour
        currentBlobcontour->AddChainCode(movement);
    }
}

/**
- FUNCI�: tracer
- FUNCIONALITAT: Searches for next point of a contour
- PAR�METRES:
    -
- RESULTAT:
    -
- RESTRICCIONS:
    -
- AUTOR: rborras
- DATA DE CREACI�: 2008/04/30
- MODIFICACI�: Data. Autor. Descripci�.
*/
cv::Point tracer(cv::Mat       image,
                 cv::Mat*      maskImage,
                 cv::Point     P,
                 bool*         visitedPoints,
                 short         initialMovement,
                 unsigned char backgroundColor,
                 short&        movement)
{
    int       d;
    cv::Point pNext;

    for (d = 0; d <= 7; d++) {
        movement = (initialMovement + d) % 8;

        pNext.x = P.x + freemanCodeIncrement[movement].x;
        pNext.y = P.y + freemanCodeIncrement[movement].y;

        // the point is inside image ?
        if (pNext.x < 0 || pNext.x >= image.size().width || pNext.y < 0 ||
            pNext.y >= image.size().height) {
            // try other movement
            continue;
        }

        // image has blobColor value in the new point?
        if ((GET_IMAGE_PIXEL(image, pNext) != backgroundColor) &&
            GET_IMAGEMASK_PIXEL(maskImage, pNext)) {
            return pNext;
        } else {
            // mark point as visited
            ASSIGN_VISITED(pNext, visitedPoints, image.size().width);
        }
    }

    // no possible movement was found
    movement = -1;
    pNext.x  = P.x;
    pNext.y  = P.y;

    return pNext;
}
