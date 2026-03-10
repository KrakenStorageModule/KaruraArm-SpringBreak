# =====================================================================
# Industry-standard base image for ROS2 Humble + Gazebo Classic
# =====================================================================
FROM osrf/ros:jazzy-desktop

# -------------------------------------------------------
# Install Gazebo Classic + ROS2 Gazebo bindings
# -------------------------------------------------------
RUN apt-get update && apt-get install -y \
    ros-jazzy-ros-gz \
    ros-jazzy-gz-ros2-control \
    ros-jazzy-ros2-control \
    ros-jazzy-ros2-controllers \
    ros-jazzy-controller-manager \
    ros-jazzy-diff-drive-controller \
    ros-jazzy-joint-state-broadcaster \
    ros-jazzy-joint-state-publisher \
    ros-jazzy-joint-state-publisher-gui \
    ros-jazzy-teleop-twist-keyboard \
    ros-jazzy-xacro \
    ros-jazzy-moveit \
    ros-jazzy-moveit-servo \
    ros-jazzy-moveit-visual-tools \
    ros-jazzy-moveit-setup-assistant \
    python3-colcon-common-extensions \
    python3-pip \
    bash-completion \
    && rm -rf /var/lib/apt/lists/*


# -------------------------------------------------------
# Create non-root user so Gazebo GUI + RVIZ work properly
# -------------------------------------------------------
ARG USERNAME=ros
ARG UID=1000
ARG GID=1000
RUN usermod -l $USERNAME ubuntu && \
    groupmod -n $USERNAME ubuntu && \
    usermod -d /home/$USERNAME -m $USERNAME && \
    echo "$USERNAME ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers


USER $USERNAME
WORKDIR /home/$USERNAME

# -------------------------------------------------------
# Set working directory to your mounted repo
# -------------------------------------------------------
WORKDIR /home/$USERNAME/NewARM

# -------------------------------------------------------
# Automatically source ROS + workspace
# -------------------------------------------------------
RUN echo "source /opt/ros/humble/setup.bash" >> ~/.bashrc

# -------------------------------------------------------
# Install Python package globally
# -------------------------------------------------------
RUN pip3 install pynput --break-system-packages

# -------------------------------------------------------
# Gazebo GUI fix for NVIDIA / OpenGL
# -------------------------------------------------------
ENV QT_X11_NO_MITSHM=1

# -------------------------------------------------------
# Entry point (bash)
# -------------------------------------------------------
CMD ["bash"]
