#include <ros/ros.h>
#include <std_msgs/Int32MultiArray.h>
#include <nav_msgs/Odometry.h>
#include <map>


using namespace std;


class Transformer
{
  ros::NodeHandle nh_;
  ros::Subscriber sub;

  int size;
  int x1, x2, x3, x4, y1, y2, y3, y4;
  int centre_x, centre_y;
  int robot_id;
  nav_msgs::Odometry robot_pose_msg;
  std::map<int, ros::Publisher> topics;


public:
  Transformer()
  {
    // Subscribe to Tracker node
    sub = nh_.subscribe("/tracker/positions", 1,
      &Transformer::transformCb, this);

    // Initialize publishers for marker ids 0 to 29
    for (int i = 0; i < 30; i++){
        // Create topic name
         char numstr[21];
         string topic_start = "/robot";
         sprintf(numstr, "%d", i);
         string topic = topic_start + numstr + "/odom";

         // Add publisher to list of publishers
         ros::Publisher pub = nh_.advertise<nav_msgs::Odometry>(topic, 1);
         topics.insert(pair<int, ros::Publisher>(i, pub));

    }







  }

  void transformCb(const std_msgs::Int32MultiArray& msg)
  {

    size = msg.data.at(0); //Get size of incoming list
    for (int i = 1; i <= size; i += 9){ // For each marker there is 9 integers of information: 1 for the id and 2 for each corner coordinate
        int x1, x2, x3, x4, y1, y2, y3, y4;

         robot_id = msg.data.at(0 + i);

         // Compute center for each robot given the coordinates of the 4 marker corners
         x1 = msg.data.at(1 + i);
         x2 = msg.data.at(3 + i);
         x3 = msg.data.at(5 + i);
         x4 = msg.data.at(7 + i);

         centre_x = (x1 + x2 + x3 + x4) / 4;

         y1 = msg.data.at(2 + i);
         y2 = msg.data.at(4 + i);
         y3 = msg.data.at(6 + i);
         y4 = msg.data.at(8 + i);

         centre_y = (y1 + y2 + y3 + y4) / 4;

         // Create ROS message
         robot_pose_msg.pose.pose.position.x = centre_x;
         robot_pose_msg.pose.pose.position.y = centre_y;
         robot_pose_msg.pose.pose.position.z = 0;

         // Publish to corresponding topic
         topics[robot_id].publish(robot_pose_msg);
    }


  }
};

int main(int argc, char** argv)
{

  ros::init(argc, argv, "tsfr");
  Transformer tsfr;
  ros::spin();
  return 0;
}
