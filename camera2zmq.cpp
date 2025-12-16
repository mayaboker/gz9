/*
 * Gazebo Camera to ZMQ Publisher with MsgPack
 * Subscribes to Gazebo camera image topic and publishes via ZMQ using msgpack
 */

#include <gazebo/transport/transport.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/gazebo_client.hh>
#include <zmq.hpp>
#include <opencv2/opencv.hpp>
#include <msgpack.hpp>
#include <iostream>
#include <cstring>
#include <map>
#include <vector>

// Global ZMQ publisher
zmq::context_t g_context(1);
zmq::socket_t g_publisher(g_context, zmq::socket_type::pub);

// Global msgpack topic name - needs to be accessible from callback function
std::string g_msgpack_topic = "camera/image";

/////////////////////////////////////////////////
// Callback function called when image is received
void onImageMsg(ConstImageStampedPtr &_msg)
{
    // Extract image data from Gazebo message
    int width = _msg->image().width();
    int height = _msg->image().height();
    int pixel_format = _msg->image().pixel_format();
    
    std::cout << "Received image: " << width << "x" << height 
              << " format: " << pixel_format << std::endl;
    
    // Convert Gazebo image to OpenCV Mat
    cv::Mat img;
    
    // Gazebo pixel formats: L_INT8 = 1, BGR_INT8 = 3, RGB_INT8 = 4
    if ((pixel_format == 3) || (pixel_format == 0)) // BGR_INT8 (most common in Gazebo)
    {
        // BGR 8-bit format - already in OpenCV format!
        img = cv::Mat(height, width, CV_8UC3);
        const char* data = _msg->image().data().c_str();
        std::memcpy(img.data, data, height * width * 3);
    }
    else if (pixel_format == 4) // RGB_INT8
    {
        // RGB 8-bit format
        img = cv::Mat(height, width, CV_8UC3);
        const char* data = _msg->image().data().c_str();
        std::memcpy(img.data, data, height * width * 3);
        
        // Convert RGB to BGR for OpenCV
        cv::cvtColor(img, img, cv::COLOR_RGB2BGR);
    }
    else if (pixel_format == 1) // L_INT8 (grayscale)
    {
        // Grayscale 8-bit format
        img = cv::Mat(height, width, CV_8UC1);
        const char* data = _msg->image().data().c_str();
        std::memcpy(img.data, data, height * width);
    }
    else
    {
        std::cerr << "Unsupported pixel format: " << pixel_format << std::endl;
        std::cerr << "Known formats: 1=L_INT8, 3=BGR_INT8, 4=RGB_INT8" << std::endl;
        return;
    }
    
    // Convert to grayscale if not already grayscale, then back to 3-channel BGR
    // This matches the Python code: BGR -> GRAY -> BGR
    if (img.channels() == 3) {
        cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
        cv::cvtColor(img, img, cv::COLOR_GRAY2BGR);
        std::cout << "Converted to grayscale (3-channel)" << std::endl;
    } else if (img.channels() == 1) {
        // If already grayscale, convert to 3-channel BGR
        cv::cvtColor(img, img, cv::COLOR_GRAY2BGR);
        std::cout << "Converted grayscale to 3-channel BGR" << std::endl;
    }
    
    // Convert frame to bytes (matching Python's frame.tobytes())
    std::vector<unsigned char> frame_bytes(img.data, img.data + img.total() * img.elemSize());
    
    // Pack with msgpack (matching Python's msgpack.packb(frame))
    msgpack::sbuffer sbuf;
    msgpack::packer<msgpack::sbuffer> pk(&sbuf);
    pk.pack(frame_bytes);
    
    // Send as ZMQ multipart message: (TOPIC, data)
    // Matching Python's socket.send_multipart((TOPIC, data))
    zmq::message_t topic_msg(g_msgpack_topic.size());
    std::memcpy(topic_msg.data(), g_msgpack_topic.c_str(), g_msgpack_topic.size());
    
    zmq::message_t data_msg(sbuf.size());
    std::memcpy(data_msg.data(), sbuf.data(), sbuf.size());
    
    g_publisher.send(topic_msg, ZMQ_SNDMORE);
    g_publisher.send(data_msg);
    // g_publisher.send(topic_msg, zmq::send_flags::sndmore);
    // g_publisher.send(data_msg, zmq::send_flags::none);
    
    std::cout << "Published frame via ZMQ multipart: " << img.rows << "x" << img.cols 
              << " topic: " << g_msgpack_topic << std::endl;
}

/////////////////////////////////////////////////
int main(int _argc, char **_argv)
{
    std::string camera_topic = "/gazebo/default/iris_demo/iris_demo/gimbal_small_2d/tilt_link/camera/image";
    std::string zmq_address = "tcp://*:5556";
    
    // Parse command line arguments
    if (_argc > 1) {
        camera_topic = _argv[1];
    }
    if (_argc > 2) {
        zmq_address = _argv[2];
    }
    if (_argc > 3) {
        g_msgpack_topic = _argv[3];
    }
    
    std::cout << "Camera2ZMQ Publisher (MsgPack)\n";
    std::cout << "==============================\n";
    std::cout << "Gazebo topic: " << camera_topic << "\n";
    std::cout << "ZMQ address: " << zmq_address << "\n";
    std::cout << "MsgPack topic: " << g_msgpack_topic << "\n\n";
    
    std::cout << "Usage: " << _argv[0] << " [gazebo_topic] [zmq_address] [msgpack_topic]\n";
    std::cout << "Defaults:\n";
    std::cout << "  gazebo_topic: /gazebo/default/iris_demo/iris_demo/gimbal_small_2d/tilt_link/camera/image\n";
    std::cout << "  zmq_address: tcp://*:5556\n";
    std::cout << "  msgpack_topic: camera/image\n\n";
    
    // Setup ZMQ publisher
    g_publisher.bind(zmq_address);
    std::cout << "ZMQ publisher bound to " << zmq_address << "\n";
    
    // Load Gazebo client
    gazebo::client::setup(_argc, _argv);
    
    // Create Gazebo transport node
    gazebo::transport::NodePtr node(new gazebo::transport::Node());
    node->Init();
    
    // Subscribe to camera topic
    gazebo::transport::SubscriberPtr sub = 
        node->Subscribe(camera_topic, onImageMsg);
    
    std::cout << "Subscribed to Gazebo camera topic\n";
    std::cout << "Waiting for images...\n\n";
    
    // Main loop
    while (true)
        gazebo::common::Time::MSleep(10);
    
    // Cleanup
    gazebo::client::shutdown();
    return 0;
}