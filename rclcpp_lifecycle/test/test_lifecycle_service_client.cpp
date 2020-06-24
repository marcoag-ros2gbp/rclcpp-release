// Copyright 2020 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * Service client test was adopted from:
 * https://github.com/ros2/demos/blob/master/lifecycle/src/lifecycle_service_client.cpp
 */

#include <gtest/gtest.h>
#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "lifecycle_msgs/msg/state.hpp"
#include "lifecycle_msgs/msg/transition.hpp"
#include "lifecycle_msgs/msg/transition_description.hpp"
#include "lifecycle_msgs/srv/change_state.hpp"
#include "lifecycle_msgs/srv/get_available_states.hpp"
#include "lifecycle_msgs/srv/get_available_transitions.hpp"
#include "lifecycle_msgs/srv/get_state.hpp"

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"

using namespace std::chrono_literals;

constexpr char const * lifecycle_node_name = "lc_talker";

constexpr char const * node_get_state_topic = "/lc_talker/get_state";
constexpr char const * node_change_state_topic = "/lc_talker/change_state";
constexpr char const * node_get_available_states_topic = "/lc_talker/get_available_states";
constexpr char const * node_get_available_transitions_topic =
  "/lc_talker/get_available_transitions";
constexpr char const * node_get_transition_graph_topic =
  "/lc_talker/get_transition_graph";
const lifecycle_msgs::msg::State unknown_state = lifecycle_msgs::msg::State();

class EmptyLifecycleNode : public rclcpp_lifecycle::LifecycleNode
{
public:
  EmptyLifecycleNode()
  : rclcpp_lifecycle::LifecycleNode(lifecycle_node_name)
  {}
};

class LifecycleServiceClient : public rclcpp::Node
{
public:
  explicit LifecycleServiceClient(std::string node_name)
  : Node(node_name)
  {
    client_get_available_states_ = this->create_client<lifecycle_msgs::srv::GetAvailableStates>(
      node_get_available_states_topic);
    client_get_available_transitions_ =
      this->create_client<lifecycle_msgs::srv::GetAvailableTransitions>(
      node_get_available_transitions_topic);
    client_get_transition_graph_ =
      this->create_client<lifecycle_msgs::srv::GetAvailableTransitions>(
      node_get_transition_graph_topic);
    client_get_state_ = this->create_client<lifecycle_msgs::srv::GetState>(
      node_get_state_topic);
    client_change_state_ = this->create_client<lifecycle_msgs::srv::ChangeState>(
      node_change_state_topic);
  }

  lifecycle_msgs::msg::State
  get_state(std::chrono::seconds time_out = 1s)
  {
    auto request = std::make_shared<lifecycle_msgs::srv::GetState::Request>();

    if (!client_get_state_->wait_for_service(time_out)) {
      return unknown_state;
    }

    auto future_result = client_get_state_->async_send_request(request);
    auto future_status = future_result.wait_for(time_out);

    if (future_status != std::future_status::ready) {
      return unknown_state;
    }

    if (future_result.get()) {
      return future_result.get()->current_state;
    } else {
      return unknown_state;
    }
  }

  bool
  change_state(std::uint8_t transition, std::chrono::seconds time_out = 1s)
  {
    auto request = std::make_shared<lifecycle_msgs::srv::ChangeState::Request>();
    request->transition.id = transition;

    if (!client_change_state_->wait_for_service(time_out)) {
      return false;
    }

    auto future_result = client_change_state_->async_send_request(request);
    auto future_status = future_result.wait_for(time_out);

    if (future_status != std::future_status::ready) {
      return false;
    }

    return future_result.get()->success;
  }

  std::vector<lifecycle_msgs::msg::State>
  get_available_states(std::chrono::seconds time_out = 1s)
  {
    auto request = std::make_shared<lifecycle_msgs::srv::GetAvailableStates::Request>();

    if (!client_get_available_states_->wait_for_service(time_out)) {
      return std::vector<lifecycle_msgs::msg::State>();
    }

    auto future_result = client_get_available_states_->async_send_request(request);
    auto future_status = future_result.wait_for(time_out);

    if (future_status != std::future_status::ready) {
      return std::vector<lifecycle_msgs::msg::State>();
    }

    if (future_result.get()) {
      return future_result.get()->available_states;
    }

    return std::vector<lifecycle_msgs::msg::State>();
  }

  std::vector<lifecycle_msgs::msg::TransitionDescription>
  get_available_transitions(std::chrono::seconds time_out = 1s)
  {
    auto request = std::make_shared<lifecycle_msgs::srv::GetAvailableTransitions::Request>();

    if (!client_get_available_transitions_->wait_for_service(time_out)) {
      return std::vector<lifecycle_msgs::msg::TransitionDescription>();
    }

    auto future_result = client_get_available_transitions_->async_send_request(request);
    auto future_status = future_result.wait_for(time_out);

    if (future_status != std::future_status::ready) {
      return std::vector<lifecycle_msgs::msg::TransitionDescription>();
    }

    if (future_result.get()) {
      return future_result.get()->available_transitions;
    }

    return std::vector<lifecycle_msgs::msg::TransitionDescription>();
  }

