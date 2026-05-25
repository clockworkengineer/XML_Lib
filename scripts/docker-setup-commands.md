# Docker Setup for XML_Lib

## Build the Docker image
From the repository root (`/path/to/XML_Lib`):

```bash
docker build -t xml_lib_ci -f ./scripts/dockerfile .
```

## Run a container with a shell
Mount the repository into the container and start an interactive shell:

```bash
docker run --rm -it -v "${PWD}:/home/jenkins" -w /home/jenkins xml_lib_ci bash
```

## Optional: Run Jenkins in the container
If you want Jenkins to start in detached mode and expose its default ports:

```bash
docker run -d -p 8080:8080 -p 50000:50000 --name xml_lib_jenkins xml_lib_ci
```

## Restart on reboot
To have the Jenkins container restart after the host reboots, use a restart policy:

```bash
docker run -d --restart unless-stopped -p 8080:8080 -p 50000:50000 --name xml_lib_jenkins xml_lib_ci
```

If the container already exists, update its restart policy:

```bash
docker update --restart unless-stopped xml_lib_jenkins
```

Also enable the Docker daemon to start on boot:

```bash
sudo systemctl enable docker
```

## Notes
- The Dockerfile installs `cmake`, `g++`, `make`, `openjdk-11-jdk`, and `jenkins`.
- It does not automatically build the project or configure Jenkins jobs.
- Use the mounted repo path `/home/jenkins` inside the container to build or run commands.
