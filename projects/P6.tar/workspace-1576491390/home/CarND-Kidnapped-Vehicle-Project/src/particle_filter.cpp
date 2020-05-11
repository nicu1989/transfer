/**
 * particle_filter.cpp
 *
 * Created on: Dec 12, 2016
 * Author: Tiffany Huang
 */

#include "particle_filter.h"

#include <math.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>
#include <string>
#include <vector>
#include <cfloat>

#include "helper_functions.h"

using namespace std;

#define NUM_PARTICLES 100

void ParticleFilter::init(double x, double y, double theta, double std[]) {
  /**
   * TODO: Set the number of particles. Initialize all particles to 
   *   first position (based on estimates of x, y, theta and their uncertainties
   *   from GPS) and all weights to 1. 
   * TODO: Add random Gaussian noise to each particle.
   * NOTE: Consult particle_filter.h for more information about this method 
   *   (and others in this file).
   */
  
  // create normal distributions for x,y and theta
  double std_x = std[0];
  double std_y = std[1];
  double std_theta = std[2];
  
  std::normal_distribution<double> dist_x(x, std_x);
  std::normal_distribution<double> dist_y(y, std_y);
  std::normal_distribution<double> dist_theta(theta, std_theta);
  
  // create the random generator and create the particles with the random noise
  std::default_random_engine gen;
  
  // this is tunned empirical
  num_particles = NUM_PARTICLES;  
  particles.resize(num_particles);
  weights.resize(num_particles);

  // initialize all particles first position and weights(initial weight=1) 
  for(int i = 0; i < num_particles; i++)
  {
    particles[i].id = i;
    particles[i].x = dist_x(gen);
    particles[i].y = dist_y(gen);
    particles[i].theta = dist_theta(gen);
    particles[i].weight = 1;
    weights[i] = 1;
  }
  
  // mark the initialization completion
  is_initialized = true;
}

/* implements this diagram part:
*	|----------|	|----------|
*	| Yaw Rate |	|Velocity  |
*	|----------|	|----------|
*		|				|
*		-----------------
*				|
*			Prediction Step
*				|
*			|----------|
*			|  Noise   |
*			|----------|
*/
void ParticleFilter::prediction(double delta_t, double std_pos[], 
                                double velocity, double yaw_rate) {
  /**
   * TODO: Add measurements to each particle and add random Gaussian noise.
   * NOTE: When adding noise you may find std::normal_distribution 
   *   and std::default_random_engine useful.
   *  http://en.cppreference.com/w/cpp/numeric/random/normal_distribution
   *  http://www.cplusplus.com/reference/random/default_random_engine/
   */
  double std_x = std_pos[0];
  double std_y = std_pos[1];
  double std_theta = std_pos[2];
  std::default_random_engine gen;
  
  for(int i = 0; i < num_particles; i++)
  {
   
    particles[i].x = computeXf(particles[i].x, particles[i].theta, delta_t, velocity, yaw_rate);
    particles[i].y = computeYf(particles[i].y, particles[i].theta, delta_t, velocity, yaw_rate);
    particles[i].theta = computeThetaf(particles[i].theta, delta_t, yaw_rate);
    
    // add the noise to each particle
    std::normal_distribution<double> dist_theta(particles[i].theta, std_theta);
    particles[i].theta = dist_theta(gen);
 
    std::normal_distribution<double> dist_x(particles[i].x, std_x);
    particles[i].x = dist_x(gen);
  
    std::normal_distribution<double> dist_y(particles[i].y, std_y);
    particles[i].y = dist_y(gen);
    
  }
}

void ParticleFilter::dataAssociation(vector<LandmarkObs> predicted, 
                                     vector<LandmarkObs>& observations) {
  /**
   * TODO: Find the predicted measurement that is closest to each 
   *   observed measurement and assign the observed measurement to this 
   *   particular landmark.
   * NOTE: this method will NOT be called by the grading code. But you will 
   *   probably find it useful to implement this method and use it as a helper 
   *   during the updateWeights phase.
   */

  // go through all observations and predictions and find for each observation the best predicted landmark
  // based on the distance of all predicted points to that observed landmark.
  for (int i = 0; i < observations.size(); i++)
  {
    //start with a high distance(maximum double type value)
    double higher_dist = DBL_MAX;

    for (int j = 0; j < predicted.size(); j++) 
    {
      double distance = dist(observations[i].x, observations[i].y, predicted[j].x, predicted[j].y);
      
      if( higher_dist > distance)
      {
        higher_dist = distance;
        observations[i].id = predicted[j].id;
      }
    }
  }
}

