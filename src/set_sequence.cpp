#include "ros/ros.h"
#include "move_base_msgs/MoveBaseAction.h"
#include "actionlib/client/simple_action_client.h"

#include <iostream>

using namespace std;

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

float px_seq[9] = {};
float py_seq[9] = {};
float oz_seq[9] = {};
float ow_seq[9] = {};
int room_seq[9] = {};
int room_count = 0;

void sequence_record();

int main(int argc, char** argv){
    
    ros::init(argc, argv, "simple_navigation_goals");

    MoveBaseClient ac("move_base", true);

    while (!ac.waitForServer(ros::Duration(5.0))){
        ROS_INFO("Waiting action server to get ready");
    }

    char choice_to_continue = 'Y';
    bool run = true;

    while (run)
    {
        cout << "\nSequence record is started. Insert room number sequence" << endl;
        cout << "Insert one number at a time" << endl;
        cout << "Max 9 number. Input 'n' to limit destination";

        sequence_record();

        move_base_msgs::MoveBaseGoal current_goal;
        // move_base_msgs::MoveBaseResult nav_status;
        current_goal.target_pose.header.frame_id = "map";
        current_goal.target_pose.header.stamp = ros::Time::now();
        bool navigation_fail = false;
        int temp_count = room_count;

        while (room_count > 0)
        {
            current_goal.target_pose.pose.position.x = px_seq[temp_count - room_count + 1];
            current_goal.target_pose.pose.position.y = py_seq[temp_count - room_count + 1];
            current_goal.target_pose.pose.orientation.z = oz_seq[temp_count - room_count + 1];
            current_goal.target_pose.pose.orientation.w = ow_seq[temp_count - room_count + 1];

            ac.sendGoal(current_goal);
            ac.waitForResult();
            if (ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
            {
                cout << "\nGoal succeded., going to next goal";
            }
            /* else if(ac.getState() == actionlib::SimpleClientGoalState::ABORTED)
            {
                cout << "Failed to do the goal, aborting all goals...";
                room_count = 1;
            }
            else if(ac.getState() == actionlib::SimpleClientGoalState::REJECTED){
                cout << "Failed to do the goal, aborting all goals...";
                room_count = 1;
            }
            else if(ac.getState() == actionlib::SimpleClientGoalState::PREEMPTED){
                cout << "Failed to do the goal, aborting all goals...";
                room_count = 1;
            }
            else if(ac.getState() == actionlib::SimpleClientGoalState::LOST){
                cout << "Failed to do the goal, aborting all goals...";
                room_count = 1;
            } */
            else
            {
                cout << "\nFailed to do the goal, aborting all goals...";
                room_count = 1;
            }
            

            room_count--;
        }

        do {
            cout << "\nWould you like to go to another destination? (Y/N)" << endl;
            cin >> choice_to_continue;
            choice_to_continue = tolower(choice_to_continue); // Put your letter to its lower case
        } while (choice_to_continue != 'n' && choice_to_continue != 'y'); 
 
        if(choice_to_continue =='n') {
            run = false;
        } 
    }   
}

void sequence_record(){
    char input_temp;
    bool record = true;
    int i = 1;
    while(record){
        START:
        cin >> input_temp;
        switch (input_temp){
            case '1':
            room_count++;
            room_seq[i] = 1;
            px_seq [i] = 2.9698095321655273;
            py_seq [i] = 0.01852560043334961;
            oz_seq [i] = -0.7035725653800884;
            ow_seq [i] = 0.710623420135082;
            break;
        
            case '2':
            room_count++;
            room_seq[i] = 2;
            px_seq [i] = 2.968283176422119;
            py_seq [i] = -4.973784446716309;
            oz_seq [i] = -0.9999751312456314;
            ow_seq [i] = 0.0070524386053544205;
            break;
            
            case '3':
            room_count++;
            room_seq[i] = 3;
            px_seq [i] = -0.01239001750946045;
            py_seq [i] = -4.995876312255859;
            oz_seq [i] = 0.7078620014619394;
            ow_seq [i] = 0.7063507534407373;
            break;

            case '4':
            room_count++;
            room_seq[i] = 4;
            px_seq [i] = 14.214;
            py_seq [i] = -4.201;
            oz_seq [i] = 0;
            ow_seq [i] = 1;
            break;
            
            case 'n':
            room_count++;
            cout << "\nRecord finished.";
            room_count = i-1;
            record = false;
            break;
        
            default:
            cout << "\nInvalid number, try again";
            goto START;
        }

        cout << "\nRoom Count: " << room_count << endl;
        i++;
    }
}