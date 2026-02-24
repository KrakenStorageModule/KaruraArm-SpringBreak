🖥️ Windows Requirements (Docker + GUI)
1. Install Docker Desktop (Mandatory)

Download:
https://www.docker.com/products/docker-desktop/

Docker Desktop automatically:

- Installs and configures WSL2

- Provides the docker-desktop Linux VM

- Runs all containers

- Integrates with Windows networking & GPU

2. Install VcXsrv (XLaunch) for GUI Apps

Download:
https://sourceforge.net/projects/vcxsrv/

Launch with:

- Multiple windows

- Start no client

Enable:

☑ Disable access control

☑ Native OpenGL

☑ Clipboard

☑ Primary Selection

Allow all Windows Firewall prompts.

🐳 Build & Start the Container

Open Docker Desktop first (note: sometimes you must end all Docker tasks via Task Manager).

Open your X11 server (VcXsrv).
Alternative: run this in Windows terminal before using the container:

- 'xhost +local:docker'


From inside the AN-2026US directory:

docker compose up --build -d

🧭 Access the Container Terminal
docker exec -it rover bash


Inside the container:

cd ~/ros2_ws
colcon build
source install/setup.bash

🛑 Stop & Remove

Stop the environment:

docker compose down


💡 Tip: Stop Docker Desktop when not in use—it consumes background resources.

Remove the container:

docker rm -f rover


Remove the image:

docker rmi rover-dev:humble