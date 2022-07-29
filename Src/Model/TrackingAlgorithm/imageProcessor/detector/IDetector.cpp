#include "IDetector.h"

#include "../../../../helper/CvHelper.h"
template<typename T>

std::vector<FishPose> IDetector<T>::convertBlobPosesToFishPoses(
    std::vector<BlobPose> blobPoses)
{
    std::vector<FishPose> fishPoses;
    fishPoses.reserve(blobPoses.size());

    for (BlobPose& blobPose : blobPoses) {
        fishPoses.push_back(
            FishPose(blobPose.posCm(),
                     blobPose.posPx(),
                     CvHelper::degToRad(blobPose.angleDegree()),
                     blobPose.angleDegree(),
                     blobPose.width(),
                     blobPose.height()));
    }

    return fishPoses;
}
