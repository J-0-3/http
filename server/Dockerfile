FROM alpine:3.18 AS builder
RUN apk add --update --no-cache build-base cmake
RUN mkdir /app
WORKDIR /app
RUN mkdir /app/shared_lib
RUN mkdir /app/server
# don't copy test directories
COPY ./CMakeLists.txt /app
COPY ./shared_lib/src /app/shared_lib/src
COPY ./shared_lib/include /app/shared_lib/include
COPY ./shared_lib/CMakeLists.txt /app/shared_lib
COPY ./server/src /app/server/src
COPY ./server/include /app/server/include
COPY ./server/default /app/server/default
COPY ./server/CMakeLists.txt /app/server

RUN mkdir /app/build
WORKDIR /app/build
RUN cmake -DBUILD_TESTING=OFF -DBUILD_CLIENT=OFF -DBUILD_PROXY=OFF -DBUILD_REVERSE_PROXY=OFF .. 
RUN make
RUN make install

FROM alpine:latest 
RUN mkdir /app
COPY --from=builder /app/build/server/http_server /app
COPY --from=builder /etc/http_server/ /etc/http_server
COPY --from=builder /var/www/html/ /var/www/html
CMD ["/app/http_server"]