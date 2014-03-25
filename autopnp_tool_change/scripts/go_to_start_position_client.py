

#! /usr/bin/env python

import roslib; roslib.load_manifest('autopnp_tool_change')
import rospy
import actionlib
import autpnp_tool_change.msg 

import go_to_start_position_action.msg

def go_to_start_position_client():
go_to_start_position_client = actionlib.SimpleActionClient('go_to_start_position_action', autpnp_tool_change.msg.GoToStartPositionAction)

go_to_start_position_client.wait_for_server()

# Creates a goal to send to the action server.
goal = autopnp_tool_change.msg.GoToStartPositionGoal()
goal_goal = "default"

# Sends the goal to the action server.
go_to_start_position_client.send_goal(goal)

# Waits for the server to finish performing the action.
finished_before_timeout = go_to_start_position_client.wait_for_result(rospy.Duration(300, 0))

if finished_before_timeout:
state = go_to_start_position_client.get_state()
print "Action finished: %s"%state
# Prints out the result of executing the action
return state # State after waiting for GoToStartPositionAction

if __name__ == '__main__':
try:
# Initializes a rospy node so that the SimpleActionClient can
# publish and subscribe over ROS.
rospy.init_node('GoToStartPositionAction_client_py')
result = go_to_start_position_client()
except rospy.ROSInterruptException:
print "program interrupted before completion"
