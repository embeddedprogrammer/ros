#include "ros/ros.h"
//#include "std_msgs/String.h"
#include "geometry_msgs/TransformStamped.h"
#include <tf/transform_broadcaster.h>
#include <sys/time.h> // For gettimeofday

//For serial library
//#include <unistd.h>
//#include <string>
//#include <iostream>
//#include <cstdio>
#include "serial/serial.h"

using std::string;
using std::exception;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;


#define MIN_MOTOR_REFRESH_TIME_MS 1000
#define MOTOR_REFRESH_TIMER_ID 0

//Utility functions
double timerStartTime[10];

double utility_getTime_ms()
{
	struct timeval tp;
	gettimeofday(&tp, NULL);
	return ((double) tp.tv_sec * 1e3 + (double) tp.tv_usec * 1e-3);
}

void utility_startTimer(int timerId)
{
	timerStartTime[timerId] = utility_getTime_ms();
}

double utility_getTimerTime_ms(int timerId)
{
	return (utility_getTime_ms() - timerStartTime[timerId]);
}

double utility_getTimerTime_s(int timerId)
{
	return (utility_getTime_ms() - timerStartTime[timerId]) / 1000;
}

void motorRefresh(const geometry_msgs::TransformStamped& msg)
{
	//ROS_INFO("I heard: [%s]", msg->data.c_str());
	//printf("I heard: [%s]\n", msg.header.frame_id.c_str());
	printf("Location: %f, %f, %f\n", msg.transform.translation.x,
		msg.transform.translation.y, msg.transform.translation.z);
	tf::Quaternion q(msg.transform.rotation.x, msg.transform.rotation.y, 
		msg.transform.rotation.z, msg.transform.rotation.w);
	tf::Matrix3x3 m(q);
	double roll, pitch, yaw;
	m.getRPY(roll, pitch, yaw);
	printf("Roll: %f Pitch: %f Yaw: %f\n", roll, pitch, yaw);
}

/**
 * This tutorial demonstrates simple receipt of messages over the ROS system.
 */
void chatterCallback(const geometry_msgs::TransformStamped& msg)
{
	if(utility_getTimerTime_ms(MOTOR_REFRESH_TIMER_ID) >= MIN_MOTOR_REFRESH_TIME_MS)
	{
		motorRefresh(msg);
		utility_startTimer(MOTOR_REFRESH_TIMER_ID);
	}
}

// Open serial port
serial::Serial my_serial("/dev/ttyUSB0", 9600, serial::Timeout::simpleTimeout(1000));

void sendCommand(int robotId, int leftMotorSpeed, int rightMotorSpeed)
{
	char cmd[4];
	cmd[0] = robotId;
	cmd[1] = 128 + leftMotorSpeed;
	cmd[2] = 128 + rightMotorSpeed;
	cmd[3] = '\n';
	string cmdStr(cmd, cmd + 4);
	my_serial.write(cmdStr);
}

int main(int argc, char **argv)
{
	sendCommand(0, -50, -50);






	/**
	 * The ros::init() function needs to see argc and argv so that it can perform
	 * any ROS arguments and name remapping that were provided at the command line.
	 * For programmatic remappings you can use a different version of init() which takes
	 * remappings directly, but for most command-line programs, passing argc and argv is
	 * the easiest way to do it.	The third argument to init() is the name of the node.
	 *
	 * You must call one of the versions of ros::init() before using any other
	 * part of the ROS system.
	 */
	ros::init(argc, argv, "listener");

	/**
	 * NodeHandle is the main access point to communications with the ROS system.
	 * The first NodeHandle constructed will fully initialize this node, and the last
	 * NodeHandle destructed will close down the node.
	 */
	ros::NodeHandle n;

	/**
	 * The subscribe() call is how you tell ROS that you want to receive messages
	 * on a given topic.	This invokes a call to the ROS
	 * master node, which keeps a registry of who is publishing and who
	 * is subscribing.	Messages are passed to a callback function, here
	 * called chatterCallback.	subscribe() returns a Subscriber object that you
	 * must hold on to until you want to unsubscribe.	When all copies of the Subscriber
	 * object go out of scope, this callback will automatically be unsubscribed from
	 * this topic.
	 *
	 * The second parameter to the subscribe() function is the size of the message
	 * queue.	If messages are arriving faster than they are being processed, this
	 * is the number of messages that will be buffered up before beginning to throw
	 * away the oldest ones.
	 */
	ros::Subscriber sub = n.subscribe("Target1/Origin", 1000, chatterCallback);

	/**
	 * ros::spin() will enter a loop, pumping callbacks.	With this version, all
	 * callbacks will be called from within this thread (the main one).	ros::spin()
	 * will exit when Ctrl-C is pressed, or the node is shutdown by the master.
	 */
	ros::spin();

	return 0;
}

