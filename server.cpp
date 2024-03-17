#include <msgpack.hpp>
#include <vector>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <stdexcept> 

int main() {

    // Create a socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        std::cerr << "Error creating socket\n";
        return 1;
    }

    // Bind the socket to a port
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8080); 

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Error binding socket\n";
        return 1;
    }

    listen(server_socket, 5);

    int count = 0;

    while (true) {
        // Accept incoming connection
        int client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket < 0) {
            std::cerr << "Error accepting connection\n";
            return 1;
        }

        int x_bytes = 589829;
        char buffer[589829];

        // Receive data from the client
        std::string received_data = "";

        while (received_data.length() < x_bytes) {
            char chunk[x_bytes - received_data.length()];
            int bytes_received = recv(client_socket, chunk, x_bytes - received_data.length(), 0);

            if (bytes_received <= 0) {
                std::cerr << "Error receiving data\n";
                close(client_socket);
                return 1;
            }
            received_data += std::string(chunk, bytes_received);
        }

        // Copy received data to buffer before unpacking
        std::copy(received_data.begin(), received_data.end(), buffer);

        // Create a vector to store received numbers
        std::vector<float> received_numbers;
        // Deserialize the received data
        msgpack::unpacked msg;
        msgpack::unpack(msg, buffer, received_data.size());
        msg.get().convert(received_numbers);

        count = count + 1;

        float sum = 0;
        for (const auto& num : received_numbers) {
            sum += num;
        }


        std::cout << "Sum of received numbers: " << sum << std::endl;
        std::cout << "Count = " << count << std::endl;


        std::vector<float> num(128 * 128 * 4, 1.0f); // Your data

        // Pack data
        std::stringstream packed_data_stream;
        msgpack::pack(packed_data_stream, num);
        std::string packed_data = packed_data_stream.str();

        // Send data
        while (!packed_data.empty()) {

            ssize_t sent = send(client_socket, packed_data.data(), packed_data.size(), 0);
            if (sent < 0) {
                std::cerr << "Error sending data\n";
                return 1;
            }
            packed_data = packed_data.substr(sent);
        }

        // Close client socket
        // close(client_socket);
    }

    // Close server socket (never reached in this example)
    // close(server_socket);

    return 0;
}