void ParticleFilter::updateWeights(double sensor_range, double std_landmark[], 
                                   const vector<LandmarkObs> &observations, 
                                   const Map &map_landmarks) {
  /**
   * TODO: Update the weights of each particle using a mult-variate Gaussian 
   *   distribution. You can read more about this distribution here: 
   *   https://en.wikipedia.org/wiki/Multivariate_normal_distribution
   * NOTE: The observations are given in the VEHICLE'S coordinate system. 
   *   Your particles are located according to the MAP'S coordinate system. 
   *   You will need to transform between the two systems. Keep in mind that
   *   this transformation requires both rotation AND translation (but no scaling).
   *   The following is a good resource for the theory:
   *   https://www.willamette.edu/~gorr/classes/GeneralGraphics/Transforms/transforms2d.htm
   *   and the following is a good resource for the actual equation to implement
   *   (look at equation 3.33) http://planning.cs.uiuc.edu/node99.html
   */

  for(int i=0; i < particles.size(); i++)
  {
    particles[i].weight = 1.0;

    // collect valid landmarks
    vector<LandmarkObs> predictions;
    
    
    //for(int l = 1; l < map_landmarks.landmark_list.size(); l++)
    //try dynamic aproach
    for(auto& landmark: map_landmarks.landmark_list)
    {
      
      double distance = dist(particles[i].x, particles[i].y, landmark.x_f, landmark.y_f);
      // if the landmark is within the sensor range, save it to predictions
      if( distance < sensor_range)
      { 
        predictions.push_back(LandmarkObs{landmark.id_i, landmark.x_f, landmark.y_f});
      }
    }

    // convert coordinates from vehicle to map
    vector<LandmarkObs> observations_map;
    double cos_theta = cos(particles[i].theta);
    double sin_theta = sin(particles[i].theta);

    for(auto& obs: observations)
    {
      LandmarkObs tmp;
      tmp.x = obs.x * cos_theta - obs.y * sin_theta + particles[i].x;
      tmp.y = obs.x * sin_theta + obs.y * cos_theta + particles[i].y;
      
      observations_map.push_back(tmp);
    }

    // find landmark index for each observation
    dataAssociation(predictions, observations_map);

    // compute the particle's weight:
    for(auto& obs_m: observations_map)
    {
      Map::single_landmark_s landmark = map_landmarks.landmark_list.at(obs_m.id-1);
      double x_term = pow(obs_m.x - landmark.x_f, 2) / (2 * pow(std_landmark[0], 2));
      double y_term = pow(obs_m.y - landmark.y_f, 2) / (2 * pow(std_landmark[1], 2));
      double w = exp(-(x_term + y_term)) / (2 * M_PI * std_landmark[0] * std_landmark[1]);
      particles[i].weight *=  w;
    }

    weights.push_back(particles[i].weight);
  }
}

/* Resample particles with replacement with probability proportional to their weight.
 * Reference 1: std::discrete_distribution, see the link below with an example
 * http://en.cppreference.com/w/cpp/numeric/random/discrete_distribution
 */
void ParticleFilter::resample() {

  // generate distribution according to weights
  default_random_engine  gen;
  discrete_distribution<> dist(weights.begin(), weights.end());

  // create resampled particles
  vector<Particle> resampled_particles;
  resampled_particles.resize(num_particles);

  // resample the particles according to weights
  for(int i=0; i<num_particles; i++)
  {
    int idx = dist(gen);
    resampled_particles[i] = particles[idx];
  }

  // assign the resampled_particles to the previous particles
  particles = resampled_particles;

  // clear the weight vector for the next round
  weights.clear();
}

void ParticleFilter::SetAssociations(Particle& particle, 
                                     const vector<int>& associations, 
                                     const vector<double>& sense_x, 
                                     const vector<double>& sense_y) {
  // particle: the particle to which assign each listed association, 
  //   and association's (x,y) world coordinates mapping
  // associations: The landmark id that goes along with each listed association
  // sense_x: the associations x mapping already converted to world coordinates
  // sense_y: the associations y mapping already converted to world coordinates
  particle.associations = associations;
  particle.sense_x = sense_x;
  particle.sense_y = sense_y;
}

string ParticleFilter::getAssociations(Particle best) {
  vector<int> v = best.associations;
  std::stringstream ss;
  copy(v.begin(), v.end(), std::ostream_iterator<int>(ss, " "));
  string s = ss.str();
  s = s.substr(0, s.length()-1);  // get rid of the trailing space
  return s;
}

string ParticleFilter::getSenseCoord(Particle best, string coord) {
  vector<double> v;

  if (coord == "X") {
    v = best.sense_x;
  } else {
    v = best.sense_y;
  }

  std::stringstream ss;
  copy(v.begin(), v.end(), std::ostream_iterator<float>(ss, " "));
  string s = ss.str();
  s = s.substr(0, s.length()-1);  // get rid of the trailing space
  return s;
}

double ParticleFilter::computeXf(double x0, double theta0, double stepTime, double v, double yr)
{
  double estimatedX;
  
  if (fabs(yr) > 0.0001)
  {
    //Xf = X0 + (v/yr)*(sin(theta0 + yr * stepTime) - sin(theta0))
    estimatedX = x0 + (v / yr) * ( sin(theta0 + yr * stepTime) - sin(theta0) );
  }
  else
  {
    // yaw rate is close to 0
    estimatedX = x0 + v * cos(theta0) * stepTime;
  }
  
  return estimatedX;
}

double ParticleFilter::computeYf(double y0, double theta0, double stepTime, double v, double yr)
{
  double estimatedY;
  
  if (fabs(yr) > 0.0001)
  {
    //Xf = X0 + (v/yr)*( cos(theta0) - cos(theta0 + yr * stepTime) )
    estimatedY = y0 + (v / yr) * ( cos(theta0) - cos(theta0 + yr * stepTime) );
  }
  else
  {
    // yaw rate is close to 0
    estimatedY = y0 + v * sin(theta0) * stepTime;
  }
  
  return estimatedY;
}

double ParticleFilter::computeThetaf(double theta0, double stepTime, double yr)
{
  double estimatedTheta;

  //Thetaf = theta0 + yr * stepTime
  estimatedTheta = theta0 + yr * stepTime;

  return estimatedTheta;
}
