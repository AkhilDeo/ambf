//==============================================================================
/*
    Software License Agreement (BSD License)
    Copyright (c) 2019-2021, AMBF
    (https://github.com/WPI-AIM/ambf)

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
    copyright notice, this list of conditions and the following
    disclaimer in the documentation and/or other materials provided
    with the distribution.

    * Neither the name of authors nor the names of its contributors may
    be used to endorse or promote products derived from this software
    without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
    ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

    \author    <amunawar@wpi.edu>
    \author    Adnan Munawar
*/
//==============================================================================

#ifndef AFWORLDCOMM_H
#define AFWORLDCOMM_H

#include <string>
#include "ambf_server/WorldRosCom.h"

#include "sensor_msgs/PointCloud.h"
#include "std_msgs/Float32.h"

namespace ambf_comm{

class PointCloudHandler;
typedef boost::shared_ptr<PointCloudHandler> PointCloudHandlerPtr;
typedef std::map<std::string, PointCloudHandlerPtr> PointCloudHandlerMap;
typedef std::vector<PointCloudHandlerPtr> PointCloudHandlerVec;

enum class WorldParamsEnum{
    point_cloud_topics
};


class WorldParams{

    friend class World;

public:

    WorldParams();

    inline void set_qualified_namespace(std::string a_base_prefix){m_base_prefix = a_base_prefix;}

    // This a flag to check if any param has been updated
    bool m_paramsChanged;

    std::vector<std::string> get_new_topic_names(){return m_new_topic_names;}

    std::vector<std::string> get_defunct_topic_names(){return m_defunct_topic_names;}

    void append_point_cloud_topic(std::string name){m_new_topic_names.push_back(name);}

protected:

    // Namespace + obj_name is the base_prefix. E.g. /ambf/env/ + Camera1 = /ambf/env/Camera1 -> Base Prefix
    std::string m_base_prefix;

    // Datatyped Variables for params defined on the server
    std::vector<std::string> m_point_cloud_topics;

    // At each update, any new topics are added to this list
    std::vector<std::string> m_new_topic_names;

    // At each update, topics to be removed are added to this list
    std::vector<std::string> m_defunct_topic_names;
};

class PointCloudHandler{
public:
    PointCloudHandler(std::string a_topicName);
    ~PointCloudHandler(){
        remove();
    }

    void init();
    void remove();

    sensor_msgs::PointCloudPtr get_point_cloud();

    double get_radius(){return m_radius;}
    void set_radius(double a_radius){m_radius = abs(a_radius);}

private:
    void pc_sub_cb(sensor_msgs::PointCloudPtr msg);
    void radius_sub_cb(std_msgs::Float32Ptr msg);
    ros::Subscriber m_pcSub;
    ros::Subscriber m_radiusSub;
    ros::Subscriber m_colorSub;
    std::string m_topicName;
    sensor_msgs::PointCloudPtr m_StatePtr;

    double m_radius=10;
};

class World: public WorldRosCom, public WorldParams{
public:
    World(std::string a_name, std::string a_namespace, int a_freq_min, int a_freq_max, double time_out);
    inline void set_num_devices(uint a_num){m_State.n_devices = a_num;}
    inline void set_graphics_loop_freq(double a_freq){m_State.graphics_loop_freq = a_freq;}
    inline void set_physics_loop_freq(double a_freq){m_State.dynamic_loop_freq = a_freq;}
    virtual void increment_sim_step();
    inline bool step_sim(){return m_stepSim;}

    // This method updates from the ROS param server instead of topics
    void update_params_from_server();
    // This method may be called when AMBF starts to load the existing
    void set_params_on_server();
};
}

#endif
