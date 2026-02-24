# =====================================================================
# Industry-standard base image for ROS2 Humble + Gazebo Classic
# =====================================================================
FROM osrf/ros:humble-desktop-full

# -------------------------------------------------------
# Install Gazebo Classic + ROS2 Gazebo bindings
# -------------------------------------------------------
RUN apt-get update && apt-get install -y \
    gazebo \
    ros-humble-gazebo-ros \
    ros-humble-gazebo-ros-pkgs \
    ros-humble-gazebo-plugins \
    ros-humble-ros2-control \
    ros-humble-ros2-controllers \
    ros-humble-controller-manager \
    ros-humble-diff-drive-controller \
    ros-humble-joint-state-broadcaster \
    ros-humble-joint-state-publisher \
    ros-humble-joint-state-publisher-gui \
    ros-humble-teleop-twist-keyboard \
    ros-humble-xacro \
    ros-humble-moveit \
    ros-humble-moveit-servo \
    ros-humble-moveit-visual-tools \
    ros-humble-moveit-setup-assistant \
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
RUN groupadd -g $GID $USERNAME && \
    useradd -m -u $UID -g $GID -s /bin/bash $USERNAME && \
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
RUN pip3 install pynput

# -------------------------------------------------------
# Gazebo GUI fix for NVIDIA / OpenGL
# -------------------------------------------------------
ENV QT_X11_NO_MITSHM=1

# -------------------------------------------------------
# Entry point (bash)
# -------------------------------------------------------
CMD ["bash"]
