//
//  GraphSolver.cpp
//  GraphScanner
//
//  Created by George Kiriy on 22/06/15.
//  Copyright (c) 2015 SMediaLink. All rights reserved.
//

#include "GraphSolver.h"

using namespace cv;
using namespace std;

GraphSolver::GraphSolver()
:autoCalculateIdentity(true)
{
}

void GraphSolver::_thresholdImage(cv::Mat ASourceImage, double AThreshold)
{
    cv::Mat tmp;
    cvtColor(ASourceImage, tmp, CV_BGRA2GRAY);
    threshold(tmp, thresholdedImage, AThreshold * 255, 255, cv::THRESH_BINARY);
    bitwise_not(thresholdedImage, inverseThresholdedImage);
    if (mask.size != thresholdedImage.size)
    {
        thresholdedImage.copyTo(mask);
        mask = cv::Scalar(255, 255, 255);
    }
}

void GraphSolver::_findAxes()
{
    std::vector<cv::Vec2f> lines;
    
    Mat temp;
    inverseThresholdedImage.copyTo(temp);
    
    HoughLines(temp, lines, 1, CV_PI/180, 100);
    
    if (lines.size() > 1)
    {
        float min = std::abs(CV_PI / 2 - std::abs(lines[0][1] - lines[1][1]));
        size_t min_i = 0, min_j = 1;
        for (size_t i = 0; i < lines.size(); i++)
        {
            for (size_t j = i + 1; j < lines.size(); j++)
            {
                if (std::abs(CV_PI / 2 - std::abs(lines[i][1] - lines[j][1])) < min)
                {
                    min = std::abs(CV_PI / 2 - std::abs(lines[i][1] - lines[j][1]));
                    min_i = i;
                    min_j = j;
                }
            }
        }
        
        axes.clear();
        axes.push_back(lines[min_i]);
        axes.push_back(lines[min_j]);
    }
    
    std::sort(axes.begin(), axes.end(), [](Vec2f a, Vec2f b){
        return a(1) < b(1);
    });
}

void GraphSolver::_findOrigin()
{
    if (axes.size())
    {
        cv::Matx21f x;
        
        cv::Matx22f A = cv::Matx22f(cos(axes[0][1]), sin(axes[0][1]), cos(axes[1][1]), sin(axes[1][1]));
        cv::Matx21f B = cv::Matx21f(axes[0][0], axes[1][0]);
        
        solve(A, B, x);
        
        pOrigin = cv::Point(x(0), x(1));
    }
}

void GraphSolver::setIdentity(Point AVector, double ALength, double AThreshold)
{
    AVector = (AVector - pOrigin);
    Vec2d temp = Vec2d(AVector.x, AVector.y) / ALength;
    AVector = Point(pOrigin.x + temp(0), pOrigin.y + temp(1));
    
    autoCalculateIdentity = false;
    
    //if point is near line
    if ((abs(AVector.x * cos(axes[0][1]) + AVector.y * sin(axes[0][1]) - axes[0][0]) < AThreshold) ^
        (abs(AVector.x * cos(axes[1][1]) + AVector.y * sin(axes[1][1]) - axes[1][0]) < AThreshold) )
    {
        //project to right line
        int ax = (abs(AVector.x * cos(axes[1][1]) + AVector.y * sin(axes[1][1]) - axes[1][0]) < AThreshold) ? 1 : 0;
        
        float rho = axes[ax][0], theta = axes[ax][1];
        cv::Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a * rho, y0 = b * rho;
        pt1.x = cvRound(x0 + 1000*(-b));
        pt1.y = cvRound(y0 + 1000*(a));
        pt2.x = cvRound(x0 - 1000*(-b));
        pt2.y = cvRound(y0 - 1000*(a));
        
        Matx21f x;
        
        Matx22f A = Matx22f(pt2.x - pt1.x, pt2.y - pt1.y, pt1.y - pt2.y, pt2.x - pt1.x);
        Matx21f B = Matx21f(AVector.x * (pt2.x - pt1.x) + AVector.y * (pt2.y - pt1.y),
                            pt1.y * (pt2.x - pt1.x) - pt1.x * (pt2.y - pt1.y));
        
        
        solve(A, B, x);
        
        cv::Point projected = cv::Point(x(0), x(1));
        
        //find shortest vectors
        Point2f baseVector = projected - pOrigin;
        double length = norm(baseVector);
        Point2f normalized = baseVector / length;
        
        if (abs(normalized.y) <= 0.5)
        {
            pI = projected;
        }
        
        if (abs(normalized.x) <= 0.5)
        {
            pJ = projected;
        }
    }
}

