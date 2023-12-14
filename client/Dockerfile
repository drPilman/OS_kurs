FROM drpilman/gpp_builder:alpine as builder
WORKDIR /app
COPY client.cpp .
RUN g++ client.cpp -o client

FROM drpilman/gpp_executor:alpine as executor
COPY --from=builder /app/client /app/client
CMD sh -c /app/client