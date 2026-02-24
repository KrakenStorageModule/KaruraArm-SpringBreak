# NewARM (Pranav's Version - Feb 2026) Installation & Run Instructions

## 1. Install Dependencies:
- **Docker Desktop**: [Download here](https://www.docker.com/products/docker-desktop/)
- **XLaunch**: [Download here](https://sourceforge.net/projects/vcxsrv/)

## 2. Before Running:
- Open the **Docker Desktop** app (it needs to be running for the setup to work).
- Open the **XLaunch** app:
  - When prompted for display number, input `0`.
  - Click "Yes" on all subsequent prompts until it closes.

## 3. Running in Terminal:
- Use a terminal in **VSCode** (or your preferred terminal). 

### Start Docker:

```bash
docker compose up --build -d
```

### Enter Docker Container:

```bash
docker exec -it new_arm bash
```

### Move to ros2_ws Directory:

```bash
cd simulation/ros2_ws
```

### Build Simulation:

```bash
colcon build
```

### Source Simulation Workspace:

```bash
source install/setup.bash
```

### Launch RViz:

```bash
ros2 launch moveit_config demo.launch.py
```