void GraphSolver::_findIdentity(double AThreshold)
{
    if (autoCalculateIdentity)
    {
        Mat K = (Mat_<uchar>(5,5)<<  \
                 0, 0, 1, 0, 0,\
                 0, 0, 1, 0, 0,\
                 1, 1, 1, 1, 1,\
                 0, 0, 1, 0, 0,\
                 0, 0, 1, 0, 0\
                 );
        
        cv::Mat eroded;
        
        erode(inverseThresholdedImage, eroded, K, cv::Point(-1, -1), 1, BORDER_REFLECT);
        
        vector <cv::Point> crossesAvg;
        vector< Vec4i > hierarchy;
        findContours(eroded, _crosses, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
        
        pI = Point(0,0);
        pJ = Point(0,0);
        
        for (int i = 0; i < _crosses.size(); i++)
        {
            cv::Point average;
            for (int j = 0; j < _crosses[i].size(); j++)
            {
                average = average + _crosses[i][j];
            }
            
            average = cv::Point(average.x / _crosses[i].size(), average.y / _crosses[i].size());
            
            //if point is near line
            if ((abs(average.x * cos(axes[0][1]) + average.y * sin(axes[0][1]) - axes[0][0]) < AThreshold) ^
                (abs(average.x * cos(axes[1][1]) + average.y * sin(axes[1][1]) - axes[1][0]) < AThreshold) )
            {
                //project to right line
                int ax = (abs(average.x * cos(axes[1][1]) + average.y * sin(axes[1][1]) - axes[1][0]) < AThreshold) ? 1 : 0;
                
                float rho = axes[ax][0], theta = axes[ax][1];
                cv::Point pt1, pt2;
                double a = cos(theta), b = sin(theta);
                double x0 = a * rho, y0 = b * rho;
                pt1.x = cvRound(x0 + 1000*(-b));
                pt1.y = cvRound(y0 + 1000*(a));
                pt2.x = cvRound(x0 - 1000*(-b));
                pt2.y = cvRound(y0 - 1000*(a));
                
                Matx21f x;
                
                Matx22f A = Matx22f(pt2.x - pt1.x, pt2.y - pt1.y, pt1.y - pt2.y, pt2.x - pt1.x);
                Matx21f B = Matx21f(average.x * (pt2.x - pt1.x) + average.y * (pt2.y - pt1.y),
                                    pt1.y * (pt2.x - pt1.x) - pt1.x * (pt2.y - pt1.y));
                
                
                solve(A, B, x);
                
                cv::Point projected = cv::Point(x(0), x(1));
                
                //find shortest vectors
                Point2f baseVector = projected - pOrigin;
                double length = norm(baseVector);
                Point2f normalized = baseVector / length;
                
                if (abs(normalized.y) <= 0.5 && normalized.x > 0)
                {
                    if (length < norm(pI - pOrigin))
                    {
                        pI = projected;
                    }
                }
                
                if (abs(normalized.x) <= 0.5 && normalized.y < 0)
                {
                    if (length < norm(pJ - pOrigin))
                    {
                        pJ = projected;
                    }
                }
            }
        }
    }
}

void GraphSolver::_findTransformMatrix()
{
    Matx66d left;
    Matx61d right;
    Matx61d ABCDEF;
    
    left << pOrigin.x, pOrigin.y, 1, 0, 0, 0,
    0, 0, 0, pOrigin.x, pOrigin.y, 1,
    pI.x, pI.y, 1, 0, 0, 0,
    0, 0, 0, pI.x, pI.y, 1,
    pJ.x, pJ.y, 1, 0, 0, 0,
    0, 0, 0, pJ.x, pJ.y, 1;
    
    right << 0, 0, 1, 0, 0, 1;
    
    solve(left, right, ABCDEF);
    
    transform << ABCDEF(0), ABCDEF(1), ABCDEF(2), ABCDEF(3), ABCDEF(4), ABCDEF(5), 0, 0, 1;
}

std::vector<cv::Point> GraphSolver::_findInputVectors()
{
    bitwise_and(inverseThresholdedImage, mask, masked);
    
    for( size_t i = 0; i < axes.size(); i++)
    {
        float rho = axes[i][0], theta = axes[i][1];
        cv::Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a * rho, y0 = b * rho;
        pt1.x = cvRound(x0 + 1000*(-b));
        pt1.y = cvRound(y0 + 1000*(a));
        pt2.x = cvRound(x0 - 1000*(-b));
        pt2.y = cvRound(y0 - 1000*(a));
        line(masked, pt1, pt2, Scalar(0,0,0), 15, CV_AA);
    }
    
    vector< Vec4i > hierarchy;
    findContours(masked, _graphicPoints, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
    

    vector <cv::Point> result;
    
    for (auto contour : _graphicPoints)
    {
        for (auto point : contour)
        {
            result.push_back(point);
        }
    }
    
    std::sort(result.begin(), result.end(), [](Point2f p1, Point2f p2){
        return p1.x < p2.x;
    });
    
    return result;
}

std::vector<cv::Vec2d> GraphSolver::_transformInputVectors(std::vector<cv::Point> APoints)
{
    std::vector<cv::Vec2d> result;
    
    for (auto point : APoints)
    {
        Vec3d vec = Vec3d(point.x, point.y, 1);
        vec = transform * vec;
        result.push_back(Vec2d(vec(0), vec(1)));
    }
    
    return result;
}

std::vector<double> GraphSolver::_findPolynom(std::vector<cv::Vec2d> AInput, int ADegree)
{
    std::vector<double> result;
    
    if (ADegree)
    {
        Mat_<double> left(ADegree, ADegree);
        Mat_<double> right(ADegree, 1);
        Mat_<double> x(ADegree, 1);
        
        for (int i = 0; i < ADegree; i++)
        {
            for (int j = 0; j < ADegree; j++)
            {
                double sum = 0;
                for (auto point : AInput)
                    sum += pow(point(0), i + j);
                left(i, j) = sum;
            }
        }
        
        for (int i = 0; i < ADegree; i++)
        {
            double sum = 0;
            for (auto point : AInput)
                sum += pow(point(0), i) * point(1);
            right(i, 0) = sum;
        }
        
        solve(left, right, x);
        
        result = x;
    }
    
    return result;
}

void GraphSolver::drawDebugDataOnImage(cv::Mat &AImage)
{
    for( size_t i = 0; i < axes.size(); i++)
    {
        float rho = axes[i][0], theta = axes[i][1];
        cv::Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a * rho, y0 = b * rho;
        pt1.x = cvRound(x0 + 1000 * (-b));
        pt1.y = cvRound(y0 + 1000 * (a));
        pt2.x = cvRound(x0 - 1000 * (-b));
        pt2.y = cvRound(y0 - 1000 * (a));
        line(AImage, pt1, pt2, Scalar(i ? 255 : 0, i ? 0 : 255, 0), 2, CV_AA);
    }
    
    rectangle(AImage, pOrigin + Point(5,5), pOrigin - Point(5,5), CV_RGB(255, 255, 255));
    rectangle(AImage, pI + Point(5,5), pI - Point(5,5), Scalar(255, 0, 0));
    rectangle(AImage, pJ + Point(5,5), pJ - Point(5,5), Scalar(0, 255, 0));
    
    for (auto cross : _crosses)
    {
        auto rect = boundingRect(cross);
        rectangle(AImage, rect, Scalar(255,255,0));
    }
    
    fillPoly(AImage, _graphicPoints, Scalar(0,0,255));

    drawGraphicOnImage(AImage, Scalar(255, 0, 255));
}

void GraphSolver::drawGraphicOnImage(Mat &AImage, Scalar AColor)
{
    Vec3d oldVec;
    const double xmin = -10;
    const double xmax = 10;
    for (double x = xmin; x < xmax; x += 0.02)
    {
        double y = 0;
        for (size_t i = 0; i < _result.size(); i++)
        {
            y += pow(x, i) * _result[i];
        }

        Matx33d inverseTransform = transform.inv();
        Vec3d vec = Vec3d(x, y, 1);
        vec = inverseTransform * vec;
        if (x == xmin)
        {
            oldVec = vec;
        }

        line(AImage, Point2d(oldVec(0), oldVec(1)), Point2d(vec(0), vec(1)), AColor, 1);
        oldVec = vec;
    }
}

std::vector<double> GraphSolver::calculatePolynomOnImage(cv::Mat AImage, int ADegree, double AThreshold)
{
    _thresholdImage(AImage, AThreshold);
    _findAxes();
    _findOrigin();
    _findIdentity(5);
    _findTransformMatrix();
    
    
    _result = _findPolynom(_transformInputVectors(_findInputVectors()), ADegree);
    
    return _result;
}
