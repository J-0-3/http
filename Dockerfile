FROM ubuntu:22.04
LABEL maintainer "Joseph Thomas"
RUN apt update -y
RUN apt install -y build-essential cmake libssl-dev libssl3
RUN mkdir /app
RUN groupadd -r httpserver
RUN useradd -r -g httpserver httpserver
RUN chown -R httpserver:httpserver /app
USER httpserver
WORKDIR /app
COPY src/ ./src
COPY include/ ./include
COPY tests/ ./tests
COPY CMakeLists.txt .
RUN mkdir /app/build
WORKDIR /app/build
RUN cmake ..
RUN make test_http_communication_server
EXPOSE 8000
CMD /app/build/test_http_communication_server
