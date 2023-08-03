#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <mutex>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;
using namespace chrono;

mutex logMutex;
mutex clientMutex;

void create_log(const string& log_file) {
    ofstream logStream(log_file, ios::trunc);
    if (logStream.is_open()) {
        logStream << "[log] Creating log file." << endl;
        logStream.close();
    }
}

class ClientThread {
public:
    ClientThread(const string& name) : name(name) {}

    void operator()() {
        lock_guard<mutex> lock(clientMutex);
        auto start_time = steady_clock::now();

        const char* host = "127.0.0.1";
        const int port = 9999;

        int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket < 0) {
            cerr << "Error creating socket" << endl;
            return;
        }

        struct sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        inet_pton(AF_INET, host, &(serverAddress.sin_addr));

        // Connect to the server
        if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
            cerr << "Connection Failed" << endl;
            close(clientSocket);
            return;
        }

        // Connection time
        auto conn_time = steady_clock::now();

        string message = name + ": hello C++";
        send_data_1(clientSocket, message);

        // Request time
        auto resp_time = steady_clock::now();

        close(clientSocket);

        auto close_time = steady_clock::now();
        double conn_time_cost = duration_cast<milliseconds>(conn_time - start_time).count();
        double req_time_cost = duration_cast<milliseconds>(resp_time - conn_time).count();
        double total_time_cost = duration_cast<milliseconds>(close_time - start_time).count();
        log("conn_time={"+to_string(conn_time_cost)+"}ms req_time_cost={"+to_string(req_time_cost)+"}ms total_time_cost={"+to_string(total_time_cost)+"}ms");
    }

private:
    string name;

    void send_data_1(int clientSocket, const string& data) {
        int cmd = 1;
        int data_len = data.length();
        string packed_data = string("work") + struct_pack("I", cmd) + struct_pack("I", data_len) + data;
        send(clientSocket, packed_data.c_str(), packed_data.length(), 0);
        char info[1024];
        recv(clientSocket, info, sizeof(info), 0);
        // cout << info << endl;
    }

    string struct_pack(const string& format, int value) {
        stringstream ss;
        ss << value;
        string packed_data = ss.str();
        while (packed_data.length() < format.length()) {
            packed_data = "0" + packed_data;
        }
        return packed_data;
    }

    void log(const string& message) {
        lock_guard<mutex> lock(logMutex);
        ofstream logStream("test.log", ios::app);
        if (logStream.is_open()) {
            auto now = system_clock::to_time_t(system_clock::now());
            logStream << put_time(localtime(&now), "%Y-%m-%d %X") << " " << message << endl;
            logStream.close();
        }
    }
};

int main() {
    string log_file = "test.log";
    ofstream logStream(log_file, ios::trunc);
    if (logStream.is_open()) {
        logStream << "[log] Creating log file." << endl;
        logStream.close();
    }

    // Read number of threads from config file
    int threads = 1000000;  // Default number of threads
    /* ifstream configFile("config/main.ini");
    if (configFile.is_open()) {
        string line;
        while (getline(configFile, line)) {
            if (line.find("threads") != string::npos) {
                threads = stoi(line.substr(line.find("=") + 1));
                break;
            }
        }
        configFile.close();
    } */

    vector<thread> thread_list;
    auto start_time = steady_clock::now();
    for (int i = 0; i < threads; i++) {
        ClientThread clientThread("thread" + to_string(i));
        thread_list.emplace_back(ClientThread("thread" + std::to_string(i)));
    }

    for (auto& thread : thread_list) {
        thread.join();
    }

    double total_time = duration_cast<seconds>(steady_clock::now() - start_time).count();
    cout << "thread finished, total time cost: " << total_time << "s" << endl;

    return 0;
}