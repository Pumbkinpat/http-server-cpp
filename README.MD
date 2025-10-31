# Http server with berkeley socket API in c++

## How to run:
- Open 2 terminal tabs
- Use this command to compile and run the server `g++ socket.cpp -o socket && ./socket`
- Use this command to send request `curl -X POST --http0.9 -d "hello world" http://localhost:8080`