# Perspective Screen

This is a project I did during Summer 2022, it uses a [Microsoft Kinect V2](https://learn.microsoft.com/en-us/windows/apps/design/devices/kinect-for-windows) in order to create the illusion of a 3D scene behind the display

### How it works

The algorithm is broken down into the following steps:
- Determine the location of the person's eyes
- Place the virtual camera in a scene at the same 3D location as the persons eye's
- Have the virtual camera face towards the center of where the monitor is in 3D space
- Decrease the FOV of the virtual camera such that it encapsulates the entire monitor, this maximises the resolution
- Finally, take the generated frame and apply a homography map such that the pixels in the location of the monitor are mapped to the real-world monitor 
- Now the person should see a generated frame with the correct perspective from where they are standing

### Videos

The explanation above is all a bit abstract, I have some videos showing the [final result](https://drive.google.com/file/d/1__uwYf_GrsklGXCk1z6QY6p9KnvtMVZr/view?usp=sharing) and the [debug view](https://drive.google.com/file/d/1Vx69FNwSarvOqNNHVGU30G5w_ftesv4T/view?usp=sharing) from the application

### Problems

Here are some of the many problems that would improve the project if addressed:
- Development requires the Kinect to be plugged in all the time, this can be resolved by using the [ROS](https://github.com/ros2) framework, specifically the bagging feature to record data
- Currently this is Windows only, it can be made cross-platform by using the [Freenect Library](https://github.com/OpenKinect/libfreenect2)
- Eye position estimates using the Kinect SDK can be quite noisy (using classical CV methods), using a neural network such as [YOLO](https://github.com/ultralytics/ultralytics) to detect key-points on the face would be much more reliable
- The method of communication between the C++ program and the partner game engine ([Unity](https://github.com/aidan-ferguson/perspective-screen-unity) or Unreal) is not ideal. It uses a shared memory region but should use something such as IPC
- The C++ code was written quite a while ago and is in general need of a clean-up