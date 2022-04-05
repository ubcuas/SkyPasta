Make sure you are running these command in the NGINX folder and not the SkyPasta folder.

# Build
docker build . -t ubcuas/nginx

# Run
docker run -it --mount src="$(pwd)/../Images",target=/Images,type=bind -p 8080:80 ubcuas/nginx

This runs NGINX on http://localhost:8080 outside of the docker container

# Debug
Run the container in the following manner:
docker run -it -d ubcuas/nginx sh

If you would like to attach to an already running container:
docker exec -it CONTAINER_ID sh

Stop Apache:
sudo /etc/init.d/apache2 stop
sudo service apache2 stop
