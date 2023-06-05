FROM alpine:3.18 AS builder
RUN apk add --update --no-cache build-base cmake
RUN mkdir /app
WORKDIR /app
RUN mkdir /app/shared_lib
RUN mkdir /app/reverse_proxy
# don't copy test directories
COPY ./CMakeLists.txt /app
COPY ./shared_lib/src /app/shared_lib/src
COPY ./shared_lib/include /app/shared_lib/include
COPY ./shared_lib/CMakeLists.txt /app/shared_lib
COPY ./reverse_proxy/src /app/reverse_proxy/src
COPY ./reverse_proxy/include /app/reverse_proxy/include
COPY ./reverse_proxy/CMakeLists.txt /app/reverse_proxy

RUN mkdir /app/build
WORKDIR /app/build
RUN cmake -DBUILD_TESTING=OFF -DBUILD_CLIENT=OFF -DBUILD_PROXY=OFF -DBUILD_SERVER=OFF .. 
RUN make http_reverse_proxy

FROM alpine:latest 
RUN mkdir /app
COPY --from=builder /app/build/reverse_proxy/http_reverse_proxy /app
EXPOSE 8000
CMD ["/app/http_reverse_proxy"]