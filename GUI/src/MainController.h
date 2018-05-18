/*
 * This file is part of ElasticFusion.
 *
 * Copyright (C) 2015 Imperial College London
 *
 * The use of the code within this file and all code within files that
 * make up the software that is ElasticFusion is permitted for
 * non-commercial purposes only.  The full terms and conditions that
 * apply to the code within this file are detailed within the LICENSE.txt
 * file and at <http://www.imperial.ac.uk/dyson-robotics-lab/downloads/elastic-fusion/elastic-fusion-license/>
 * unless explicitly stated.  By downloading this file you agree to
 * comply with these terms.
 *
 * If you wish to use any of this code for commercial purposes then
 * please email researchcontracts.engineering@imperial.ac.uk.
 *
 */

#include <ElasticFusion.h>
#include <Utils/Parse.h>

#include "Tools/GUI.h"
#include "Tools/GroundTruthOdometry.h"
#include "Tools/RawLogReader.h"
#include "Tools/LiveLogReader.h"
#include <ros/ros.h>
#include <tf2_ros/transform_broadcaster.h>
#include <tf2_ros/transform_listener.h>
#include <std_srvs/Empty.h>

#include <mutex>

#ifndef MAINCONTROLLER_H_
#define MAINCONTROLLER_H_

class MainController
{
    public:
        MainController(int argc, char * argv[], std::shared_ptr<ros::NodeHandle> nh);
        virtual ~MainController();

        void launch();

    private:
        void run();

        void loadCalibration(const std::string & filename);

        bool good;
        ElasticFusion * eFusion;
        GUI * gui;
        GroundTruthOdometry * groundTruthOdometry;
        LogReader * logReader;

        bool iclnuim;
        std::string logFile;
        std::string poseFile;

        float confidence,
              depth,
              icp,
              icpErrThresh,
              covThresh,
              photoThresh,
              fernThresh;

        int timeDelta,
            icpCountThresh,
            start,
            end;

        bool fillIn,
             openLoop,
             reloc,
             frameskip,
             quiet,
             fastOdom,
             so3,
             rewind,
             frameToFrameRGB;

        int framesToSkip;
        bool streaming;
        bool resetButton;

        Resize * resizeStream;
    private:
      bool running = true;

      // Publish pose
      tf2_ros::TransformBroadcaster tf_b;
      ros::Publisher pose_pub;

      // ===== Pointcloud publishing
      std::mutex cloud_mutex;
      // Download from GPU, fills mapData and mapSize
      bool download_cloud = false;
      Eigen::Vector4f* mapData = nullptr;
      unsigned int mapSize = 0;
      // Set to true to publish the latest available cloud
      bool do_cloud_publishing = false;
      // Cloud publishing thread. Reads mapData, converts it to pcl::Pointcloud,
      // downsample using a voxel grid filter
      std::thread cloud_publishing_thread;
      void publishPointCloudThread();

      ros::Publisher cloud_pub;

      // Reset service
      ros::ServiceServer reset_service;

    protected:
      bool reset_callback(std_srvs::Empty::Request &req, std_srvs::Empty::Response &res);
};

#endif /* MAINCONTROLLER_H_ */
