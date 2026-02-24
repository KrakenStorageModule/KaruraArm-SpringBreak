/*      Title     : servo_keyboard_input.cpp
 *      Project   : moveit2_tutorials
 *      Created   : 05/31/2021
 *      Author    : Adam Pettinger
 */

#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist_stamped.hpp>
#include <control_msgs/msg/joint_jog.hpp>
#include <std_msgs/msg/float64_multi_array.hpp>

#include <signal.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

// Define used keys
#define KEYCODE_1 0x31
#define KEYCODE_2 0x32
#define KEYCODE_3 0x33
#define KEYCODE_4 0x34
#define KEYCODE_5 0x35
#define KEYCODE_6 0x36
#define KEYCODE_7 0x37
#define KEYCODE_Q 0x71

#define KEYCODE_W 0x77 
#define KEYCODE_A 0x61
#define KEYCODE_S 0x73
#define KEYCODE_D 0x64
#define KEYCODE_O 0x6F //o key,up
#define KEYCODE_L 0x6C //l ley,down
#define KEYCODE_I 0x69
#define KEYCODE_K 0x6B
#define KEYCODE_U 0x75
#define KEYCODE_J 0x6A
#define KEYCODE_M 0x6D

#define KEYCODE_R 0x72
#define KEYCODE_Z 0x7A
#define KEYCODE_X 0x78
// Some constants used in the Servo Teleop demo
const std::string TWIST_TOPIC = "/servo_node/delta_twist_cmds";
const std::string JOINT_TOPIC = "/servo_node/delta_joint_cmds";
const size_t ROS_QUEUE_SIZE = 10;
const std::string EEF_FRAME_ID = "hand";
const std::string BASE_FRAME_ID = "base_link";
int case_select = 0;
int direction = 1;
float hand_status = 0;
float tuning_signal = 0;
// A class for reading the key inputs from the terminal
class KeyboardReader
{
public:
  KeyboardReader() : kfd(0)
  {
    // get the console in raw mode
    tcgetattr(kfd, &cooked);
    struct termios raw;
    memcpy(&raw, &cooked, sizeof(struct termios));
    raw.c_lflag &= ~(ICANON | ECHO);
    // Setting a new line, then end of file
    raw.c_cc[VEOL] = 1;
    raw.c_cc[VEOF] = 2;
    tcsetattr(kfd, TCSANOW, &raw);
  }
  void readOne(char* c)
  {
    int rc = read(kfd, c, 1);
    if (rc < 0)
    {
      throw std::runtime_error("read failed");
    }
  }
  void shutdown()
  {
    tcsetattr(kfd, TCSANOW, &cooked);
  }

private:
  int kfd;
  struct termios cooked;
};

// Converts key-presses to Twist or Jog commands for Servo, in lieu of a controller
class KeyboardServo
{
public:
  KeyboardServo();
  int keyLoop();

private:
  void spin();

  rclcpp::Node::SharedPtr nh_;

  rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr twist_pub_;
  rclcpp::Publisher<control_msgs::msg::JointJog>::SharedPtr joint_pub_;

  std::string frame_to_publish_;
  double joint_vel_cmd_;
};

KeyboardServo::KeyboardServo() : frame_to_publish_(BASE_FRAME_ID), joint_vel_cmd_(1.0)
{
  nh_ = rclcpp::Node::make_shared("servo_keyboard_input");

  twist_pub_ = nh_->create_publisher<geometry_msgs::msg::TwistStamped>(TWIST_TOPIC, ROS_QUEUE_SIZE);
  joint_pub_ = nh_->create_publisher<control_msgs::msg::JointJog>(JOINT_TOPIC, ROS_QUEUE_SIZE);
}

KeyboardReader input;

void quit(int sig)
{
  (void)sig;
  input.shutdown();
  rclcpp::shutdown();
  exit(0);
}

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);

  KeyboardServo keyboard_servo;

  signal(SIGINT, quit);

  int rc = keyboard_servo.keyLoop();
  input.shutdown();
  rclcpp::shutdown();

  return rc;
}

