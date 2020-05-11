#include <math.h>
#include <uWS/uWS.h>
#include <iostream>
#include <string>
#include "json.hpp"
#include "PID.h"

//compile switch to switch between Kp and Kd parameter tunning
//and PID testing
#define PARAM_ADJ 0

// for convenience
using nlohmann::json;
using std::string;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
string hasData(string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != string::npos) {
    return "";
  }
  else if (b1 != string::npos && b2 != string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int main() {
  uWS::Hub h;

  static unsigned int trackLenght = 0;
  
  PID steerPID;
  /**
   * TODO: Initialize the pid variable.
   */
#if PARAM_ADJ == 1
  //start with high Ks, we will tune this online
  steerPID.Init(1, 0, 2);
#else
  // these values usually pops out after adjustment
  steerPID.Init(0.179, 0, 1.5);
#endif
  
  h.onMessage([&steerPID](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, 
                     uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2') {
      auto s = hasData(string(data).substr(0, length));

      if (s != "") {
        auto j = json::parse(s);

        string event = j[0].get<string>();

        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<string>());
          double speed = std::stod(j[1]["speed"].get<string>());
          double angle = std::stod(j[1]["steering_angle"].get<string>());
          double steer_value;
          /**
           * TODO: Calculate steering value here, remember the steering value is
           *   [-1, 1].
           * NOTE: Feel free to play around with the throttle and speed.
           *   Maybe use another PID controller to control the speed!
           */
          steer_value = steerPID.RunPID(cte);

          // ### This part is relevant only for determing the P and D params
          static int allowAdjust = 0;
#if PARAM_ADJ == 1
          if (allowAdjust == 100)
          {
            steerPID.adjustP(cte);
            steerPID.adjustD(cte);
          }
          else
          {
            //one track was complete, dont do adjustment anymore
            if(trackLenght<1400)
            {
              allowAdjust++;
            }
          }
          
          if(trackLenght>=1400)
          {
            allowAdjust = 0;
            steerPID.PrintParams();
          }
#endif
          trackLenght++;

          // DEBUG
          std::cout << "CTE: " << cte << " Steering Value: " << steer_value 
                    << std::endl;

          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = 0.3;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          
          //If the CTE is too big, just reset the track and continue to adjust parameters.
          //A delay is added after reseting the track so that the adjustment doesnt start too soon
          if ((cte>2.5) && (allowAdjust == 100))
          {
            msg = "42[\"reset\",{}]";
            allowAdjust = 0;
            trackLenght = 0;
          }
          std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }  // end "telemetry" if
      } else {
        // Manual driving
        string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }  // end websocket message if
  }); // end h.onMessage

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, 
                         char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port)) {
    std::cout << "Listening to port " << port << std::endl;
  } else {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  
  h.run();
}