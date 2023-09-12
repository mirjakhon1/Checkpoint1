#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "my_rb1_ros/rotate.h"
#include "nav_msgs/Odometry.h"
#include <tf/tf.h>


ros::Publisher vel_pub;
geometry_msgs::Twist vel_msg;
ros::Subscriber odomSubscriber;
nav_msgs::Odometry my_odom_msg;
double yaw;
double yaw_old;


void odomCallback(const nav_msgs::Odometry::ConstPtr& msg)
{
    tf::Pose pose;
    tf::poseMsgToTF(msg->pose.pose, pose);
    yaw = tf::getYaw(pose.getRotation());
}

bool my_callback(my_rb1_ros::rotate::Request  &req,
                 my_rb1_ros::rotate::Response &res)
{  

  ROS_INFO("The Service /rotate_robot has been called");

    double distance = 0.0;

    vel_msg.angular.z = -0.5 * ((float)req.degrees / std::abs(req.degrees));
    vel_pub.publish(vel_msg);

    yaw_old = std::abs(yaw);

    while (distance < std::abs(req.degrees)) {
      double distance_rad = std::abs(std::abs(yaw) - yaw_old);
      distance += (180 / M_PI) * distance_rad;
      yaw_old = std::abs(yaw);

      ros::spinOnce();
    }

    vel_msg.angular.z = 0.0;
    vel_pub.publish(vel_msg);

    res.result = "true";
    ROS_INFO("Finished service /rotate_robot");
    return true;
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "rotate_robot_server");
  ros::NodeHandle nh;

  ros::ServiceServer my_service = nh.advertiseService("/rotate_robot", my_callback);
  vel_pub = nh.advertise<geometry_msgs::Twist>("cmd_vel", 1000);
  // Subscribe to the /odom topic
  odomSubscriber = nh.subscribe("/odom", 10, odomCallback);

  ROS_INFO("Service /rotate_robot Ready");
  ros::spin();

  return 0;
}