  std::vector<lifecycle_msgs::msg::TransitionDescription>
  get_transition_graph(std::chrono::seconds time_out = 1s)
  {
    auto request = std::make_shared<lifecycle_msgs::srv::GetAvailableTransitions::Request>();

    if (!client_get_transition_graph_->wait_for_service(time_out)) {
      return std::vector<lifecycle_msgs::msg::TransitionDescription>();
    }

    auto future_result = client_get_transition_graph_->async_send_request(request);
    auto future_status = future_result.wait_for(time_out);

    if (future_status != std::future_status::ready) {
      return std::vector<lifecycle_msgs::msg::TransitionDescription>();
    }

    if (future_result.get()) {
      return future_result.get()->available_transitions;
    }

    return std::vector<lifecycle_msgs::msg::TransitionDescription>();
  }

private:
  std::shared_ptr<rclcpp::Client<lifecycle_msgs::srv::GetAvailableStates>>
  client_get_available_states_;
  std::shared_ptr<rclcpp::Client<lifecycle_msgs::srv::GetAvailableTransitions>>
  client_get_available_transitions_;
  std::shared_ptr<rclcpp::Client<lifecycle_msgs::srv::GetAvailableTransitions>>
  client_get_transition_graph_;
  std::shared_ptr<rclcpp::Client<lifecycle_msgs::srv::GetState>> client_get_state_;
  std::shared_ptr<rclcpp::Client<lifecycle_msgs::srv::ChangeState>> client_change_state_;
};


class TestLifecycleServiceClient : public ::testing::Test
{
protected:
  EmptyLifecycleNode * lifecycle_node() {return lifecycle_node_.get();}
  LifecycleServiceClient * lifecycle_client() {return lifecycle_client_.get();}

private:
  void SetUp() override
  {
    rclcpp::init(0, nullptr);
    lifecycle_node_ = std::make_shared<EmptyLifecycleNode>();
    lifecycle_client_ = std::make_shared<LifecycleServiceClient>("client");
    spinner_ = std::thread(&TestLifecycleServiceClient::spin, this);
  }

  void TearDown() override
  {
    rclcpp::shutdown();
    spinner_.join();
  }

