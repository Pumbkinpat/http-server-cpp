# Http server with berkeley socket API in c++

## How to run:
- Open 2 terminal tabs
- Use this command to compile and run the server `g++ socket.cpp -o ./build/debug/socket && ./build/debug/socket`
- Use this command to send request `curl -i -X POST --http0.9 -d --output "hello world" http://localhost:8080`