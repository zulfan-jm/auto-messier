#include "ros/ros.h"
#include "marvelmind_nav/hedge_imu_fusion.h"
#include "marvelmind_nav/hedge_pos_ang.h"
#include "sensor_msgs/Imu.h"
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include "tf/transform_broadcaster.h"

class hedge_msg_adapter_node
{
public:
  hedge_msg_adapter_node() // Class constructor
  {
    ros::NodeHandle nh_; // Public nodehandle for pub-sub
    ros::NodeHandle nh_private_("~"); // Private nodehandle for handling parameters

    // Init subscribers
    imu_fusion_sub_ = nh_.subscribe("hedge_imu_fusion", 10, &hedge_msg_adapter_node::imu_fusion_callback, this);
    pos_ang_sub_ = nh_.subscribe("hedge_pos_ang", 10, &hedge_msg_adapter_node::pos_ang_callback, this);

    // Init publishers
    hedge_pose_pub_ = nh_.advertise<geometry_msgs::PoseWithCovarianceStamped>("hedge_pose", 10, false);
    hedge_imu_pub_ = nh_.advertise<sensor_msgs::Imu>("hedge_imu", 10, false);

    // You must provide the static transforms for these in a launch file!
    imu_out_.header.frame_id = "beacon_imu_link";
    pose_out_.header.frame_id = "beacon_map";

    // Init covariances grabbed from the parameter server
    init_covariances(nh_private_);
  }

  void imu_fusion_callback(const marvelmind_nav::hedge_imu_fusion::ConstPtr& imu_fusion_msg)
  {
    // Populate header
    imu_out_.header.stamp = ros::Time::now();

    // Populate orientation data
    imu_out_.orientation.x = imu_fusion_msg->qx;
    imu_out_.orientation.y = imu_fusion_msg->qy;
    imu_out_.orientation.z = imu_fusion_msg->qz;
    imu_out_.orientation.w = imu_fusion_msg->qw;

    // Populate angular velocity data
    imu_out_.angular_velocity.x = imu_fusion_msg->vx;
    imu_out_.angular_velocity.y = imu_fusion_msg->vy;
    imu_out_.angular_velocity.z = imu_fusion_msg->vz;

    // Populate linear acceleration data
    imu_out_.linear_acceleration.x = imu_fusion_msg->ax;
    imu_out_.linear_acceleration.y = imu_fusion_msg->ay;
    imu_out_.linear_acceleration.z = imu_fusion_msg->az;

    // Publish the sensor_msgs/Imu message
    hedge_imu_pub_.publish(imu_out_);
  }

double last_x = 0;
double last_y = 0;

void pos_ang_callback(const marvelmind_nav::hedge_pos_ang::ConstPtr& pos_ang_msg)
  {
    // Populate header
    pose_out_.header.stamp = ros::Time::now();

    double current_x = pos_ang_msg->x_m;
    double current_y = pos_ang_msg->y_m;

    if(last_x && last_y == 0){
      last_x = current_x;
      last_y = current_y;
    }
    // Populate position data
    if(abs(last_x - current_x) > 0.02) {
      pose_out_.pose.pose.position.x = last_x;
      } else {
        pose_out_.pose.pose.position.x = current_x;
      }
      
    if(abs(last_y - current_y) > 0.02) {
      pose_out_.pose.pose.position.y = last_y;
      } else {
        pose_out_.pose.pose.position.y = current_y;
      }

    last_x = pose_out_.pose.pose.position.x;
    last_y = pose_out_.pose.pose.position.y;
    // pose_out_.pose.pose.position.x = pos_ang_msg->x_m;
    // pose_out_.pose.pose.position.y = pos_ang_msg->y_m;
    pose_out_.pose.pose.position.z = pos_ang_msg->z_m;

    // Populate orientation data
    pose_out_.pose.pose.orientation = tf::createQuaternionMsgFromYaw(pos_ang_msg->angle);

    // Publish the geometry_msgs/PoseWithCovarianceStamped message
    hedge_pose_pub_.publish(pose_out_);
  }

  // Handy function for initialising covariance matrices from parameters
  void init_covariances(ros::NodeHandle &nh_private_)
  {
    // Create the vectors to store the covariance matrix arrays
    std::vector<double> orientation_covar;
    std::vector<double> ang_vel_covar;
    std::vector<double> linear_accel_covar;
    std::vector<double> pose_covar;

    // Grab the parameters and populate the vectors
    nh_private_.getParam("imu_orientation_covariance", orientation_covar);
    nh_private_.getParam("imu_angular_velocity_covariance", ang_vel_covar);
    nh_private_.getParam("imu_linear_acceleration_covariance", linear_accel_covar);
    nh_private_.getParam("pose_covariance", pose_covar);

    // Iterate through each vector and populate the respective message fields
    for (int i = 0; i < orientation_covar.size(); i++)
      imu_out_.orientation_covariance[i] = orientation_covar.at(i);

    for (int i = 0; i < ang_vel_covar.size(); i++)
      imu_out_.angular_velocity_covariance[i] = ang_vel_covar.at(i);

    for (int i = 0; i < linear_accel_covar.size(); i++)
      imu_out_.linear_acceleration_covariance[i] = linear_accel_covar.at(i);

    for (int i = 0; i < pose_covar.size(); i++)
      pose_out_.pose.covariance[i] = pose_covar.at(i);
  }

protected:
  // Subscriber objects
  ros::Subscriber imu_fusion_sub_;
  ros::Subscriber pos_ang_sub_;

  // Publisher objects
  ros::Publisher hedge_pose_pub_;
  ros::Publisher hedge_imu_pub_;

  // Message objects
  sensor_msgs::Imu imu_out_;
  geometry_msgs::PoseWithCovarianceStamped pose_out_;
};

int main(int argc, char** argv)
{
  ros::init(argc, argv, "hedge_msg_adapter");

  hedge_msg_adapter_node adapter;

  ros::spin();
}