  void spin()
  {
    while (rclcpp::ok()) {
      rclcpp::spin_some(lifecycle_node_->get_node_base_interface());
      rclcpp::spin_some(lifecycle_client_);
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  std::shared_ptr<EmptyLifecycleNode> lifecycle_node_;
  std::shared_ptr<LifecycleServiceClient> lifecycle_client_;
  std::thread spinner_;
};


TEST_F(TestLifecycleServiceClient, construct_destruct) {
  EXPECT_NE(nullptr, lifecycle_client());
  EXPECT_NE(nullptr, lifecycle_node());
}

TEST_F(TestLifecycleServiceClient, available_states) {
  auto states = lifecycle_client()->get_available_states();
  EXPECT_EQ(states.size(), 11u);
  EXPECT_EQ(states[0].id, lifecycle_msgs::msg::State::PRIMARY_STATE_UNKNOWN);
  EXPECT_EQ(states[1].id, lifecycle_msgs::msg::State::PRIMARY_STATE_UNCONFIGURED);
  EXPECT_EQ(states[2].id, lifecycle_msgs::msg::State::PRIMARY_STATE_INACTIVE);
  EXPECT_EQ(states[3].id, lifecycle_msgs::msg::State::PRIMARY_STATE_ACTIVE);
  EXPECT_EQ(states[4].id, lifecycle_msgs::msg::State::PRIMARY_STATE_FINALIZED);
  EXPECT_EQ(states[5].id, lifecycle_msgs::msg::State::TRANSITION_STATE_CONFIGURING);
  EXPECT_EQ(states[6].id, lifecycle_msgs::msg::State::TRANSITION_STATE_CLEANINGUP);
  EXPECT_EQ(states[7].id, lifecycle_msgs::msg::State::TRANSITION_STATE_SHUTTINGDOWN);
  EXPECT_EQ(states[8].id, lifecycle_msgs::msg::State::TRANSITION_STATE_ACTIVATING);
  EXPECT_EQ(states[9].id, lifecycle_msgs::msg::State::TRANSITION_STATE_DEACTIVATING);
  EXPECT_EQ(states[10].id, lifecycle_msgs::msg::State::TRANSITION_STATE_ERRORPROCESSING);
}

TEST_F(TestLifecycleServiceClient, transition_graph) {
  auto transitions = lifecycle_client()->get_transition_graph();
  EXPECT_EQ(transitions.size(), 25u);
}

TEST_F(TestLifecycleServiceClient, available_transitions) {
  auto transitions = lifecycle_client()->get_available_transitions();
  EXPECT_EQ(transitions.size(), 2u);
  EXPECT_EQ(transitions[0].transition.id, lifecycle_msgs::msg::Transition::TRANSITION_CONFIGURE);
  EXPECT_EQ(
    transitions[1].transition.id,
    lifecycle_msgs::msg::Transition::TRANSITION_UNCONFIGURED_SHUTDOWN);
}

TEST_F(TestLifecycleServiceClient, lifecycle_transitions) {
  EXPECT_EQ(
    lifecycle_client()->get_state().id, lifecycle_msgs::msg::State::PRIMARY_STATE_UNCONFIGURED);

  auto transitions = lifecycle_client()->get_available_transitions();
  EXPECT_EQ(transitions.size(), 2u);
  EXPECT_EQ(transitions[0].transition.id, lifecycle_msgs::msg::Transition::TRANSITION_CONFIGURE);
  EXPECT_EQ(
    transitions[1].transition.id,
    lifecycle_msgs::msg::Transition::TRANSITION_UNCONFIGURED_SHUTDOWN);

  EXPECT_TRUE(
    lifecycle_client()->change_state(
      lifecycle_msgs::msg::Transition::TRANSITION_CONFIGURE));
  EXPECT_EQ(
    lifecycle_client()->get_state().id,
    lifecycle_msgs::msg::State::PRIMARY_STATE_INACTIVE);
  transitions = lifecycle_client()->get_available_transitions();
  EXPECT_EQ(transitions.size(), 3u);
  EXPECT_EQ(transitions[0].transition.id, lifecycle_msgs::msg::Transition::TRANSITION_CLEANUP);
  EXPECT_EQ(transitions[1].transition.id, lifecycle_msgs::msg::Transition::TRANSITION_ACTIVATE);
  EXPECT_EQ(
    transitions[2].transition.id,
    lifecycle_msgs::msg::Transition::TRANSITION_INACTIVE_SHUTDOWN);

  EXPECT_TRUE(
    lifecycle_client()->change_state(
      lifecycle_msgs::msg::Transition::TRANSITION_ACTIVATE));
  EXPECT_EQ(lifecycle_client()->get_state().id, lifecycle_msgs::msg::State::PRIMARY_STATE_ACTIVE);
  transitions = lifecycle_client()->get_available_transitions();
  EXPECT_EQ(transitions.size(), 2u);
  EXPECT_EQ(transitions[0].transition.id, lifecycle_msgs::msg::Transition::TRANSITION_DEACTIVATE);
  EXPECT_EQ(
    transitions[1].transition.id,
    lifecycle_msgs::msg::Transition::TRANSITION_ACTIVE_SHUTDOWN);

  EXPECT_TRUE(
    lifecycle_client()->change_state(
      lifecycle_msgs::msg::Transition::
      TRANSITION_DEACTIVATE));
  EXPECT_EQ(
    lifecycle_client()->get_state().id,
    lifecycle_msgs::msg::State::PRIMARY_STATE_INACTIVE);
  transitions = lifecycle_client()->get_available_transitions();
  EXPECT_EQ(transitions.size(), 3u);
  EXPECT_EQ(transitions[0].transition.id, lifecycle_msgs::msg::Transition::TRANSITION_CLEANUP);
  EXPECT_EQ(transitions[1].transition.id, lifecycle_msgs::msg::Transition::TRANSITION_ACTIVATE);
  EXPECT_EQ(
    transitions[2].transition.id,
    lifecycle_msgs::msg::Transition::TRANSITION_INACTIVE_SHUTDOWN);

  EXPECT_TRUE(
    lifecycle_client()->change_state(
      lifecycle_msgs::msg::Transition::TRANSITION_CLEANUP));
  EXPECT_EQ(
    lifecycle_client()->get_state().id, lifecycle_msgs::msg::State::PRIMARY_STATE_UNCONFIGURED);
  transitions = lifecycle_client()->get_available_transitions();
  EXPECT_EQ(transitions.size(), 2u);
  EXPECT_EQ(transitions[0].transition.id, lifecycle_msgs::msg::Transition::TRANSITION_CONFIGURE);
  EXPECT_EQ(
    transitions[1].transition.id,
    lifecycle_msgs::msg::Transition::TRANSITION_UNCONFIGURED_SHUTDOWN);

  EXPECT_TRUE(
    lifecycle_client()->change_state(
      lifecycle_msgs::msg::Transition::
      TRANSITION_UNCONFIGURED_SHUTDOWN));
  EXPECT_EQ(
    lifecycle_client()->get_state().id,
    lifecycle_msgs::msg::State::PRIMARY_STATE_FINALIZED);
  transitions = lifecycle_client()->get_available_transitions();
  EXPECT_EQ(transitions.size(), 0u);
}
