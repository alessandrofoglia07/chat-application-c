<h1 align="center">
  <br>
  Chat Application
</h1>

<h4 align="center">Chat application written in C</h4>

<p align="center">
  <a href="#key-features">Key Features</a> •
  <a href="#how-to-use">How To Use</a> •
  <a href="#demo">Demo</a> •
  <a href="#license">License</a>
</p>

## Key Features

- **Multi-Client Support**: Allows multiple clients to connect and communicate with each other simultaneously.
- **Real-Time Messaging**: Enables real-time message exchange between clients.
- **Threaded Client Handling**: Utilizes multi-threading to handle multiple clients efficiently.
- **Command Support**: Provides built-in commands such as /help, /send, /clear, and /quit for enhanced user interaction.
- **Broadcast Messaging**: Supports broadcasting messages to all connected clients or excluding the sender.
- **Graceful Exit**: Allows clients to exit the chat gracefully by typing "exit".
- **Error Handling**: Provides appropriate error messages and responses for invalid requests or missing files.
- **Simple Setup**: Minimal dependencies and easy setup for running and testing the server and client locally.

## How To Use

To clone and run this application, you'll need [Git](https://git-scm.com) and [CMake](https://cmake.org/)
installed on your computer. From your command line:

```bash
# Clone this repository
$ git clone https://github.com/alessandrofoglia07/chat-application-c

# Go into the repository
$ cd chat-application-c
```

The app is divided into two main components: the server and the client. The server is responsible for handling client
connections and messages, while the client is used to connect to the server and send/receive messages.

You can run the server like this:

```bash
# Build the server
$ cmake --build ./cmake-build-debug --target server -- -j 6
# Run the server
$ ./build/server
```

Note: You can edit the default configuration in the `common.h` file to change the server IP address, port number, and
other settings.

Once the server is running, you can connect to it using the client. You can run multiple instances of the client to
simulate multiple users chatting with each other.

You can run the client like this:

```bash
# Build the client
$ cmake --build ./cmake-build-debug --target client -- -j 6
# Run the client
$ ./build/client
```

## Demo

<a href="https://github.com/alessandrofoglia07/chat-application-c/raw/main/.github/C-Chat-App-Demo.mp4">
  <video src="https://github.com/alessandrofoglia07/chat-application-c/raw/main/.github/C-Chat-App-Demo.mp4"></video>
</a>

## License

MIT

---

> GitHub [@alessandrofoglia07](https://github.com/alessandrofoglia07) &nbsp;&middot;&nbsp;
> StackOverflow [@Alexxino](https://stackoverflow.com/users/21306952/alexxino) &nbsp;&middot;&nbsp;
> NPM [alessandrofoglia07](https://www.npmjs.com/~alessandrofoglia07)
