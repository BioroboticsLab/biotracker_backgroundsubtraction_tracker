#pragma once

#include <opencv2/opencv.hpp>

class BlobPose
{
public:
    /**
     * The standard contructor.
     */
    BlobPose(void);

    /**
     * The contructor with parameters.
     * @param: pos_cm, position in real world coordinate.
     * @param: pos_px, position in pixel coordinate.
     * @param: angle_degree, orientation angle in degree.
     * @param: width, width in px.
     * @param: height, height in px.
     * @param: used, check if this blob pose is currently in use.
     */
    BlobPose(cv::Point2f pos_cm,
             cv::Point   pos_px,
             float       angle_degree,
             float       width,
             float       height,
             bool        use = false);

    /**
     * The standard destructor.
     */
    ~BlobPose(void);

    /**
     * Gets the pixel position of the pose.
     * @return pixel position.
     */
    cv::Point posPx()
    {
        return _center_px;
    }

    /**
     * Gets the real world position of the pose.
     * @return real world position.
     */
    cv::Point2f posCm()
    {
        return _center_cm;
    }

    /**
     * Gets the orientation angle as degree of the pose.
     * @return degree angle.
     */
    float angleDegree()
    {
        return _angle_degree;
    }

    /**
     * Gets the width of the blob pose.
     * @return width in px.
     */
    float width()
    {
        return _width;
    }

    /**
     * Gets the height of the blob pose.
     * @return height in px.
     */
    float height()
    {
        return _height;
    }

    /**
     * Gets the flag whether this pose blob is already in use.
     * @return true if is used, false otherwise.
     */
    bool isUsed()
    {
        return _used;
    }

    /**
     * Sets the pixel position of the pose.
     * @param int x, int y
     * @return void.
     */
    void setposPx(int x, int y)
    {
        cv::Point point(x, y);
        _center_px = point;
    }

    /**
     * Sets the angle in degree.
     * @param float angleDegree
     * @return void.
     */
    void setAngle(float angleDegree)
    {
        _angle_degree = angleDegree;
    }

private:
    bool        _used;
    cv::Point2f _center_cm;
    cv::Point   _center_px;
    float       _angle_degree;
    float       _width;
    float       _height;
};
