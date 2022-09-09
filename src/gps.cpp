
#include "ros/ros.h"
#include "tf/transform_broadcaster.h"
#include "nav_msgs/Odometry.h"
#include "sensor_msgs/NavSatFix.h"
#include <geonav_transform/navsat_conversions.h>
#include "iostream"
#include "stdio.h"
#include "time.h"

double latitude;
double longitude;
double th;

double vx = 0.0;
double vy = 0.0;
double vth = 0.0;

void gpsCallBack(const sensor_msgs::NavSatFix::ConstPtr &gps_msg)
{
    latitude = gps_msg->latitude;
    longitude = gps_msg->longitude;
    th = gps_msg->altitude;
}

void callback(const nav_msgs::Odometry::ConstPtr &odom)
{
    printf("the x is: %f\n", odom->pose.pose.position.x);
    printf("the y is: %f\n", odom->pose.pose.position.y);
}

// void xy_based_on_lat_long(float lat, float lon, float olat,float olon){
//     float xg2, yg2;
//     xg2, yg2 = ll2xy(lat,lon,olat,olon);
//     printf("xyz coordinates => %f and %f ", xg2);
// }

int main(int argc, char **argv)
{
    ros::init(argc, argv, "Geoposition");

    ros::NodeHandle nh;
    ros::NodeHandle nh1;
    ros::NodeHandle nh2;

    ros::Subscriber topic_sub = nh.subscribe("/imu/imu/nav_sat_fix", 10, gpsCallBack);
    ros::Publisher position_pub = nh1.advertise<nav_msgs::Odometry>("nav_odom", 100);
    ros::Subscriber sub = nh2.subscribe("odom", 10, callback);

    ros::Time current_time;
    current_time = ros::Time::now();

    ros::Rate loop_rate(10);

    while (nh1.ok())
    {

        current_time = ros::Time::now();

        geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(th);

        nav_msgs::Odometry nav_odom;
        nav_odom.header.stamp = current_time;
        nav_odom.header.frame_id = "odom";
        nav_odom.pose.pose.position.x = longitude;
        nav_odom.pose.pose.position.y = latitude;
        nav_odom.pose.pose.position.z = th;
        nav_odom.pose.pose.orientation = odom_quat;

        nav_odom.header.frame_id = "base_link";
        nav_odom.twist.twist.linear.x = vx;
        nav_odom.twist.twist.linear.y = vy;
        nav_odom.twist.twist.angular.z = vth;

        position_pub.publish(nav_odom);

        ros::spin();
        loop_rate.sleep();
    }
}