void KeyboardServo::spin()
{
  while (rclcpp::ok())
  {
    rclcpp::spin_some(nh_);
  }
}

int KeyboardServo::keyLoop()
{
  char c;
  bool publish_twist = false;
  bool publish_joint = false;

  auto node = rclcpp::Node::make_shared("hand_publisher");
  auto publisher = node->create_publisher<std_msgs::msg::Float64MultiArray>("status", 10);
  auto status_msg = std_msgs::msg::Float64MultiArray();  
  status_msg.data.resize(1);

  auto node1 = rclcpp::Node::make_shared("status_publisher");
  auto publisher1 = node1->create_publisher<std_msgs::msg::Float64MultiArray>("tuning_signal", 10);
  auto status_msg1 = std_msgs::msg::Float64MultiArray();  
  status_msg1.data.resize(1);

  std::thread{ std::bind(&KeyboardServo::spin, this) }.detach();

  puts("Reading from keyboard");
  puts("---------------------------");
  puts("Use W-A-S-D and O-L to Cartesian jog");
  //puts("Use 'I' to Cartesian jog in the world frame, and 'K' for the End-Effector frame");
  puts("Use 1|2|3|4|5|6|7 keys to joint jog. 'R' to reverse the direction of jogging.");
  puts("Z key: x-y-z, X key: r-p-y , U:close hand J:stop M: open");
  puts("A-D:roll,W-S:pitch,O-L:yaw");  
  puts("'Q' to quit.");

  for (;;)
  {
    // get the next event from the keyboard
    try
    {
      input.readOne(&c);
    }
    catch (const std::runtime_error&)
    {
      perror("read():");
      return -1;
    }

    RCLCPP_DEBUG(nh_->get_logger(), "value: 0x%02X\n", c);

    // // Create the messages we might publish
    auto twist_msg = std::make_unique<geometry_msgs::msg::TwistStamped>();
    auto joint_msg = std::make_unique<control_msgs::msg::JointJog>();

    // Use read key-press
    switch (c)
    {
      case KEYCODE_A:
        RCLCPP_DEBUG(nh_->get_logger(), "A");
        if (case_select == 0){
        twist_msg->twist.linear.y = 0.3;
        publish_twist = true;
        } else if(case_select == 1){
        twist_msg->twist.angular.y = 0.3;
        publish_twist = true;          
        }
        break;
      case KEYCODE_D:
        RCLCPP_DEBUG(nh_->get_logger(), "D");
        if (case_select == 0){
        twist_msg->twist.linear.y = -0.3;
        publish_twist = true;
        } else if(case_select == 1){
        twist_msg->twist.angular.y = -0.3;
        publish_twist = true;          
        }
        break;
      case KEYCODE_W:
        RCLCPP_DEBUG(nh_->get_logger(), "W");
        if (case_select == 0){
        twist_msg->twist.linear.x = 0.3;
        publish_twist = true;
        } else if(case_select == 1){
        twist_msg->twist.angular.x = 0.3;
        publish_twist = true;          
        }
        break;
      case KEYCODE_S:
        RCLCPP_DEBUG(nh_->get_logger(), "S");
        if (case_select == 0){
        twist_msg->twist.linear.x = -0.3;
        publish_twist = true;
        } else if(case_select == 1){
        twist_msg->twist.angular.x = -0.3;
        publish_twist = true;          
        }
        break;
      case KEYCODE_O:
        RCLCPP_DEBUG(nh_->get_logger(), "O");
        if (case_select == 0){
        twist_msg->twist.linear.z = 0.3;
        publish_twist = true;
        } else if(case_select == 1){
        twist_msg->twist.angular.z = 0.3;
        publish_twist = true;          
        }
        break;
      case KEYCODE_L:
        RCLCPP_DEBUG(nh_->get_logger(), "L");
        if (case_select == 0){
        twist_msg->twist.linear.z = -0.3;
        publish_twist = true;
        } else if(case_select == 1){
        twist_msg->twist.angular.z = -0.3;
        publish_twist = true;          
        }
        break;
      case KEYCODE_U:
        RCLCPP_DEBUG(nh_->get_logger(), "U");
        hand_status = 1;
        status_msg.data[0] = hand_status;
        publisher->publish(status_msg);        
        break;
      case KEYCODE_J:
        RCLCPP_DEBUG(nh_->get_logger(), "J");
        hand_status = 0;
        status_msg.data[0] = hand_status;
        publisher->publish(status_msg);               
        break;        
      case KEYCODE_M:
        RCLCPP_DEBUG(nh_->get_logger(), "M");
        hand_status = -1;        
        status_msg.data[0] = hand_status;
        publisher->publish(status_msg);               
        break;  

      case KEYCODE_1:
        RCLCPP_DEBUG(nh_->get_logger(), "1");
        joint_msg->joint_names.push_back("joint_1"); //joint1
        joint_msg->velocities.push_back(0.5*joint_vel_cmd_); 
        publish_joint = true;
        break;
      case KEYCODE_2:
        RCLCPP_DEBUG(nh_->get_logger(), "2");
        joint_msg->joint_names.push_back("joint_2"); //joint2
        joint_msg->velocities.push_back(0.5*joint_vel_cmd_);
        publish_joint = true;
        break;
      case KEYCODE_3:
        RCLCPP_DEBUG(nh_->get_logger(), "3");
        joint_msg->joint_names.push_back("joint_3"); //...
        joint_msg->velocities.push_back((0.5)*joint_vel_cmd_);
        publish_joint = true;
        break;
      case KEYCODE_4:
        RCLCPP_DEBUG(nh_->get_logger(), "4");
        joint_msg->joint_names.push_back("joint_4");
        joint_msg->velocities.push_back((-0.5)*joint_vel_cmd_);
        publish_joint = true;
        break;
      case KEYCODE_5:
        RCLCPP_DEBUG(nh_->get_logger(), "5");
        joint_msg->joint_names.push_back("joint_5");
        joint_msg->velocities.push_back((-0.5)*joint_vel_cmd_);
        publish_joint = true;
        break;
      case KEYCODE_6:
        RCLCPP_DEBUG(nh_->get_logger(), "6");
        joint_msg->joint_names.push_back("joint_6");
        joint_msg->velocities.push_back(0.5*joint_vel_cmd_);
        publish_joint = true;
        break;
      case KEYCODE_R:
        RCLCPP_DEBUG(nh_->get_logger(), "R");
        joint_vel_cmd_ *= -1;
        direction *= -1;
        status_msg.data[1] = direction;
        if(direction == 1){
          puts("forward.j1:left.j2:down.j3:up.j4:right.j5:up.j6:right");          
        } else {
          puts("back.j1:right.j2:up.j3:down.j4:left.j5:downj6:left");                 
        }
        break;
      case KEYCODE_Z:
        case_select = 0;
        break;
      case KEYCODE_X:
        case_select = 1;
        break;
      case KEYCODE_Q:
        RCLCPP_DEBUG(nh_->get_logger(), "quit");
        return 0;
    }

    // If a key requiring a publish was pressed, publish the message now
    if (publish_twist)
    {
      twist_msg->header.stamp = nh_->now();
      twist_msg->header.frame_id = frame_to_publish_;
      twist_pub_->publish(std::move(twist_msg));
      publish_twist = false;
    }
    else if (publish_joint)
    {
      joint_msg->header.stamp = nh_->now();
      joint_msg->header.frame_id = BASE_FRAME_ID;
      joint_pub_->publish(std::move(joint_msg));
      publish_joint = false;
    }
  }

  return 0;
}
