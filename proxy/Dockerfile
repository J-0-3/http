FROM alpine:3.18 AS builder
RUN apk add --update --no-cache build-base cmake
RUN mkdir /app
WORKDIR /app
RUN mkdir /app/shared_lib
RUN mkdir /app/proxy
# don't copy test directories
COPY ./CMakeLists.txt /app
COPY ./shared_lib/src /app/shared_lib/src
COPY ./shared_lib/include /app/shared_lib/include
COPY ./shared_lib/CMakeLists.txt /app/shared_lib
COPY ./proxy/src /app/proxy/src
COPY ./proxy/include /app/proxy/include
COPY ./proxy/CMakeLists.txt /app/proxy

RUN mkdir /app/build
WORKDIR /app/build
RUN cmake -DBUILD_TESTING=OFF -DBUILD_CLIENT=OFF -DBUILD_PROXY=OFF -DBUILD_SERVER=OFF .. 
RUN make http_proxy

FROM alpine:latest 
RUN mkdir /app
COPY --from=builder /app/build/proxy/http_proxy /app
EXPOSE 8000
CMD ["/app/http_proxy"]