FROM drpilman/gpp_builder:alpine as builder
WORKDIR /app
ARG SERVER_VERSION
COPY server${SERVER_VERSION}.cpp server.cpp server.hpp .
RUN g++ server${SERVER_VERSION}.cpp server.cpp -o server

FROM drpilman/gpp_executor:alpine as executor
COPY --from=builder /app/server /app/server
ENTRYPOINT /app/server