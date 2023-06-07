# HTTP

An set of HTTP related tools written in C. These are: a server, a client (like curl), a proxy and a reverse proxy (like nginx).
All tools are built using CMake, and the server, proxy and reverse proxy have Dockerfiles so they can be containerised.

Right now, I am only working on the HTTP server, and the other components are **not implemented**, but I will implement them once I'm finished with the server.

## Compiling

This project uses CMake as its build system, and should be compiled this way.

To compile the project, change your working directory to the build directory with the following command:

```cd build```

Then, tell cmake to generate the buildfiles for the project with the following command:

``` cmake -DBUILD_TESTING=OFF .. ```

Compile the project using make by running the command:

```make```

Finally, install the executables and default configuration files using

```sudo make install```

(this step can be omitted, but you will have to manually create/copy the required configuration files to the correct locations)

The 4 components are individual CMake targets, and you can specify that CMake should omit a specific component by setting the BUILD_\<COMPONENT\> option to OFF when running the cmake command.

To build only the server:

``` bash
cmake -DBUILD_TESTING=OFF -DBUILD_CLIENT=OFF 
-DBUILD_PROXY=OFF -DBUILD_REVERSE_PROXY=OFF ..
```

To build the client and proxy:

``` bash
cmake -DBUILD_TESTING=OFF -DBUILD_SERVER=OFF
-DBUILD_REVERSE_PROXY=OFF ..
```

To build the server, reverse proxy and proxy:

``` bash
cmake -DBUILD_TESTING=OFF -DBUILD_CLIENT=OFF ..
```

## Docker

The server, proxy and reverse proxy can be built into docker images using the Dockerfiles contained in their respective directories. To build a component as a docker image, run the following command:

```docker build -t <image name> -f <component>/Dockerfile .```

where \<image name\> is the name of the docker image that will be built and \<component\> is the name of the directory corresponding to the component to be built (server, proxy, reverse_proxy).

You may need to run this command with ```sudo``` or add yourself to the docker group.

Once you have built the image, you can create and run a container using it with the following:

```docker run <image name>```

To expose a port from the container to the host, add the ```-p <host>:<container>``` where \<host\> and \<container\> are the ports to map on the host and container respectively (e.g. ```-p 80:8080``` exposes port 8080 from inside the container on port 80 on the host). For example, to expose the http proxy running on port 80 inside the container on port 8080 on the host, you would run the following:

```docker run -p 8080:80 http_proxy```

To share configuration and other important files (such as webpage source files) with the container, use the ```-v <host>:<container>``` to create a bind mount between a directory on the host and a directory on the container. For example, to share configuration files stored in a directory called ```./server_config``` with the http server running in a container you would create a bind mount to ```/etc/http_server``` in the container, like so: 

```docker run -v ./server_config:/etc/http_server http_server```

and to share webpage source code files stored in a directory called ```./web``` with the http server you would create a bind mount to the web server root (by default ```/var/www/html```) like so:

```docker run -v ./web:/var/www/html http_server```

You will likely want to do all three of these when running any of the applications, so typically to run the program you should execute something like:

```docker run -p 8080:8080 -v /etc/http_server:/etc/http_server -v /var/www/html:/var/www/html http_server```

I might add docker-compose files to automate this at some point. 

## Usage

### Server

The server loads its configuration from ```/etc/http_server/http_server.conf```. If this file does not exist it will look for ```/etc/http_server/http_server.default.conf```.

The ```http_server.conf``` file has a very simple syntax where every line is in the form:

```SETTING value```

To get an example of how this file should look, consult the default configuration file [here](/server/default/http_server.default.conf).

The available settings are:

|Setting|Value Format|Description|
|-------|------------|-----------|
|ADDR|IPv4 Address|The IPv4 Address of the network interface to listen on|
|PORT|Integer 1-65535|The TCP port to listen on|
|ROOT|Absolute Path|The root directory to serve files from|
|ERROR_DIR|Relative Path|The subdirectory where source code for error pages is stored|
|ROUTE|\<route path\> \<file path\>|Associate a URL path with a file relative to the root directory|
|ALLOW_UNROUTED_FILE_ACCESS|1\|0|Whether to allow access to files using their name directly|

It is recommended that ```ALLOW_UNROUTED_FILE_ACCESS``` is set to `1` as otherwise all files which need to be accessed on the web server must have an explicit route defined (for example, a route must be created to every single image file or CSS stylesheet that needs to be loaded).

When run, the server will simply listen for HTTP requests and respond with the contents of the file with the corresponding route/filename in the root directory.
