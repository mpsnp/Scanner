//
//  GraphSolver.h
//  GraphScanner
//
//  Created by George Kiriy on 22/06/15.
//  Copyright (c) 2015 SMediaLink. All rights reserved.
//

#ifndef __GraphScanner__GraphSolver__
#define __GraphScanner__GraphSolver__

#include <stdio.h>
#include <vector>
#include <opencv2/opencv.hpp>

/**
 * Class for analyzing images, finding of graphics and calculating of polynomial describing this graphic.
 */
class GraphSolver
{
public:
    cv::Mat thresholdedImage;
    cv::Mat inverseThresholdedImage;
    cv::Mat mask;
    cv::Mat masked;
    std::vector<cv::Vec2f> axes;
    cv::Point pOrigin;
    cv::Point pI;
    cv::Point pJ;
    cv::Matx33d transform;
    std::vector<double> result;
    bool autoCalculateIdentity;
private:
    std::vector< std::vector<cv::Point> > _graphicPoints;
    std::vector< std::vector<cv::Point> > _crosses;
    std::vector<double> _result;
    
    void _thresholdImage(cv::Mat, double);
    void _findAxes();
    void _findOrigin();
    void _findIdentity(double);
    void _findTransformMatrix();
    std::vector<cv::Point> _findInputVectors();
    std::vector<cv::Vec2d> _transformInputVectors(std::vector<cv::Point>);
    std::vector<double> _findPolynom(std::vector<cv::Vec2d>, int);
public:
    GraphSolver();
    /**
     * Calculates polynomial using provided image, automatically fills all it's public variables
     * @param image input image
     * @param degree degree of generated polynomial
     * @param threshold threshold value of binary thresholding image
     * @return array of polynomial coefficients
     */
    std::vector<double> calculatePolynomOnImage(cv::Mat, int, double);
    
    /**
     * Sets identity vectors manually, if they are not found on image or user wants to use own.
     * @param AVector input vector
     * @param ALength length of vector in graphic basis
     * @param AThreshold allowed distance from axes
     */
    void setIdentity(cv::Point AVector, double ALength, double AThreshold);
    
    /**
     * Draws debug data on provided image. It draws axeses, origin, and identity vectors.
     * @param AImage image on which to draw
     */
    void drawDebugDataOnImage(cv::Mat&);
};

#endif /* defined(__GraphScanner__GraphSolver__) */
